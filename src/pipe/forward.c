/**************************************************************************
 * C S 429 system emulator
 * 
 * forward.c - The value forwarding logic in the pipelined processor.
 **************************************************************************/ 

#include <stdbool.h>
#include "forward.h"

/* STUDENT TO-DO:
 * Implement forwarding register values from
 * execute, memory, and writeback back to decode.
 */
// forward_reg(src1, src2, out->dst, M_in->dst, W_in->dst,
                //  M_in->val_ex, M_out->val_ex, M_in->val_b, W_in->val_ex,
                //  W_in->val_mem, M_in->W_sigs.wval_sel, W_in->W_sigs.wval_sel, X_in->W_sigs.w_enable,
                //  M_in->W_sigs.w_enable, W_in->W_sigs.w_enable,
                // &(out->val_a), &(out->val_b));
comb_logic_t forward_reg(uint8_t D_src1, uint8_t D_src2, uint8_t X_dst, uint8_t M_dst, uint8_t W_dst,
                 uint64_t X_val_ex, uint64_t M_val_ex, uint64_t M_val_mem, uint64_t W_val_ex,
                 uint64_t W_val_mem, bool M_wval_sel, bool W_wval_sel, bool X_w_enable,
                 bool M_w_enable, bool W_w_enable,
                 uint64_t *val_a, uint64_t *val_b) {
    /* Students: Implement Below */
    

    //wtf are M_val_mem / W_val_mem supposed to be used for // M_wval_sel / W_wval_sel
    
    //pointer syntax?,,,,
    if (W_w_enable){
        if(W_wval_sel){
            *val_a = W_dst == D_src1 ? W_val_mem : *val_a;
            *val_b = W_dst == D_src2 ? W_val_mem : *val_b;
        } else {
            *val_a = W_dst == D_src1 ? W_val_ex : *val_a;
            *val_b = W_dst == D_src2 ? W_val_ex : *val_b;
        }
    } 
    if (M_w_enable){
        if(M_wval_sel){
            *val_a = M_dst == D_src1 ? M_val_mem : *val_a;
            *val_b = M_dst == D_src2 ? M_val_mem : *val_b;
        } else {
            *val_a = M_dst == D_src1 ? M_val_ex : *val_a;
            *val_b = M_dst == D_src2 ? M_val_ex : *val_b;
        }
    }
    if(X_w_enable){
        *val_a = X_dst == D_src1 ? X_val_ex : *val_a;
        *val_b = X_dst == D_src2 ? X_val_ex : *val_b;
    }

    return;
}
