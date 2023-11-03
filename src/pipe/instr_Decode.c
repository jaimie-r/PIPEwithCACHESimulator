/**************************************************************************
 * C S 429 system emulator
 * 
 * instr_Decode.c - Decode stage of instruction processing pipeline.
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
#include "forward.h"
#include "machine.h"
#include "hw_elts.h"

#define SP_NUM 31
#define XZR_NUM 32

extern machine_t guest;
extern mem_status_t dmem_status;

extern int64_t W_wval;

//TODO: decode instr

/*
 * Control signals for D, X, M, and W stages.
 * Generated by D stage logic.
 * D control signals are consumed locally. 
 * Others must be buffered in pipeline registers.
 * STUDENT TO-DO:
 * Generate the correct control signals for this instruction's
 * future stages and write them to the corresponding struct.
 */

static comb_logic_t 
generate_DXMW_control(opcode_t op,
                      d_ctl_sigs_t *D_sigs, x_ctl_sigs_t *X_sigs, m_ctl_sigs_t *M_sigs, w_ctl_sigs_t *W_sigs) {

    D_sigs->src2_sel = (op == OP_STUR);
    X_sigs->set_CC = (op == OP_ADDS_RR || op == OP_SUBS_RR || op == OP_CMP_RR || op == OP_ANDS_RR || op == OP_TST_RR);
    X_sigs->valb_sel = (op == OP_ADDS_RR || op == OP_SUBS_RR || op == OP_CMP_RR || op == OP_ANDS_RR || op == OP_TST_RR || op == OP_ORR_RR || op == OP_EOR_RR || op == OP_MVN);
    M_sigs->dmem_read = op == OP_LDUR;
    M_sigs->dmem_write = op == OP_STUR;
    W_sigs->dst_sel = op == OP_BL;
    W_sigs->wval_sel = op == OP_LDUR;
    W_sigs->w_enable = (op == OP_LDUR || op == OP_MOVK || op == OP_MOVZ || op == OP_ADRP || op == OP_ADD_RI || 
        op == OP_ADDS_RR || op == OP_SUB_RI || op == OP_SUBS_RR || op == OP_MVN || op == OP_ORR_RR || op == OP_EOR_RR ||
        op == OP_ANDS_RR || op == OP_LSL || op == OP_LSR || op == OP_ASR || op == OP_BL);
    return;
}

/*
 * Logic for extracting the immediate value for M-, I-, and RI-format instructions.
 * STUDENT TO-DO:
 * Extract the immediate value and write it to *imm.
 */

static comb_logic_t 
extract_immval(uint32_t insnbits, opcode_t op, int64_t *imm) {
    switch(op) {
        case OP_LDUR:
        case OP_STUR:
            *imm = bitfield_u32(insnbits, 12, 9);
            break;
        case OP_MOVK:
        case OP_MOVZ:
            *imm = bitfield_u32(insnbits, 5, 16);
            break;
        case OP_ADRP:
            *imm = *imm;
            int64_t immlow = bitfield_s64(insnbits, 29, 2);
            immlow <<= 12;
            int64_t immhigh = bitfield_s64(insnbits, 5, 19);
            immhigh <<= 14;
            *imm = immlow + immhigh;
            break;
        case OP_ADD_RI:
        case OP_SUB_RI:
        case OP_LSL:
        case OP_LSR:
        case OP_UBFM:
        case OP_ASR:
            *imm = bitfield_u32(insnbits, 10, 12);
            break;
        case OP_B:
        case OP_BL:
            *imm = bitfield_u32(insnbits, 0, 26);
            break;
        case OP_B_COND:
            *imm = bitfield_u32(insnbits, 5, 19);
            break;
        default:
            *imm = 0;
            break;
    }
    return;
}

/*
 * Logic for determining the ALU operation needed for this opcode.
 * STUDENT TO-DO:
 * Determine the ALU operation based on the given opcode
 * and write it to *ALU_op.
 */
