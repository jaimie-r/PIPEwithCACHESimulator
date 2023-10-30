/**************************************************************************
 * C S 429 system emulator
 * 
 * proc.h - Headers for processor parameters used by the emulator. 
 * 
 * Copyright (c) 2022. 
 * Authors: S. Chatterjee. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _PROC_H_
#define _PROC_H_
#include <stdint.h>
#include "reg.h"
#include "instr.h"
#include "instr_pipeline.h"

// Processor state.
typedef struct proc {
    // Architectural state
    reg_file_t GPR;     // General-purpose registers
    // reg_file_t FPR;  // TODO
    reg_t PC;           // Program counter
    reg_t SP;           // Stack pointer
    reg_t NZCV;         // Condition flags

    // Micro-architectural state
    pipe_reg_t *f_insn; // Fetch stage pipeline register
    pipe_reg_t *d_insn; // Decode stage pipeline register 
    pipe_reg_t *x_insn; // Execute stage pipeline register
    pipe_reg_t *m_insn; // Memory stage pipeline register
    pipe_reg_t *w_insn; // Writeback stage pipeline register
    stat_t status;      // Pipeline status
} proc_t;

// Run the loaded ELF executable for no more than a specified number of cycles.
extern int runElf(const uint64_t);
#endif