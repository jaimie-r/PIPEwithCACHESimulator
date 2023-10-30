/**************************************************************************
 * C S 429 system emulator
 * 
 * instr.h - Header file for everything architectural related to 
 * chArm-v2 instructions.
 * 
 * Copyright (c) 2022, 2023.
 * Authors: S. Chatterjee, Si. Nemana, Z. Leeper.
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _INSTR_H_
#define _INSTR_H_

#include <stdint.h>
#include <stdbool.h>
#include "reg.h"
#include "mem.h"
// #include "forward.h"

typedef void comb_logic_t;

extern uint32_t bitfield_u32(int32_t src, unsigned frompos, unsigned width);
extern int64_t bitfield_s64(int32_t src, unsigned frompos, unsigned width);

// #define EXTRACT(src, mask, shift) (((src) & (mask)) >> (shift))
// #define GETBF(src, frompos, width) safe_GETBF(((int32_t) (src)), (frompos), (width))

/* Used to set condition flags for flag setting instructions
 *
 * N: Negative condition flag
 *      Set to 1 when the result of the instruction is negative
 *      Set to 0 when the result of the instruction is non-negative
 * Z: Zero condition flag
 *      Set to 1 when the result of the instruction is zero
 *      Set to 0 when the result of the instruction is non-zero
 * C: Carry condition flag
 *      Set to 1 if instruction results in a carry condition, like an unsigned overflow that is the result of an addition
 *      Set to 0 otherwise
 * V: Overflow condition flag
 *      Set to 1 if the instruction results in an overflow condition, like a signed overflow that is the result of an addition
 *      Set to 0 otherwise
 * 
 */

#define PACK_CC(n,z,c,v) (((n)<<3)|((z)<<2)|((c)<<1)|((v)<<0))

//Negative Condition flag
#define GET_NF(cc) (((cc) >> 3)&0x1) 
//Zero Condition flag
#define GET_ZF(cc) (((cc) >> 2)&0x1) 
//Carry Condition flag
#define GET_CF(cc) (((cc) >> 1)&0x1) 
//Overflow Condition flag
#define GET_VF(cc) (((cc) >> 0)&0x1) 

// Opcodes for all chArm-v2 instructions.
typedef enum opcode {
    OP_NOP,
    // Data transfer
    // OP_LDURB,
    OP_LDUR,
    // OP_STURB,
    OP_STUR,
    OP_MOVK,
    OP_MOVZ,
    // Computation
    OP_ADRP,
    OP_ADD_RI,
    OP_ADDS_RR,
    OP_SUB_RI,
    OP_SUBS_RR,
    OP_CMP_RR,
    OP_MVN,
    OP_ORR_RR,
    OP_EOR_RR,
    OP_ANDS_RR,
    OP_TST_RR,
    OP_LSL,
    OP_LSR,
    // LSL and LSR are implemented in terms of UBFM
    OP_UBFM,
    OP_ASR,
    // Control transfer
    OP_B,
    OP_B_COND,
    OP_BL,
    OP_RET,
    // Misc
    OP_HLT,
    OP_ERROR = -1
} opcode_t;
// TODO: Add enum for 1-hot encoding

// Lookup table mapping the most-significant 11 bits of an instruction to its opcode.
extern opcode_t itable[];

// Condition codes. 
// C1.2.4, p. C1-225.
typedef enum cond {
    C_EQ,
    C_NE,
    C_CS,
    C_CC,
    C_MI,
    C_PL,
    C_VS,
    C_VC,
    C_HI,
    C_LS,
    C_GE,
    C_LT,
    C_GT,
    C_LE,
    C_AL,
    C_NV,
    C_ERROR = -1
} cond_t;

// State of an instruction.
typedef enum insn_state {
    STAT_BUB = 0,   // pipeline bubble
    STAT_AOK,       // all is good
    STAT_HLT,       // end of program (clean exit)
    STAT_ADR,       // bad address
    STAT_INS        // bad instruction
} stat_t;

/* Function prototypes. */
extern void init_itable(void);
#endif