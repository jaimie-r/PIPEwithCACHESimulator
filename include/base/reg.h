/**************************************************************************
 * C S 429 system emulator
 * 
 * reg.h - Headers for register parameters used by the emulator. 
 * 
 * Copyright (c) 2022. 
 * Authors: S. Chatterjee. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _REG_H_
#define _REG_H_

#include <stdint.h>

// The width of the register in bits.
typedef enum wvar {
    WVAR_4,         // Used only for NZCV
    WVAR_32,        // Used for W registers
    WVAR_64,        // Used for X registers
    WVAR_ERROR = -1
} wvar_t;

// The contents of the register.
typedef union gpregval {
    uint8_t ccval;  // This is to be used only for condition codes, i.e., for the NZCV register.
    int8_t  sval;   // ??
    int32_t wval;   // For W registers
    int64_t xval;   // For X registers
} gpregval_t, val_t;// TODO: Check dependence on host machine endian-ness.

// A single register.
typedef struct reg {
    char        *name;  // The name of the register.
    unsigned    index;  // An index into a register file for this register.
    wvar_t      width;  // Bit width of the register.
    gpregval_t  *bits;   // A pointer to the bit storage.
} reg_t;

// Register file.
typedef struct reg_file {
    char        *name;      // Name of the register file.
    unsigned    num;        // How many registers in the file.
    unsigned    width;      // How wide is each register?
    gpregval_t  *bits;      // Storage area for the bits. Allocate an array of "num" gpregval_t structs.
    reg_t       *names32;   // Pointers to the W variant of the registers.
    reg_t       *names64;   // Pointers to the X variant of the registers.
} reg_file_t;

extern char *reg_prefix[];  // For printing.

// Should be called only during emulator initialization.
extern void init_reg(reg_t *r, char *name, unsigned index, wvar_t width, gpregval_t *bits);
extern void init_reg_file(reg_file_t *rf, char *name, unsigned num, unsigned width);
#endif