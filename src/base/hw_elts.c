/**************************************************************************
 * STUDENTS: DO NOT MODIFY.
 * 
 * C S 429 system emulator
 * 
 * hw_elts.c - Module for emulating hardware elements.
 * 
 * Copyright (c) 2022, 2023. 
 * Authors: S. Chatterjee, Z. Leeper. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include <assert.h>
#include "hw_elts.h"
#include "mem.h"
#include "reg.h"
#include "machine.h"
#include "forward.h"
#include "err_handler.h"

extern machine_t guest;

comb_logic_t 
imem(uint64_t imem_addr,
     uint32_t *imem_rval, bool *imem_err) {
    // imem_addr must be in "instruction memory" and a multiple of 4
    *imem_err = (!addr_in_imem(imem_addr) || (imem_addr & 0x3U));
    *imem_rval = (uint32_t) mem_read_I(imem_addr);
}

comb_logic_t
regfile(uint8_t src1, uint8_t src2, uint8_t dst, uint64_t val_w,
        // bool src1_31isSP, bool src2_31isSP, bool dst_31isSP, 
        bool w_enable,
        uint64_t *val_a, uint64_t *val_b) {
    // assert(src1 < 32);
    // assert(src2 < 32);
    // assert(dst < 32);
    if (src1 < 32) {
        *val_a = (src1 == 31) ? guest.proc->SP.bits->xval : guest.proc->GPR.names64[src1].bits->xval;
    }
    else {
        *val_a = 0x0UL;
    }
    if (src2 < 32) {
        *val_b = (src2 == 31) ? guest.proc->SP.bits->xval : guest.proc->GPR.names64[src2].bits->xval;
    }
    else {
        *val_b = 0x0UL;
    }
    if (w_enable && dst < 32) {
        if (dst == 31) {
            guest.proc->SP.bits->xval = val_w;
        }
        else guest.proc->GPR.names64[dst].bits->xval = val_w;
    }
}

static bool 
cond_holds(cond_t cond, uint8_t ccval) {
    switch (cond) {
        case C_EQ: return  (GET_ZF(ccval) == 1);
        case C_NE: return !(GET_ZF(ccval) == 1);
        case C_CS: return  (GET_CF(ccval) == 1);
        case C_CC: return !(GET_CF(ccval) == 1);
        case C_MI: return  (GET_NF(ccval) == 1);
        case C_PL: return !(GET_NF(ccval) == 1);
        case C_VS: return  (GET_VF(ccval) == 1);
        case C_VC: return !(GET_VF(ccval) == 1);
        case C_HI: return  (GET_CF(ccval) == 1 && GET_ZF(ccval) == 0);
        case C_LS: return !(GET_CF(ccval) == 1 && GET_ZF(ccval) == 0);
        case C_GE: return  (GET_NF(ccval) == GET_VF(ccval));
        case C_LT: return !(GET_NF(ccval) == GET_VF(ccval));
        case C_GT: return  (GET_NF(ccval) == GET_VF(ccval) && GET_ZF(ccval) == 0);
        case C_LE: return !(GET_NF(ccval) == GET_VF(ccval) && GET_ZF(ccval) == 0);
        case C_AL: case C_NV: return true;
        case C_ERROR: IMPOSSIBLE(); break;
    }
    return false;
}

comb_logic_t 
alu(uint64_t alu_vala, uint64_t alu_valb, uint8_t alu_valhw, alu_op_t ALUop, bool set_CC, cond_t cond, 
    uint64_t *val_e, bool *cond_val) {
    uint64_t res = 0xFEEDFACEDEADBEEF;  // To make it easier to detect errors.
    switch (ALUop) { // Perform the operation.
        case PLUS_OP: 
            res = alu_vala + (alu_valb << alu_valhw); break;
        case MINUS_OP: 
            res = alu_vala - (alu_valb << alu_valhw); break;
        case NEG_OP:
            res = alu_vala | (~alu_valb); break;
        case OR_OP:
            res = alu_vala | alu_valb; break;
        case EOR_OP:
            res = alu_vala ^ alu_valb; break;
        case AND_OP:
            res = alu_vala & alu_valb; break;
        case MOV_OP:
            res = alu_vala | (alu_valb << alu_valhw); break;
        case LSL_OP:
            res = alu_vala << (alu_valb & 0x3FUL); break;
        case LSR_OP:
            res = alu_vala >> (alu_valb & 0x3FUL); break;
        case ASR_OP:
            res = ((int64_t) alu_vala) >> (alu_valb & 0x3FUL); break;
        case PASS_A_OP:
            res = alu_vala; break;
        case PASS_B_OP:
            res = alu_valb; break;
        case ERROR_OP:
            break;
    }
    *val_e = res; // Store to output.
    if (set_CC) { // Update NZCV if needed.
        bool N = (res &0x8000000000000000) != 0;
        bool Z = res == 0;
        bool C = false;
        bool V = false;
        if (ALUop == PLUS_OP || ALUop == MINUS_OP) {
            // Just use the value-based determination strategies for C and V
            C = res < alu_vala;
            int64_t sa = (int64_t) alu_vala;
            int64_t sb = (int64_t) alu_valb;
            int64_t sres = sa + sb;
            if (sa >= 0 && sb >= 0 && sres < 0) V = true;
            if (sa < 0 && sb < 0 && sres >= 0) V = true;
        }
        guest.proc->NZCV.bits->ccval = PACK_CC(N, Z, C, V);
    }
    // Compute condition specified. No instruction both tests and sets CC, so ordering is irrelevant.
    *cond_val = cond_holds(cond, guest.proc->NZCV.bits->ccval);
}

comb_logic_t 
dmem(uint64_t dmem_addr, uint64_t dmem_wval, bool dmem_read, bool dmem_write, 
     uint64_t *dmem_rval, bool *dmem_err) {
    // dmem_addr must be in "data memory" and a multiple of 8
    *dmem_err = (!addr_in_dmem(dmem_addr) || (dmem_addr & 0x7U));
    if (is_special_addr(dmem_addr)) *dmem_err = false;
    if (dmem_read) *dmem_rval = (uint64_t) mem_read_L(dmem_addr);
    if (dmem_write) mem_write_L(dmem_addr, dmem_wval);
}