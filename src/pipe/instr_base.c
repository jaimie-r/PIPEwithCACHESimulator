/**************************************************************************
 * STUDENTS: DO NOT MODIFY.
 * 
 * C S 429 system emulator
 * 
 * instr_base.c - Common helper routines for instruction processing.
 * 
 * Stage details are written in the instr_*.c files in this directory.
 * 
 * Copyright (c) 2022, 2023. 
 * Authors: S. Chatterjee, P. Jamadagni, Z. Leeper. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "err_handler.h"
#include "instr.h"
#include "instr_pipeline.h"
#include "machine.h"
#include "hw_elts.h"

extern machine_t guest;
extern mem_status_t dmem_status;

uint64_t F_PC;
bool X_condval;
int64_t W_wval;

/*
 * Extracts the bitfield src[frompos+width-1:frompos] and returns it
 * as an unsigned 32-bit integer.
 * 
 * Use this function for register specifiers, condition specifiers, 
 * and immediates in I1- and RI-format instructions. Also use it to
 * extract the 11 bits used to determine the opcode.
 */
uint32_t
bitfield_u32(int32_t src, unsigned frompos, unsigned width) {
    return (uint32_t) ((src >> frompos) & ((1 << width) - 1));
}

/*
 * Extracts the bitfield src[frompos+width-1:frompos] and returns it
 * as an signed 64-bit integer.
 * 
 * Use this function for 64-bit offsets needed in M-, I2-, B1-, and 
 * B2-format instructions.
 */
int64_t
bitfield_s64(int32_t src, unsigned frompos, unsigned width) {
    return ((int64_t) ((((int64_t)src >> frompos) & ((1 << width) - 1)) << (64 - width))) >> (64 - width);
}

static inline void 
init_itable_entry(opcode_t op, unsigned idx) {
    assert(OP_ERROR == itable[idx]);
    itable[idx] = op;
}

static inline void 
init_itable_range(opcode_t op, unsigned idx1, unsigned idx2) {
    for (unsigned i = idx1; i <= idx2; i++) {
        assert(OP_ERROR == itable[i]);
        itable[i] = op;
    }
}

/*
 * Initialize the itable. Called from interface.c.
 *
 * There are no entries for CMP, TST, LSL, and LSR because they are aliases of other instructions.
 * Do not re-write.
 */

void 
init_itable(void) {
    for (int i = 0; i < 2<<11; i++) itable[i] = OP_ERROR;
    init_itable_entry(OP_LDUR, 0x7c2U);
    init_itable_entry(OP_STUR, 0x7c0U);
    init_itable_range(OP_MOVK, 0x794U, 0x797U);
    init_itable_range(OP_MOVZ, 0x694U, 0x697U);
    init_itable_range(OP_ADRP, 0x480U, 0x487U);
    init_itable_range(OP_ADRP, 0x580U, 0x587U);
    init_itable_range(OP_ADRP, 0x680U, 0x687U);
    init_itable_range(OP_ADRP, 0x780U, 0x787U);
    init_itable_range(OP_ADD_RI, 0x488U, 0x489U);
    init_itable_entry(OP_ADDS_RR, 0x558U);
    init_itable_range(OP_SUB_RI, 0x688U, 0x689U);
    init_itable_entry(OP_SUBS_RR, 0x758U);
    init_itable_entry(OP_MVN, 0x551U);
    init_itable_entry(OP_ORR_RR, 0x550U);
    init_itable_entry(OP_EOR_RR, 0x650U);
    init_itable_entry(OP_ANDS_RR, 0x750U);
    init_itable_range(OP_UBFM, 0x69aU, 0x69bU); // LSL, LSR share the same opcode
    init_itable_range(OP_ASR, 0x49aU, 0x49bU);
    init_itable_range(OP_B, 0x0a0U, 0x0bfU);
    init_itable_range(OP_B_COND, 0x2a0U, 0x2a7U);
    init_itable_range(OP_BL, 0x4a0U, 0x4bfU);
    init_itable_entry(OP_RET, 0x6b2U);
    init_itable_entry(OP_NOP, 0x6a8U);
    init_itable_entry(OP_HLT, 0x6a2U);
}

static char *opcode_names[] = {
    "NOP ",
    "LDUR ",
    "STUR ",
    "MOVK ",
    "MOVZ ",
    "ADRP ",
    "ADD ",
    "ADDS ",
    "SUB",
    "SUBS ",
    "CMP ",
    "MVN ",
    "ORR ",
    "EOR ",
    "ANDS ",
    "TST ",
    "LSL ",
    "LSR ",
    "UBFM ",
    "ASR ",
    "B ",
    "B.cond ",
    "BL ",
    "RET ",
    "HLT ",
    "ERR ", 
};

static char *cond_names[] = {
    "EQ", "NE", "CS", "CC", "MI", "PL", "VS", "VC", 
    "HI", "LS", "GE", "LT", "GT", "LE", "AL", "NV"
};

static char *alu_op_names[] = {
    "PLUS_OP",
    "MINUS_OP",
    "NEG_OP",
    "OR_OP",
    "EOR_OP",
    "AND_OP",
    "MOV_OP",
    "LSL_OP",
    "LSR_OP",
    "ASR_OP",
    "PASS_A_OP",
    "PASS_B_OP"
};


