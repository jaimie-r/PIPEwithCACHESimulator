/**************************************************************************
 * STUDENTS: DO NOT MODIFY.
 * 
 * C S 429 system emulator
 * 
 * proc.c - The top-level instruction processing loop of the processor.
 * 
 * Copyright (c) 2022, 2023. 
 * Authors: S. Chatterjee, Z. Leeper. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include "archsim.h"
#include "hw_elts.h"
#include "hazard_control.h"

extern uint32_t bitfield_u32(int32_t src, unsigned frompos, unsigned width);
extern int64_t bitfield_s64(int32_t src, unsigned frompos, unsigned width);

extern machine_t guest;
extern bool X_condval;
extern uint64_t F_PC;
extern mem_status_t dmem_status;

int runElf(const uint64_t entry) {
    logging(LOG_INFO, "Running ELF executable");
    guest.proc->PC.bits->xval = entry;
    guest.proc->SP.bits->xval = guest.mem->seg_start_addr[STACK_SEG]-8;
    guest.proc->NZCV.bits->ccval = PACK_CC(0, 1, 0, 0);
    guest.proc->GPR.bits[30].xval = RET_FROM_MAIN_ADDR;

    pipe_reg_t **pipes[] = {&F_instr, &D_instr, &X_instr, &M_instr, &W_instr};

    uint64_t sizes[5] = {sizeof(f_instr_impl_t), sizeof(d_instr_impl_t), sizeof(x_instr_impl_t),
                         sizeof(m_instr_impl_t), sizeof(w_instr_impl_t)};
    for (int i = 0; i < 5; i++) {
        *pipes[i] = (pipe_reg_t *)calloc(1, sizeof(pipe_reg_t));
        (*pipes[i])->size = sizes[i];
        (*pipes[i])->in = (pipe_reg_implt_t) calloc(1, sizes[i]);
        (*pipes[i])->out = (pipe_reg_implt_t) calloc(1, sizes[i]);
        (*pipes[i])->ctl = P_BUBBLE;
    }

    /* Will be selected as the first PC */
    F_out->pred_PC = guest.proc->PC.bits->xval;
    F_out->status = STAT_AOK;
    dmem_status = READY;

#ifdef DEBUG
    printf("\n%s%s   Addr      Instr       Op  \tCond\tDest\tSrc1\tSrc2\tImmval   \t\tShift%s\n", 
           ANSI_BOLD, ANSI_COLOR_RED, ANSI_RESET);
#endif
    num_instr = 0;
    do {        
        /* Run each stage (in reverse order, to get the correct effect) */
        /* TODO: rewrite as independent threads */
        wback_instr(W_out);
        memory_instr(M_out, W_in);
        execute_instr(X_out, M_in);   
        decode_instr(D_out, X_in);   
        fetch_instr(F_out, D_in);

        F_in->pred_PC = F_PC;

        /* Set machine state to either continue executing or shutdown */
        guest.proc->status = W_out->status;

        /* Check for hazards and appropriately stall/bubble stages */
        uint8_t D_src1 = (D_out->op == OP_MOVZ) ? 0x1F : bitfield_u32(D_out->insnbits, 5, 5);
        uint8_t D_src2 = (D_out->op != OP_STUR) ? bitfield_u32(D_out->insnbits, 16, 5) : bitfield_u32(D_out->insnbits, 0, 5);
        uint8_t X_dst = X_out->W_sigs.dst_sel ? 30 : X_out->dst;

        /* Hazard handling and pipeline control */
        handle_hazards(D_out->op, D_src1, D_src2, X_out->op, X_dst, X_condval);

        /* Print debug output */
        if(debug_level > 0)
            printf("\nPipeline state at end of cycle %ld:\n", num_instr);
        show_instr(S_FETCH, debug_level);
        show_instr(S_DECODE, debug_level);
        show_instr(S_EXECUTE, debug_level);
        show_instr(S_MEMORY, debug_level);
        show_instr(S_WBACK, debug_level);
        if(debug_level > 0)
            printf("\n\n");

        guest.proc->PC.bits->xval = F_PC;

        for (int i = 0; i < 5; i++) {
            pipe_reg_t *pipe = *pipes[i];
            switch(pipe->ctl) {
                case P_LOAD: // Normal, cycle stage
                    memcpy(pipe->out.generic, pipe->in.generic, pipe->size);
                    break;
                case P_ERROR:  // Error, bubble this stage
                    guest.proc->status = STAT_HLT;
                case P_BUBBLE: // Hazard, needs to bubble
                    memset(pipe->out.generic, 0, pipe->size);
                    break;
                case P_STALL: // Hazard, needs to stall
                    break;
            }
        }

        num_instr++;
    } while ((guest.proc->status == STAT_AOK || guest.proc->status == STAT_BUB)
             && num_instr < cycle_max);
    return EXIT_SUCCESS;
}