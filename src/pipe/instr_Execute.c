/**************************************************************************
 * C S 429 system emulator
 * 
 * instr_Execute.c - Execute stage of instruction processing pipeline.
 **************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "err_handler.h"
#include "instr.h"
#include "instr_pipeline.h"
#include "machine.h"
#include "hw_elts.h"

extern machine_t guest;
extern mem_status_t dmem_status;

extern bool X_condval;

extern comb_logic_t copy_m_ctl_sigs(m_ctl_sigs_t *, m_ctl_sigs_t *);
extern comb_logic_t copy_w_ctl_sigs(w_ctl_sigs_t *, w_ctl_sigs_t *);

/*
 * Execute stage logic.
 * STUDENT TO-DO:
 * Implement the execute stage.
 * 
 * Use in as the input pipeline register,
 * and update the out pipeline register as output.
 * 
 * You will need the following helper functions:
 * copy_m_ctl_signals, copy_w_ctl_signals, and alu.
 */

comb_logic_t execute_instr(x_instr_impl_t *in, m_instr_impl_t *out) {
    //connecting wires
    out->m->seq_succ_PC = in->x->seq_succ_PC;
    copy_m_ctl_sigs(out->m->M_sigs, in->x->M_sigs);
    copy_w_ctl_sigs(out->m->W_sigs, in->x->W_sigs);
    out->m->val_b = in->x->val_b;
    out->m->dst = in->x->dst;
    
    //MUX2 
    uint64_t valb = (in->x->X_sigs->valb_sel ? in->x->val_imm : in->x->b);

    //ALU
    alu(in->x->val_a, in->x->val_b, in->x->val_hw, in->x->op, in->x->X_sigs->set_CC, in->x->cond, in->m->val_ex, X_condval);
    out->m->cond_holds = X_condval;

    return;
}
