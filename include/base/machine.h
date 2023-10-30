/**************************************************************************
 * C S 429 system emulator
 * 
 * machine.h - Headers for machine parameters used by the emulator. 
 * 
 * Copyright (c) 2022, 2023.
 * Authors: S. Chatterjee, Z. Leeper.
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _MACHINE_H_
#define _MACHINE_H_

#include "proc.h"
#include "mem.h"
#include "cache/cache.h"

// User/supervisor mode.
// TODO: Change to Arm ELs.
typedef enum {
    MODE_USER,
    MODE_KER,
    MODE_ERR = -1
} machine_mode_t;

// Machine state.
typedef struct machine {
    char *name;                 // Descriptive name of machine
    unsigned word_size;         // Word size of machine
    byte_order_t code_order;    // Endianness for instructions
    byte_order_t data_order;    // Endianness for data
    machine_mode_t mode;        // Exception level of machine
    proc_t *proc;               // Pointer to machine's processor
    mem_t *mem;                 // Pointer to machine's memory
    cache_t *cache;             // Pointer to machine's cache
} machine_t;

extern uint64_t seg_starts[];   // Starting locations of memory segments (e.g., code, data, stack, etc.).

extern void init_machine(char *, unsigned, byte_order_t, byte_order_t);
extern void log_machine_state(void);
#endif