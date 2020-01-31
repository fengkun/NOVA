/*
 * Bootstrap Code
 *
 * Copyright (C) 2019 Udo Steinberg, BedRock Systems, Inc.
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

#include "atomic.hpp"
#include "compiler.hpp"
#include "cpu.hpp"
#include "lowlevel.hpp"

extern "C" NORETURN
void bootstrap (unsigned i, unsigned e)
{
    static mword barrier;

    Cpu::init (i, e);

    // Barrier: wait for all CPUs to arrive here
    for (Atomic::add (barrier, 1UL); barrier != Cpu::online; pause()) ;

    for (;;) {}
}
