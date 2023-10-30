/**************************************************************************
 * STUDENTS: DO NOT MODIFY.
 * 
 * C S 429 system emulator
 * 
 * archsim.c - The entry point for the emulator.
 * 
 * Copyright (c) 2022, 2023. 
 * Authors: S. Chatterjee, Z. Leeper.
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include "archsim.h"

machine_t       guest;
opcode_t        itable[2<<11];
FILE            *infile, *outfile, *errfile, *checkpoint;
char            *infile_name;
char            *ae_prompt;
uint64_t        num_instr;
uint64_t        cycle_max;
int             debug_level;
int             A, B, C, d;
uint64_t        inflight_cycles;
uint64_t        inflight_addr;
bool            inflight;
mem_status_t    dmem_status;

int main(int argc, char* argv[]) {
    debug_level = 0;
    cycle_max = MAX_NUM_INSTR;
    handle_args(argc, argv);
    init();
    
    uint64_t entry = loadElf(infile_name);
    int ret = runElf(entry);
    
    finalize();
    
    return ret;
}