static comb_logic_t
decide_alu_op(opcode_t op, alu_op_t *ALU_op) {
    switch(op) {
        case OP_LDUR:
        case OP_STUR:
        case OP_ADRP:
        case OP_ADD_RI:
        case OP_ADDS_RR:
            *ALU_op = PLUS_OP;
            break;
        case OP_SUB_RI:
        case OP_SUBS_RR:
        case OP_CMP_RR:
            *ALU_op = MINUS_OP;
            break;
        case OP_MVN:
            *ALU_op = NEG_OP;
            break;
        case OP_ORR_RR:
            *ALU_op = OR_OP;
            break;
        case OP_EOR_RR:
            *ALU_op = EOR_OP;
            break;
        case OP_ANDS_RR:
        case OP_TST_RR:
            *ALU_op = AND_OP;
            break;
        case OP_MOVK:
        case OP_MOVZ:
            *ALU_op = MOV_OP;
            break;
        case OP_LSL: 
            *ALU_op = LSL_OP;
            break;
        case OP_LSR:
            *ALU_op = LSR_OP;
            break;
        case OP_ASR:
            *ALU_op = ASR_OP;
            break;
        default:
            *ALU_op = PASS_A_OP;
            break;
    }
    return;
}

/*
 * Utility functions for copying over control signals across a stage.
 * STUDENT TO-DO:
 * Copy the input signals from the input side of the pipeline
 * register to the output side of the register.
 */

comb_logic_t 
copy_m_ctl_sigs(m_ctl_sigs_t *dest, m_ctl_sigs_t *src) {
    dest->dmem_read = src->dmem_read ? 1 : 0;
    dest->dmem_write = src->dmem_write ? 1 : 0;
    return;
}

comb_logic_t 
copy_w_ctl_sigs(w_ctl_sigs_t *dest, w_ctl_sigs_t *src) {
    dest->dst_sel = src->dst_sel ? 1 : 0;
    dest->wval_sel = src->wval_sel ? 1 : 0;
    dest->w_enable = src->w_enable ? 1 : 0;
    return;
}

comb_logic_t
extract_regs(uint32_t insnbits, opcode_t op, 
             uint8_t *src1, uint8_t *src2, uint8_t *dst) {
    
    //src1
    if(op!=OP_NOP){
        if(op==OP_B || op==OP_B_COND || op==OP_HLT || op==OP_BL){ //op==OP_NOP?
            //*src1 = 18;
        } else if (op==OP_MOVZ){
            *src1 = XZR_NUM;
        } else if (op==OP_MOVK){
            *src1 = bitfield_u32(insnbits, 0, 5);
        } else {
            *src1 = bitfield_u32(insnbits, 5, 5);
        }
        
        //src2
        if(op != OP_RET){
            // if (op==OP_ADDS_RR || op==OP_SUBS_RR || op==OP_CMP_RR || 
            //         op==OP_ORR_RR || op==OP_EOR_RR ||op==OP_ANDS_RR || op==OP_TST_RR){
            //     *src2 = bitfield_u32(insnbits, 16, 5);
            // } else {
            //     *src2 = bitfield_u32(insnbits, 0, 5);
            // }
            if(op==OP_STUR){
                *src2 = bitfield_u32(insnbits, 0, 5);
            } else if (op==OP_ADDS_RR || op==OP_SUBS_RR || op==OP_CMP_RR || op==OP_MVN || 
             op==OP_ORR_RR || op==OP_EOR_RR ||op==OP_ANDS_RR || op==OP_TST_RR){
                *src2 = bitfield_u32(insnbits, 16, 5);
            } else if (op==OP_SUB_RI ){
                *src2 = *src1;
            }
            if(op==OP_MVN && *src2==SP_NUM){
                *src2 = XZR_NUM;
            }
        }

        //dst
        if(op==OP_TST_RR || op==OP_CMP_RR){
            *dst = XZR_NUM;
        } else if (op!= OP_RET){
            *dst = X_in->W_sigs.dst_sel ? 30 : bitfield_u32(insnbits, 0, 5);
        }
    }

    // if(op==OP_LDUR){
    //     //M
    //     *src1 = bitfield_u32(insnbits, 5, 5);
    //     *dst = bitfield_u32(insnbits, 0, 5);
    // } else if (op==OP_STUR){
    //     //M
    //     *src1 = bitfield_u32(insnbits, 5, 5);
    //     *src2 = bitfield_u32(insnbits, 0, 5);
    //     *dst = bitfield_u32(insnbits, 0, 5);
    // } else if (op==OP_MOVK || op==OP_MOVZ || op==OP_ADRP){
    //     //I1 + I2
    //     *dst = bitfield_u32(insnbits, 0, 5);
    // } else if (op==OP_ADD_RI || op==OP_SUB_RI || op==OP_LSL || 
    //             op==OP_LSR || op==OP_UBFM || op==OP_ASR){
    //     //RI
    //     *src1 = bitfield_u32(insnbits, 5, 5);
    //     *dst = bitfield_u32(insnbits, 0, 5);
    // } else if (op==OP_ADDS_RR || op==OP_SUBS_RR || op==OP_CMP_RR || op==OP_MVN || 
    //         op==OP_ORR_RR || op==OP_EOR_RR ||op==OP_ANDS_RR || op==OP_TST_RR){
    //     //RR
    //     *src1 = bitfield_u32(insnbits, 5, 5);
    //     *src2 = bitfield_u32(insnbits, 16, 5);
    //     *dst = bitfield_u32(insnbits, 0, 5);   
    // } else if (op==OP_RET){
    //     //B3
    //     *src1 = bitfield_u32(insnbits, 5, 5);
    // }
    
    //check if src1, src2, or dst are the sp
    //if they are, and the opperation isn't allowed to access the sp, change it to xzr
    // if(*src1 == SP_NUM && !(op==OP_ADD_RI || op==OP_SUB_RI)) {
    //     *src1 = XZR_NUM;
    // }
    // if(*src2 == SP_NUM) {
    //     *src2 = XZR_NUM;
    // }
    // if(*dst == SP_NUM) {
    //     *dst = XZR_NUM;
    // }

    return;
}

