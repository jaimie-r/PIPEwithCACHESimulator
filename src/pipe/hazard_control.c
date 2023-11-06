/**************************************************************************
 * C S 429 system emulator
 * 
 * Bubble and stall checking logic.
 * STUDENT TO-DO:
 * Implement logic for hazard handling.
 * 
 * handle_hazards is called from proc.c with the appropriate
 * parameters already set, you must implement the logic for it.
 * 
 * You may optionally use the other three helper functions to 
 * make it easier to follow your logic.
 **************************************************************************/ 

#include "machine.h"

extern machine_t guest;
extern mem_status_t dmem_status;

/* Use this method to actually bubble/stall a pipeline stage.
 * Call it in handle_hazards(). Do not modify this code. */
void pipe_control_stage(proc_stage_t stage, bool bubble, bool stall) {
    pipe_reg_t *pipe;
    switch(stage) {
        case S_FETCH: pipe = F_instr; break;
        case S_DECODE: pipe = D_instr; break;
        case S_EXECUTE: pipe = X_instr; break;
        case S_MEMORY: pipe = M_instr; break;
        case S_WBACK: pipe = W_instr; break;
        default: printf("Error: incorrect stage provided to pipe control.\n"); return;
    }
    if (bubble && stall) {
        printf("Error: cannot bubble and stall at the same time.\n");
        pipe->ctl = P_ERROR;
    }
    // If we were previously in an error state, stay there.
    if (pipe->ctl == P_ERROR) return;

    if (bubble) {
        pipe->ctl = P_BUBBLE;
    }
    else if (stall) {
        pipe->ctl = P_STALL;
    }
    else { 
        pipe->ctl = P_LOAD;
    }
}

bool check_ret_hazard(opcode_t D_opcode) {
    /* Students: Implement Below */
    return (D_opcode == OP_RET);
}

bool check_mispred_branch_hazard(opcode_t X_opcode, bool X_condval) {
    /* Students: Implement Below */
    return (X_opcode == OP_B_COND && !X_condval);
    //x_out->op == OP_B_COND && !X_condval 
    //or ret in D (squash ret)
    //bubble D, bubble X 

}

bool check_load_use_hazard(opcode_t D_opcode, uint8_t D_src1, uint8_t D_src2,
                           opcode_t X_opcode, uint8_t X_dst) {
    /* Students: Implement Below */
    //*issue only when it's back to back load use
    //if x is ldur to one of the sources
    //xout->op == OP_LDUR && (x.out->dst == src1 || x.out->dst == src2)
    return ((X_opcode == OP_LDUR && ((X_dst == D_src1 || X_dst == D_src2))) || (D_opcode == OP_RET && X_dst == 30));
    //or if ret in D && dst == X30 
    //stall F, stall D, bubble X 

}

comb_logic_t handle_hazards(opcode_t D_opcode, uint8_t D_src1, uint8_t D_src2, 
                            opcode_t X_opcode, uint8_t X_dst, bool X_condval) {
    /* Students: Change the below code IN WEEK TWO -- do not touch for week one */
    //
    //in f/d/x/m/w do we in each stage check for what to do based on status? ex stall/bubble
    bool f_stall = F_out->status == STAT_HLT || F_out->status == STAT_INS;
    bool d_stall = D_out->status == STAT_HLT || D_out->status == STAT_INS;
    bool x_stall = X_out->status == STAT_HLT || X_out->status == STAT_INS;
    bool m_stall = M_out->status == STAT_HLT || M_out->status == STAT_INS;
    bool w_stall = W_out->status == STAT_HLT || W_out->status == STAT_INS;
    if(check_ret_hazard(D_opcode)){
        //stall F, bubble D
        pipe_control_stage(S_FETCH, false, true);
        pipe_control_stage(S_DECODE, true, false);
        pipe_control_stage(S_EXECUTE, false, false);
        pipe_control_stage(S_MEMORY, false, false);
        pipe_control_stage(S_WBACK, false, false);
    } else if (check_mispred_branch_hazard(X_opcode, X_condval)){
        //bubble D, bubble X
        pipe_control_stage(S_FETCH, false, false);
        pipe_control_stage(S_DECODE, true, false);
        pipe_control_stage(S_EXECUTE, true, false);
        pipe_control_stage(S_MEMORY, false, false);
        pipe_control_stage(S_WBACK, false, false);
    } else if (check_load_use_hazard(D_opcode, D_src1, D_src2, X_opcode, X_dst)){
        //stall F, stall D, bubble X 
        pipe_control_stage(S_FETCH, false, true);
        pipe_control_stage(S_DECODE, false, true);
        pipe_control_stage(S_EXECUTE, true, false);
        pipe_control_stage(S_MEMORY, false, false);
        pipe_control_stage(S_WBACK, false, false);
    }else {
        pipe_control_stage(S_FETCH, false, (f_stall || d_stall || x_stall || m_stall || w_stall));
        pipe_control_stage(S_DECODE, false, (d_stall || x_stall || m_stall || w_stall));
        pipe_control_stage(S_EXECUTE, false, (x_stall || m_stall || w_stall));
        pipe_control_stage(S_MEMORY, false, (m_stall || w_stall));
        pipe_control_stage(S_WBACK, false, w_stall);
    }

    //exceptions?,,,,
    //if x_out status != AOK || BUB
    //if W_in status != AOK || BUB
    //if W_out status != AOK || BUB 
}