/*
 * A debugging aid to print out the fields of an instruction.
 * This routine is called from runElf().
 * Do not re-write.
 * 
 * The amount of detail printed is controlled by debug_level.
 *  0: No output.
 *  1: Medium output. Data signals only in the pipeline register.
 *  2: Full output. Data and control signals in the pipeline register.
 */

static void 
get_stat_str(char *str, stat_t status) {
    switch (status) {
        case STAT_AOK:
            strcpy(str, "AOK");
            break;
        case STAT_BUB:
            strcpy(str, "BUB");
            break;
        case STAT_HLT:
            strcpy(str, "HLT");
            break;
        case STAT_ADR:
            strcpy(str, "ADR");
            break;
        case STAT_INS:
            strcpy(str, "INS");
            break;
    }
}

void 
show_instr(const proc_stage_t stage, int debug_level) {
    
    if(debug_level < 1) {
        return;
    }
    char status[4];

    switch (stage) {
    case S_FETCH:
        get_stat_str(status, F_out->status);
        printf("F: %-6s[PC, insn_bits] = [%08lX,  %08X], seq_succ_PC: 0x%lX, pred_PC: 0x%lX, status: %s\n", 
            opcode_names[D_in->print_op],
            guest.proc->PC.bits->xval, 
            D_in->insnbits,
            D_in->seq_succ_PC,
            F_in->pred_PC,
            status);

        break;
    case S_DECODE:
        get_stat_str(status, D_out->status);
        printf("D: %-6s[val_a, val_b, imm] = [0x%lX, 0x%lX, 0x%lX], alu_op: %s, cond: %s, dst: X%d, status: %s\n",
            opcode_names[D_out->print_op],
            X_in->val_a,
            X_in->val_b,
            X_in->val_imm,
            alu_op_names[X_in->ALU_op],
            cond_names[X_in->cond],
            X_in->dst,
            status);

        if (debug_level == 1)
            break;
        
        printf("\t X_sigs: [valb_sel, set_CC] = [%s, %s]\n",
            X_in->X_sigs.valb_sel ? "true " : "false",
            X_in->X_sigs.set_CC ? "true" : "false");

        printf("\t M_sigs: [dmem_read, dmem_write] = [%s, %s]\n",
            X_in->M_sigs.dmem_read ? "true " : "false",
            X_in->M_sigs.dmem_write ? "true" : "false");

        printf("\t W_sigs: [dst_sel, wval_sel, w_enable] = [%s, %s, %s]\n",
            X_in->W_sigs.dst_sel ? "true " : "false",
            X_in->W_sigs.wval_sel ? "true " : "false",
            X_in->W_sigs.w_enable ? "true" : "false");

        break;

        case S_EXECUTE: 
            get_stat_str(status, X_out->status);
            printf("X: %-6s[val_ex, a, b, imm, hw] = [0x%lX, 0x%lX, 0x%lX, 0x%lX, 0x%X], alu_op: %s, status: %s\n",
                opcode_names[X_out->print_op],
                M_in->val_ex,
                X_out->val_a,
                X_out->val_b,
                X_out->val_imm,
                X_out->val_hw,
                alu_op_names[X_out->ALU_op],
                status);
            printf("\t X_condval: %s\n", X_condval ? "true" : "false");
            if (debug_level == 1)
                break;
        
            // debug level 2: also print control signals

            printf("\t X_sigs: [valb_sel, set_CC] = [%s, %s]\n",
            X_out->X_sigs.valb_sel ? "true " : "false",
            X_out->X_sigs.set_CC ? "true" : "false");

            break;
        case S_MEMORY:
            get_stat_str(status, M_out->status); 
            printf("M: %-6s[val_ex, val_b, val_mem] = [0x%lX, 0x%lX, 0x%lX], status: %s\n",
                opcode_names[M_out->print_op],
                M_out->val_ex,
                M_out->val_b,
                W_in->val_mem,
                status);

            if (debug_level == 1)
                break;
        
            // debug level 2: also print control signals

            printf("\t M_sigs: [dmem_read, dmem_write] = [%s, %s]\n",
                M_out->M_sigs.dmem_read ? "true " : "false",
                M_out->M_sigs.dmem_write ? "true" : "false");

            break;
        case S_WBACK:
            get_stat_str(status, W_out->status);
            printf("W: %-6s[dst, val_ex, val_mem] = [X%d, 0x%lX, 0x%lX], status: %s\n",
                opcode_names[W_out->print_op],
                W_out->dst,
                W_out->val_ex,
                W_out->val_mem,
                status);

            if (debug_level == 1)
                break;
        
            // debug level 2: also print control signals

            printf("\t W_sigs: [dst_sel, wval_sel, w_enable] = [%s, %s, %s], W_wval: 0x%lx\n",
                W_out->W_sigs.dst_sel ? "true " : "false",
                W_out->W_sigs.wval_sel ? "true " : "false",
                W_out->W_sigs.w_enable ? "true" : "false",
                W_wval);

            break;
        default: IMPOSSIBLE(); break;
    }
    return;
}
