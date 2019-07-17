/*
 * Nested Page Table (NPT)
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

#pragma once

#include "memtype.hpp"
#include "pte.hpp"
#include "vmid.hpp"

class Npt : public Pte<Npt, 3, 9, uint64, uint64>
{
    friend class Pte;

    using Pte::Pte;

    private:
        enum
        {
            ATTR_P      = BIT64  (0),   // Present
            ATTR_nL     = BIT64  (1),   // Not Large
            ATTR_R      = BIT64  (6),   // Readable
            ATTR_W      = BIT64  (7),   // Writable
            ATTR_A      = BIT64 (10),   // Accessed
            ATTR_nX     = BIT64 (54),   // Not Executable
        };

        bool large (unsigned l) const { return l && (val & (ATTR_nL | ATTR_P)) == (ATTR_P); }
        bool table (unsigned l) const { return l && (val & (ATTR_nL | ATTR_P)) == (ATTR_nL | ATTR_P); }

        // Attributes for PTEs mapping page tables
        static Attributes ptab_attr()
        {
            return Attributes (ATTR_nL | ATTR_P);
        }

        // Attributes for PTEs mapping pages
        static Attributes page_attr (unsigned l, Paging::Permissions pm, Memtype::Shareability sh, Memtype::Index mt)
        {
            return Attributes (!(pm & (Paging::Permissions::X | Paging::Permissions::W | Paging::Permissions::R)) ? 0 :
                               !(pm & Paging::Permissions::X) * (ATTR_nX) |
                              !!(pm & Paging::Permissions::W) * (ATTR_W)  |
                              !!(pm & Paging::Permissions::R) * (ATTR_R)  |
                               (static_cast<mword>(sh) & 0x3) << 8 | (Memtype::npt_attr (mt) & 0xf) << 2 | !l * ATTR_nL | ATTR_A | ATTR_P);
        }

        static Paging::Permissions page_pm (Attributes a)
        {
            return Paging::Permissions (!a ? 0 :
                                       !(a & ATTR_nX) * Paging::Permissions::X |
                                      !!(a & ATTR_W)  * Paging::Permissions::W |
                                      !!(a & ATTR_R)  * Paging::Permissions::R);
        }

        static Memtype::Shareability page_sh (Attributes a) { return Memtype::Shareability (a >> 8 & 0x3); }

        static Memtype::Index page_mt (Attributes a) { return Memtype::Index ((a & 0x30 ? 4 : 0) + (a >> 2 & 0x3)); }

    protected:
        static OAddr const ADDR_MASK = 0x0000fffffffff000;
        static OAddr const ATTR_MASK = 0xfff0000000000fff;

    public:
        static unsigned const ord = 3 * bpl - 1;
};

class Nptp : public Npt
{
    private:
        static uint64 current CPULOCAL;

    public:
        ALWAYS_INLINE
        inline void make_current (Vmid v)
        {
            uint64 vttbr = v.vmid() << 48 | addr();

            if (current != vttbr)
                asm volatile ("msr vttbr_el2, %0; isb" : : "r" (current = vttbr) : "memory");
        }

        ALWAYS_INLINE
        inline void flush (Vmid v)
        {
            make_current (v);

            asm volatile ("dsb  ishst           ;"  // Ensure PTE writes have completed
                          "tlbi vmalls12e1is    ;"  // Invalidate TLB
                          "dsb  ish             ;"  // Ensure TLB invalidation completed
                          "isb                  ;"  // Ensure subsequent instructions use new translation
                          : : : "memory");
        }
};
