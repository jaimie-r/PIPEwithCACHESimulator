/**************************************************************************
 * C S 429 system emulator
 * 
 * instr_Memory.c - Memory stage of instruction processing pipeline.
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

extern comb_logic_t copy_w_ctl_sigs(w_ctl_sigs_t *, w_ctl_sigs_t *);

/*
 * Memory stage logic.
 * STUDENT TO-DO:
 * Implement the memory stage.
 * 
 * Use in as the input pipeline register,
 * and update the out pipeline register as output.
 * 
 * You will need the following helper functions:
 * copy_w_ctl_signals and dmem.
 */

comb_logic_t memory_instr(m_instr_impl_t *in, w_instr_impl_t *out) {
    in->m->op = out->w->op;
    in->m->print_op = out->w->print_op;
    in->m->val_ex = out->w->val_ex;
    in->m->status = out->w->status;
    copy_w_ctl_sigs(out->w->W_sigs, in->m->M_sigs);
    in->m->dst = out->w->dst;
    // questions:
    // seq succ PC?
    // cond_holds? for when branching is wrong?
    // where to change status?
    if(in->m->dmem_read || in->m->dmem_write) {
        dmem(in->m->val_b, in->m->val_ex, in->m->M_sigs->dmem_read, in->m->M_sigs->dmem_write); 
    }
    return;
}
