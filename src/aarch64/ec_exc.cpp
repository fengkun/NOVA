/*
 * Execution Context (EC)
 *
 * Copyright (C) 2019-2020 Udo Steinberg, BedRock Systems, Inc.
 *
 * This file is part of the NOVA microhypervisor.
 *
 * NOVA is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * NOVA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License version 2 for more details.
 */

#include "assert.hpp"
#include "ec.hpp"
#include "event.hpp"
#include "interrupt.hpp"
#include "stdio.hpp"

extern "C"
void (*const syscall[16])() =
{
    &Ec::sys_ipc_call,
    &Ec::sys_ipc_reply,
    &Ec::sys_create_pd,
    &Ec::sys_create_ec,
    &Ec::sys_create_sc,
    &Ec::sys_create_pt,
    &Ec::sys_create_sm,
    &Ec::sys_ctrl_pd,
    &Ec::sys_ctrl_ec,
    &Ec::sys_ctrl_sc,
    &Ec::sys_ctrl_pt,
    &Ec::sys_ctrl_sm,
    &Ec::sys_ctrl_hw,
    &Ec::sys_assign_int,
    &Ec::sys_assign_dev,
    &Ec::sys_finish<Sys_regs::BAD_HYP>,
};

void Ec::switch_fpu()
{
    assert (!(Cpu::hazard & HZD_FPU));
    assert (fpowner != this);

    if (EXPECT_FALSE (!fpu))
        return;

    Fpu::enable();

    if (EXPECT_TRUE (fpowner)) {
        fpowner->fpu->save();
        fpowner->clr_hazard (HZD_FPU);
    }

    fpu->load();
    set_hazard (HZD_FPU);

    trace (TRACE_FPU, "Switching FPU %p -> %p", static_cast<void *>(fpowner), static_cast<void *>(this));

    fpowner = this;

    subtype == Kobject::Subtype::EC_VCPU ? ret_user_vmexit() : ret_user_exception();
}

void Ec::handle_exc_kern (Exc_regs *r)
{
    mword iss = r->el2_esr & 0x1ffffff;
    mword isv = iss >> 24 & 0x1;
    mword srt = iss >> 16 & 0x1f;
    bool  wrt = iss >> 6 & 0x1;

    switch (r->ep()) {

        case 0x21:      // Instruction Abort
            trace (0, "KERN inst abort (ISS:%#lx) at IP:%#lx FAR:%#lx", iss, r->el2_elr, r->el2_far);
            if (isv)
                trace (0, "%s via X%lu (%#lx)", wrt ? "ST" : "LD", srt, r->r[srt]);
            break;

        case 0x25:      // Data Abort
            trace (0, "KERN data abort (ISS:%#lx) at IP:%#lx FAR:%#lx", iss, r->el2_elr, r->el2_far);
            if (isv)
                trace (0, "%s via X%lu (%#lx)", wrt ? "ST" : "LD", srt, r->r[srt]);
            break;

        default:
            trace (0, "KERN exception %#lx (ISS:%#lx) at IP:%#lx", r->ep(), iss, r->el2_elr);
            break;
    }

    kill ("exception");
}

void Ec::handle_exc_user (Exc_regs *r)
{
    switch (r->ep()) {

        case 0x7:       // FPU access
            current->switch_fpu();
            break;

        case 0x15:      // SVC from AArch64 state
            (*syscall[r->r[0] & 0xf])();
            UNREACHED;
    }

    if (current->subtype == Kobject::Subtype::EC_VCPU) {
        trace (TRACE_EXCEPTION, "EC:%p VMX %#lx at M:%#x IP:%#lx", static_cast<void *>(current), r->ep(), r->emode(), r->el2_elr);
        current->vmcb->save_gst();
        send_msg<ret_user_vmexit>();
    } else {
        trace (TRACE_EXCEPTION, "EC:%p EXC %#lx at M:%#x IP:%#lx", static_cast<void *>(current), r->ep(), r->emode(), r->el2_elr);
        send_msg<ret_user_exception>();
    }
}

void Ec::handle_irq_kern()
{
    Interrupt::handler (false);
}

void Ec::handle_irq_user()
{
    Event::Selector evt = Interrupt::handler (current->subtype == Kobject::Subtype::EC_VCPU);

    if (current->subtype != Kobject::Subtype::EC_VCPU)
        ret_user_exception();

    current->vmcb->save_gst();

    if (evt == Event::Selector::NONE)
        ret_user_vmexit();

    current->regs.set_ep (Event::gst_arch + evt);
    send_msg<ret_user_vmexit>();
}