/*
 * Decode stage logic.
 * STUDENT TO-DO:
 * Implement the decode stage.
 * 
 * Use `in` as the input pipeline register,
 * and update the `out` pipeline register as output.
 * Additionally, make sure the register file is updated
 * with W_out's output when you call it in this stage.
 * 
 * You will also need the following helper functions:
 * generate_DXMW_control, regfile, extract_immval,
 * and decide_alu_op.
 */

comb_logic_t decode_instr(d_instr_impl_t *in, x_instr_impl_t *out) {

    //connect wires
    out->seq_succ_PC = in->seq_succ_PC;
    out->op = in->op;
    out->print_op = in->print_op;
    out->status = in->status;

    //local vars 
    d_ctl_sigs_t *D_sigs;
    uint8_t *src1 = XZR_NUM;
    uint8_t *src2 = XZR_NUM;

    //helpers
    generate_DXMW_control(in->op, &D_sigs, &(out->X_sigs), &(out->M_sigs), &(out->W_sigs));
    //below helps pass basic test
    if(in->insnbits == 0 && in->op!=OP_NOP){
        out->W_sigs.w_enable = true;
    }
    extract_regs(in->insnbits, in->op, &src1, &src2, &(out->dst));
    regfile(src1, src2, W_out->dst, W_wval, W_out->W_sigs.w_enable, &(out->val_a), &(out->val_b));
    decide_alu_op(in->op, &(out->ALU_op));
    extract_immval(in->insnbits, in->op, &(out->val_imm));

    if(out->X_sigs.valb_sel){
        out->val_b = out->val_imm;
    } 

    //adrp fix 
    if(in->op == OP_ADRP){
        out->val_a = in->seq_succ_PC;
    }

    //hw
    if(in->op == OP_MOVK || in->op == OP_MOVZ){
        out->val_hw = bitfield_u32(in->insnbits, 21, 2) << 4;
        out->val_b = 0;
    } else {
        out->val_hw = 0;
    }

    if(in->op == OP_BL){
        out->val_a = out->seq_succ_PC;
    }

    // cond
    if(out->op == OP_B_COND) {
        uint32_t insCond = in->insnbits &0xf;
        switch(insCond) {
            case 0x0:
                out->cond = C_EQ;
                break;
            case 0x1:
                out->cond = C_NE;
                break;
            case 0x2:
                out->cond = C_CS;
                break;
            case 0x3:
                out->cond = C_CC;
                break;
            case 0x4:
                out->cond = C_MI;
                break;
            case 0x5:
                out->cond = C_PL;
                break;
            case 0x6:
                out->cond = C_VS;
                break;
            case 0x7:
                out->cond = C_VC;
                break;
            case 0x8:
                out->cond = C_HI;
                break;
            case 0x9:
                out->cond = C_LS;
                break;
            case 0xa:
                out->cond = C_GE;
                break;
            case 0xb:
                out->cond = C_LT;
                break;
            case 0xc:
                out->cond = C_GT;
                break;
            case 0xd:
                out->cond = C_LE;
                break;
            case 0xe:
                out->cond = C_AL;
                break;
            case 0xf:
                out->cond = C_NV;
                break; 
            default:
                break;
        }
    }


    return;
}
