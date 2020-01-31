/*
 * Generic Interrupt Controller: Distributor (GICD)
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

#include "assert.hpp"
#include "barrier.hpp"
#include "bits.hpp"
#include "gicd.hpp"
#include "hpt.hpp"
#include "lock_guard.hpp"
#include "lowlevel.hpp"
#include "pd.hpp"
#include "stdio.hpp"

Spinlock    Gicd::lock;
unsigned    Gicd::arch;
unsigned    Gicd::ints;
Gicd::Group Gicd::group;

void Gicd::wait_rwp()
{
    if (arch >= 3)
        while (read (Register32::CTLR) & BIT (31))
            pause();
}

void Gicd::init()
{
    if (!GICD_SIZE)
        return;

    unsigned itl = 1;

    if (Cpu::bsp) {

        // Reserve MMIO region
        Pd::remove_mem_user (GICD_BASE, GICD_SIZE);

        for (unsigned size = PAGE_SIZE; size <= GICD_SIZE; size <<= 4) {

            Hptp::master.update (DEV_GLOBL_GICD, GICD_BASE, static_cast<unsigned>(bit_scan_reverse (size)) - PAGE_BITS,
                                 Paging::Permissions (Paging::R | Paging::W | Paging::G),
                                 Memtype::Index::DEV, Memtype::Shareability::NONE);

            auto pidr = Coresight::read (Coresight::Component::PIDR2, DEV_GLOBL_GICD + size);

            if (pidr) {

                auto iidr  = read (Register32::IIDR);
                auto typer = read (Register32::TYPER);
                itl        = (typer & 0x1f) + 1;
                arch       = pidr >> 4 & 0xf;
                ints       = itl * 32;
                group      = arch >= 3 || typer >> 10 & 0x1 ? GROUP1 : GROUP0;

                trace (TRACE_INTR, "GICD: %#010x Impl:%#x Prod:%#x r%up%u (v%u) INTs:%u S:%u G:%u",
                       GICD_BASE,
                       iidr & 0xfff, iidr >> 24, iidr >> 16 & 0xf, iidr >> 12 & 0xf,
                       arch, ints, typer >> 10 & 0x1, group & 1);

                break;
            }
        }
    }

    // Disable interrupt forwarding
    write (Register32::CTLR, 0);

    // BSP initializes all, APs SGI/PPI bank only. v3+ skips SGI/PPI bank
    for (unsigned i = arch < 3 ? 0 : 1; i < itl * 1; i++) {

        // Disable all interrupts
        write (Array32::ICENABLER, i, BIT_RANGE (31, 0));

        // Assign interrupt groups
        write (Array32::IGROUPR, i, group);
    }

    // BSP initializes all, APs SGI/PPI bank only. v3+ skips SGI/PPI bank
    for (unsigned i = arch < 3 ? 0 : 8; i < itl * 8; i++) {

        // Assign interrupt priorities
        write (Array32::IPRIORITYR, i, 0);
    }

    // Wait for completion on CTLR and ICENABLER
    wait_rwp();

    // Enable all SGIs
    if (arch < 3) {
        write (Array32::ISENABLER, 0, BIT_RANGE (15, 0));
        trace (TRACE_INTR, "GICD: Available SGIs: %#x", read (Array32::ISENABLER, 0));
    }

    // Enable interrupt forwarding
    write (Register32::CTLR, arch < 3 ? BIT (0) : BIT (4) | BIT (1));
}

void Gicd::conf (unsigned i, bool edge, unsigned cpu)
{
    assert (i >= SPI_BASE || arch < 3);

    if (i >= ints)
        return;

    Lock_guard <Spinlock> guard (lock);

    // Configure trigger mode
    auto v = read (Array32::ICFGR, i / 16);
    v &= ~(0x3U << 2 * (i % 16));
    v |= (static_cast<uint32>(edge) << 1) << 2 * (i % 16);
    write (Array32::ICFGR, i / 16, v);

    // SGI/PPI CPU targets are read-only
    if (i < SPI_BASE)
        return;

    // Configure SPI CPU target
    if (arch < 3) {
        auto t = read (Array32::ITARGETSR, i / 4);
        t &= ~(0xffU << 8 * (i % 4));
        t |= BIT (cpu) << 8 * (i % 4);
        write (Array32::ITARGETSR, i / 4, t);
    } else {
        uint64 affinity = Cpu::remote_affinity (cpu);
        write (Array64::IROUTER, i, (affinity & 0xff000000) << 8 | (affinity & 0xffffff));
    }
}

void Gicd::mask (unsigned i, bool masked)
{
    assert (i >= SPI_BASE || arch < 3);

    if (i >= ints)
        return;

    if (masked) {
        write (Array32::ICENABLER, i / 32, BIT (i % 32));
        wait_rwp();
    } else
        write (Array32::ISENABLER, i / 32, BIT (i % 32));
}

void Gicd::send_sgi (unsigned sgi, unsigned cpu)
{
    assert (sgi < SGI_NUM && cpu < 8 && arch < 3);

    // Ensure completion (global observability) of all previous stores
    Barrier::wmb_sync();

    write (Register32::SGIR, BIT (cpu + 16) | sgi);
}
