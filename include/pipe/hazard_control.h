/**************************************************************************
 * C S 429 system emulator
 * 
 * hazard_control.h - Headers for pipeline control unit in the emulator. 
 * 
 * Copyright (c) 2022. 
 * Authors: S. Chatterjee. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _HAZARD_CONTROL_H_
#define _HAZARD_CONTROL_H_
#include <stdint.h>
#include "instr.h"

extern void pipe_control_stage(proc_stage_t stage, bool bubble, bool stall);
extern bool check_ret_hazard(opcode_t D_opcode);
extern bool check_mispred_branch_hazard(opcode_t X_opcode, bool X_condval);
extern bool check_load_use_hazard(opcode_t D_opcode, uint8_t D_src1, uint8_t D_src2, opcode_t X_opcode, uint8_t X_dst);
extern void handle_hazards(opcode_t D_opcode, uint8_t D_src1, uint8_t D_src2, opcode_t X_opcode, uint8_t X_dst, bool X_condval);
#endif