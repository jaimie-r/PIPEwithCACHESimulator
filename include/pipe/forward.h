/**************************************************************************
 * C S 429 system emulator
 * 
 * forward.h - Headers for value forwarding logic used by the emulator. 
 * 
 * Copyright (c) 2022. 
 * Authors: S. Chatterjee. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _FORWARD_H_
#define _FORWARD_H_
#include <stdint.h>
#include "instr.h"

// Interface for the forwarding unit. Note that it handles forwarding for two outputs.
extern comb_logic_t
forward_reg(uint8_t D_src1, uint8_t D_src2, uint8_t X_dst, uint8_t M_dst, uint8_t W_dst,           // in, data
            uint64_t X_val_ex, uint64_t M_val_ex, uint64_t M_val_mem, uint64_t W_val_ex,           // in, data
            uint64_t W_val_mem,                                                                    // in, data
            bool M_wval_sel, bool W_wval_sel, bool X_w_enable, bool M_w_enable, bool W_w_enable,   // in, control
            // bool D_src1_31isSP, bool D_src2_31isSP,                                                // in, control
            uint64_t *val_a, uint64_t *val_b);                                                     // out
#endif