/*
 * Page Table Entry (PTE)
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

#include "atomic.hpp"
#include "buddy.hpp"
#include "memtype.hpp"
#include "paging.hpp"

template <typename PTT, unsigned L, unsigned B, typename I, typename O>
class Pte
{
    public:
        typedef I IAddr;
        typedef O OAddr;

        static OAddr page_size (unsigned o) { return 1UL << (o + PAGE_BITS); }
        static OAddr page_mask (unsigned o) { return page_size (o) - 1; }

        OAddr init_root (unsigned l = L - 1)
        {
            return Buddy::ptr_to_phys (walk (0, l, true));
        }

        Paging::Permissions lookup (IAddr, OAddr &, unsigned &, Memtype::Index &, Memtype::Shareability &);

        void update (IAddr, OAddr, unsigned, Paging::Permissions, Memtype::Index, Memtype::Shareability);

    protected:
        enum Attributes { MAX = ~0UL };

        static unsigned const lev = L;
        static unsigned const bpl = B;

        OAddr val;

        OAddr addr (unsigned l = 0) const { return val & PTT::ADDR_MASK & ~page_mask (l * B); }

        Attributes attr() const { return Attributes (val & PTT::ATTR_MASK); }

        Pte() = default;

        Pte (OAddr p, OAddr s)
        {
            for (unsigned i = 0; i < 1UL << B; i++, p += s)
                this[i].val = p;
        }

        PTT *walk (IAddr, unsigned, bool);

    private:
        void deallocate (unsigned);

        static void *operator new (size_t)
        {
            return Buddy::allocator.alloc (0, Buddy::FILL_0);
        }

        static void operator delete (void *ptr)
        {
            Buddy::allocator.free (reinterpret_cast<mword>(ptr));
        }
};
