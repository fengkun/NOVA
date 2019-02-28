/*
 * Host Page Table (HPT)
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

#pragma once

#include "memtype.hpp"
#include "pte.hpp"

class Hpt : public Pte<Hpt, 4, 9, uint64, uint64>
{
    friend class Pte;

    using Pte::Pte;

    private:
        enum
        {
            ATTR_P      = BIT64  (0),   // Present
            ATTR_nL     = BIT64  (1),   // Not Large
            ATTR_nS     = BIT64  (5),   // Not Secure
            ATTR_U      = BIT64  (6),   // User
            ATTR_nW     = BIT64  (7),   // Not Writable
            ATTR_A      = BIT64 (10),   // Accessed
            ATTR_nG     = BIT64 (11),   // Not Global
            ATTR_nXP    = BIT64 (53),   // Not Executable (Privileged)
            ATTR_nXU    = BIT64 (54),   // Not Executable (Unprivileged)
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
                               !(pm & Paging::Permissions::G) * (ATTR_nG) |
                              !!(pm & Paging::Permissions::U) * (ATTR_U)  |
                               !(pm & Paging::Permissions::X) * (ATTR_nXU | ATTR_nXP) |
                               !(pm & Paging::Permissions::W) * (ATTR_nW) |
                               (static_cast<mword>(sh) & 0x3) << 8 | (static_cast<mword>(mt) & 0x7) << 2 | !l * ATTR_nL | ATTR_A | ATTR_P);
        }

        static Paging::Permissions page_pm (Attributes a)
        {
            return Paging::Permissions (!a ? 0 :
                                       !(a & ATTR_nG)  * Paging::Permissions::G |
                                      !!(a & ATTR_U)   * Paging::Permissions::U |
                                       !(a & ATTR_nXU) * Paging::Permissions::X |
                                       !(a & ATTR_nW)  * Paging::Permissions::W |
                                      !!(a & ATTR_P)   * Paging::Permissions::R);
        }

        static Memtype::Shareability page_sh (Attributes a) { return Memtype::Shareability (a >> 8 & 0x3); }

        static Memtype::Index page_mt (Attributes a) { return Memtype::Index (a >> 2 & 0x7); }

    protected:
        static OAddr const ADDR_MASK = 0x0000fffffffff000;
        static OAddr const ATTR_MASK = 0xfff0000000000fff;

    public:
        static unsigned const ord = 3 * bpl - 1;

        void sync_from_master (mword, mword);

        static void *map (OAddr, bool = false);
};

class Hptp : public Hpt
{
    public:
        static Hptp master;

        explicit Hptp (OAddr v = 0) { val = v; }

        static Hptp current()
        {
            uint64 val;
            asm volatile ("mrs %0, ttbr0_el2" : "=r" (val));
            return Hptp (val & ADDR_MASK);
        }

        ALWAYS_INLINE
        inline void make_current()
        {
            asm volatile ("msr ttbr0_el2, %0; isb" : : "r" (addr()) : "memory");
        }

        ALWAYS_INLINE
        static inline void flush_cpu()
        {
            asm volatile ("dsb  nshst   ;"  // Ensure PTE writes have completed
                          "tlbi alle2   ;"  // Invalidate TLB
                          "dsb  nsh     ;"  // Ensure TLB invalidation completed
                          "isb          ;"  // Ensure subsequent instructions use new translation
                          : : : "memory");
        }
};
