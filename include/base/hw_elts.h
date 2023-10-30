/**************************************************************************
 * C S 429 system emulator
 * 
 * hw_elts.h - A header file defining the prototypes of the hardware elements
 * used in the pipeline simulator.
 * 
 * All of the hardware elements are of return type void, so they write return
 * values in their out parameters (which must therefore be passed by reference).
 * The in and out parameters are indicated in the prototypes below.
 * 
 * Copyright (c) 2022, 2023. 
 * Author: S. Chatterjee. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _HW_ELTS_H_
#define _HW_ELTS_H_
#include <stdint.h>
#include <stdbool.h>
#include "instr.h"
#include "instr_pipeline.h"

/* Instruction memory. */
extern comb_logic_t imem(uint64_t imem_addr,                                                    // in, data
                         uint32_t *imem_rval, bool *imem_err);                                  // out

/* 
 * Register file.
 * 
 * Remember that the inputs for writing are fed by the instruction in Writeback at this cycle,
 * while the inputs for reading are fed by the instruction in Decode at this cycle.
 * THESE ARE TWO DIFFERENT INSTRUCTIONS.
 * 
 * Register number 31 is always interpreted as SP.
 * Register number 32 and higher is interpreted as ZR.
 */
extern comb_logic_t regfile(uint8_t src1, uint8_t src2, uint8_t dst, uint64_t val_w,            // in, data
                            // bool src1_31isSP, bool src2_31isSP, bool dst_31isSP, 
                            bool w_enable, // in, control
                            uint64_t *val_a, uint64_t *val_b);                                  // out

/* Arithmetic and logic unit. Includes the NZCV register. */
extern comb_logic_t alu(uint64_t alu_vala, uint64_t alu_valb, uint8_t alu_valhw,               // in, data
                        alu_op_t ALUop, bool set_CC, cond_t cond,                               // in, control
                        uint64_t *val_e, bool *cond_val);                                       // out

/* Data memory. */
extern comb_logic_t dmem(uint64_t dmem_addr, uint64_t dmem_wval,                                // in, data
                         bool dmem_read, bool dmem_write,                                       // in, control
                         uint64_t *dmem_rval, bool *dmem_err);                                   // out
#endif