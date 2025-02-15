/**************************************************************************
 * C S 429 system emulator
 * 
 * instr_Fetch.c - Fetch stage of instruction processing pipeline.
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

extern uint64_t F_PC;

extern uint32_t bitfield_u32(int32_t src, unsigned frompos, unsigned width);

//TODO: fetch instr, select pc, predict pc, fix instr aliases
// bin/test-se -v 1 do
// ^ runs all tests cases 

// exceptions/simple/ldur
// exceptions/simple/stur

/*
 * Select PC logic.
 * STUDENT TO-DO:
 * Write the next PC to *current_PC.
 */

static comb_logic_t 
select_PC(uint64_t pred_PC,                                     // The predicted PC
          opcode_t D_opcode, uint64_t val_a,                    // Possible correction from RET
          opcode_t M_opcode, bool M_cond_val, uint64_t seq_succ,// Possible correction from B.cond
          uint64_t *current_PC) {
    /* 
     * Students: Please leave this code
     * at the top of this function. 
     * You may modify below it. 
     */
    if (D_opcode == OP_RET && val_a == RET_FROM_MAIN_ADDR) {
        *current_PC = 0; // PC can't be 0 normally.
        return;
    }
    // Modify starting here.
    if(!seq_succ){
        *current_PC = pred_PC;
    } else if(!M_cond_val && M_opcode == OP_B_COND) { // correction from b.cond, not sure if this is supposed to be ! or not
        *current_PC = seq_succ;
    } else if (D_opcode == OP_RET) { // ret
        *current_PC = val_a;
    } else { // base case
        *current_PC = pred_PC;
    }
    return;
}

/*
 * Predict PC logic. Conditional branches are predicted taken.
 * STUDENT TO-DO:
 * Write the predicted next PC to *predicted_PC
 * and the next sequential pc to *seq_succ.
 */

static comb_logic_t 
predict_PC(uint64_t current_PC, uint32_t insnbits, opcode_t op, 
           uint64_t *predicted_PC, uint64_t *seq_succ) {
    /* 
     * Students: Please leave this code
     * at the top of this function. 
     * You may modify below it. 
     */
    if (!current_PC) {
        return; // We use this to generate a halt instruction.
    }
    // Modify starting here.
    if(op == OP_ADRP) {
        *predicted_PC = current_PC + 4;
        // *seq_succ = current_PC & ~0xFFF;
        // int32_t upper12 = insnbits & 0xFFFFE0;
        // upper12 >>= 8;
        // upper12 &= ~0xFFF;
        // *predicted_PC = upper12;
        *seq_succ = current_PC & ~0xFFF;
    } else {
        *seq_succ = current_PC + 4; //changed from +32 to +4
        if (op == OP_B || op == OP_BL ) {
            int imm26 = bitfield_s64(insnbits, 0, 26);
            int64_t offset = imm26 << 2;
            *predicted_PC = current_PC + offset;
        } else if(op == OP_B_COND) {
            uint64_t imm19 = bitfield_s64(insnbits, 5, 19);
            uint64_t offset = imm19 << 2;
            *predicted_PC = current_PC + offset;
        } else {
            *predicted_PC = current_PC + 4; //same as above
        }
    }

    return;
}

/*
 * Helper function to recognize the aliased instructions:
 * LSL, LSR, CMP, and TST. We do this only to simplify the 
 * implementations of the shift operations (rather than having
 * to implement UBFM in full).
 */

static
void fix_instr_aliases(uint32_t insnbits, opcode_t *op) {
    uint32_t b0to4 = bitfield_u32(insnbits, 0, 5);
    if(*op == OP_SUBS_RR){
        if(b0to4 == 0x1f){
            *op = OP_CMP_RR;
        }
    } else if (*op == OP_ANDS_RR){
        if(b0to4 == 0x1f){
            *op = OP_TST_RR;
        }
    } else if (*op == OP_UBFM){
        uint32_t b10to15 = bitfield_u32(insnbits, 10, 6);
        if(b10to15 == 0x3f){
            *op = OP_LSR;
        } else {
            *op = OP_LSL;
        }
    }
    // uint32_t b22to31 = bitfield_u32(insnbits, 22, 10);
    // uint32_t b21to31 = bitfield_u32(insnbits, 21, 11);
    // uint32_t b10to15 = bitfield_u32(insnbits, 10, 6);
    // uint32_t b16to21 = bitfield_u32(insnbits, 16, 6);
    // uint32_t b0to4 = bitfield_u32(insnbits, 0, 5);
    // if(b22to31 == 0x34d) { // UBFM
    //     if(b10to15 >= b16to21) {
    //         *op = OP_LSR;
    //     } else {
    //         *op = OP_LSL;
    //     }
    // } else if (b21to31 == 0x758) { // CMP
    //     if(b0to4 != 0x1f) {
    //         *op = OP_SUBS_RR;
    //     } 
    // } else if (b21to31 == 0x758) { // TST
    //     *op = OP_ANDS_RR;
    // }
    // return;
}

/*
 * Fetch stage logic.
 * STUDENT TO-DO:
 * Implement the fetch stage.
 * 
 * Use in as the input pipeline register,
 * and update the out pipeline register as output.
 * Additionally, update F_PC for the next
 * cycle's predicted PC.
 * 
 * You will also need the following helper functions:
 * select_pc, predict_pc, and imem.
 */

comb_logic_t fetch_instr(f_instr_impl_t *in, d_instr_impl_t *out) {
    bool imem_err = 0;
    uint64_t current_PC;
    select_PC(in->pred_PC, X_out->op, X_out->val_a, M_out->op, M_out->cond_holds, M_out->seq_succ_PC, &current_PC);
    /* 
     * Students: This case is for generating HLT instructions
     * to stop the pipeline. Only write your code in the **else** case. 
     */
    if (!current_PC || F_in->status == STAT_HLT) {
        out->insnbits = 0xD4400000U;
        out->op = OP_HLT;
        out->print_op = OP_HLT;
        imem_err = false;
    }
    else {
        // WRITE HERE!   
        //get isnbits
        //get opcode from those bits
        //helper functions 
        uint32_t *imm_errPtr;
        imem(current_PC, &(out->insnbits), &imm_errPtr); // out->insnbits
        // getting opcode from insnbits
        uint32_t bits = bitfield_u32(out->insnbits, 21, 11);
        uint32_t opCode = itable[bits]; //out->op = itable[11bits]
        out->op = opCode;
        fix_instr_aliases(out->insnbits, &(out->op));
        predict_PC(current_PC, out->insnbits, out->op, &F_PC, &(out->seq_succ_PC));
        out->print_op = out->op;
        out->this_PC = current_PC;
    }
    
    // We do not recommend modifying the below code.
    if (imem_err || out->op == OP_ERROR) {
        in->status = STAT_INS;
        F_in->status = in->status;
    } else if (out->op == OP_HLT) {
        in->status = STAT_HLT;
        F_in->status = in->status;
    } else {
        in->status = STAT_AOK;
    }
    out->status = in->status;
    return;
}                                                                                                                       
