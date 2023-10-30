/**************************************************************************
 * STUDENTS: DO NOT MODIFY.
 * 
 * C S 429 system emulator
 * 
 * reg.c - Module for defining and initializing register files.
 * 
 * Copyright (c) 2022. 
 * Authors: S. Chatterjee. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include <stdlib.h>
#include <string.h>
#include "reg.h"

static char *GPR_names32[] = {
    " W0", " W1", " W2", " W3", " W4", " W5", " W6", " W7",
    " W8", " W9", "W10", "W11", "W12", "W13", "W14", "W15",
    "W16", "W17", "W18", "W19", "W20", "W21", "W22", "W23",
    "W24", "W25", "W26", "W27", "W28", "W29", "W30"
};

static char *GPR_names64[] = {
    " X0", " X1", " X2", " X3", " X4", " X5", " X6", " X7",
    " X8", " X9", "X10", "X11", "X12", "X13", "X14", "X15",
    "X16", "X17", "X18", "X19", "X20", "X21", "X22", "X23",
    "X24", "X25", "X26", "X27", "X28", "X29", "X30"
};

// static char *FPR_names[] = {
//      "V0",  "V1",  "V2",  "V3",  "V4",  "V5",  "V6",  "V7",
//      "V8",  "V9", "V10", "V11", "V12", "V13", "V14", "V15",
//     "V16", "V17", "V18", "V19", "V20", "V21", "V22", "V23",
//     "V24", "V25", "V26", "V27", "V28", "V29", "V30", "V31"
// };

char *reg_prefix[] = {"", "W", "X"};

static inline wvar_t reg_width(unsigned width) {
    switch (width) {
        case 4: return WVAR_4;
        case 32: return WVAR_32;
        case 64: return WVAR_64;
        default: return WVAR_ERROR;
    }
}

void init_reg(reg_t *r, char *name, unsigned index, wvar_t width, gpregval_t *bits) {
    r->name = malloc(strlen(name)+1);
    strcpy(r->name, name);
    r->index = index;
    r->width = width;
    r->bits = bits;
}

void init_reg_file(reg_file_t *rf, char *name, unsigned num, unsigned width) {
    rf->name = malloc(strlen(name)+1);
    strcpy(rf->name, name);
    rf->num = num;
    rf->width = reg_width(width);
    rf->bits = malloc(num*sizeof(gpregval_t));
    rf->names32 = malloc(num*sizeof(reg_t));
    rf->names64 = malloc(num*sizeof(reg_t));
    if (0 == strcmp(name, "GPR")) {
        for (int i = 0; i < num; i++) {
            init_reg(rf->names32+i, GPR_names32[i], i, WVAR_32, rf->bits+i);
            init_reg(rf->names64+i, GPR_names64[i], i, WVAR_64, rf->bits+i);
        }
        return;
    }
    // TODO
    // if (0 == strcmp(name, "FPR")) {
    //     for (int i = 0; i < num; i++)
    //         init_reg(rf->regs+i, FPR_names[i], i, rf->width, );
    //     return;
    // }
}