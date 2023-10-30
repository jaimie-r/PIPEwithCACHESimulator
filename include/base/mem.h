/**************************************************************************
 * C S 429 system emulator
 * 
 * mem.h - Headers for memory parameters used by the emulator. 
 * 
 * Copyright (c) 2022, 2023.
 * Authors: S. Chatterjee, M. Hinton, Z. Leeper.
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _MEM_H_
#define _MEM_H_

#include <stdint.h>
#include <stdbool.h>

// Memory state.
typedef enum {
    L_ENDIAN,
    B_ENDIAN,
    ERROR_ENDIAN = -1
} byte_order_t;

// Addressing granularity of memory.
typedef enum {
    BYTE_GRAN,
    ERROR_GRAN = -1
} mem_gran_t;

// Memory segments corresponding to standard Linux address space.
typedef enum {
    NOACCESS_SEG = 0,   // Inaccessible at EL0
    TEXT_SEG,           // Machine instructions for program (.text)
    DATA_SEG,           // Global data (.data, .bss, .rodata)
    HEAP_SEG,           // Heap data (grows upwards)
    SO_SEG,             // Shared objects
    STACK_SEG,          // Stack data (grows downwards)
    KERNEL_SEG,         // Kernel data (inaccessible at EL0)
    ERROR_SEG = -1
} seg_t;

// Details of memory subsystem.
typedef struct mem {
    unsigned long long max_addr;            // Maximum address (2^W-1)
    unsigned addr_size;                     // Word size of machine (W)
    mem_gran_t gran;                        // Addressing granularity
    uint64_t seg_start_addr[KERNEL_SEG+1];  // Starting addresses of each memory segment
    uint8_t seg_prot[KERNEL_SEG+1];         // Protection bits for each memory segment
} mem_t;

// Status of a memory request. Needed for week 4, when cache delay is modeled.
typedef enum {
    READY,      // Memory access has completed
    IN_FLIGHT,  // Memory access has been initiated but has not yet completed
    ERROR
} mem_status_t;

// Return value read from address.
extern char      mem_read_B (uint64_t address);
extern short     mem_read_S (uint64_t address);
extern int       mem_read_I (uint64_t address);
extern long      mem_read_L (uint64_t address);
extern long long mem_read_LL(uint64_t address);

// Return codes for memory writes.
typedef enum write_ret_code {
    WRITE_FAILURE,
    WRITE_SUCCESS
} write_ret_code_t, mrc_t;

// Write data to address.
extern write_ret_code_t mem_write_B (uint64_t address, char      data);
extern write_ret_code_t mem_write_S (uint64_t address, short     data);
extern write_ret_code_t mem_write_I (uint64_t address, int       data);
extern write_ret_code_t mem_write_L (uint64_t address, long      data);
extern write_ret_code_t mem_write_LL(uint64_t address, long long data);

// Helper functions.
extern bool addr_in_imem(const uint64_t);
extern bool addr_in_dmem(const uint64_t);
extern bool is_special_addr(const uint64_t);

// Important addresses. 
// Accesses to these addresses invoke trap handlers in the emulator.
extern const uint64_t NULL_ADDR;
extern const uint64_t IO_CHAR_ADDR;
extern const uint64_t RET_FROM_MAIN_ADDR;
extern const uint64_t CHECKPOINT_ADDR;
#endif