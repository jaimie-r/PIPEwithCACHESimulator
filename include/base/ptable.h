/**************************************************************************
 * C S 429 system emulator
 * 
 * ptable.h - Headers for "page table" parameters used by the emulator.
 * 
 * This is not a real paged virtual memory. It is just a way to provide the
 * illusion of a 64-bit address page and materialize pages on first touch.
 * In particular, there is no page replacement.
 * 
 * Copyright (c) 2022. S. Chatterjee. All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _PTABLE_H_
#define _PTABLE_H_
#include <stdint.h>

/* This corresponds to the Arm64 notion of a hardware page (see ADRP) */
#define PAGESIZE 4096

// A page table entry.
typedef struct pte {
    uint64_t p_num;     // The page number.
    unsigned p_prot;    // The page protection bits.
    char *p_data;       // The page payload.
    struct pte *p_next; // Link to next PTE.
} pte_t, *pte_ptr_t;

// Get a pointer to a PTE given its page number.
extern pte_ptr_t get_page(const uint64_t);
// Materialize a page with the given page number and protection bits.
extern pte_ptr_t add_page(const uint64_t, const uint8_t);
#endif