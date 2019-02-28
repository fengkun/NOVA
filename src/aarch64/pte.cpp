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

#include "assert.hpp"
#include "barrier.hpp"
#include "hpt.hpp"
#include "pte.hpp"
#include "stdio.hpp"
#include "util.hpp"

template <typename PTT, unsigned L, unsigned B, typename I, typename O>
PTT *Pte<PTT,L,B,I,O>::walk (IAddr v, unsigned n, bool a)
{
    unsigned l = L;

    for (PTT pte, *e = static_cast<PTT *>(this);; e = static_cast<PTT *>(Buddy::phys_to_ptr (pte.addr())) + (v >> (--l * B + PAGE_BITS) & (BIT (B) - 1))) {

        trace (TRACE_PTE, "%s %#llx: l=%u e=%p val=%#llx", __func__, v, l, static_cast<void *>(e), static_cast<uint64>(e->val));

        if (l == n)
            return e;

        for (;;) {

            pte.val = Atomic::load (e->val);

            if (!pte.val && !a)
                return nullptr;

            if (!pte.val || pte.large (l)) {

                PTT *p = !pte.val ? new PTT (0, 0) : new PTT (pte.addr (l) | PTT::page_attr (l - 1, PTT::page_pm (pte.attr()), PTT::page_sh (pte.attr()), PTT::page_mt (pte.attr())), page_size ((l - 1) * B));

                OAddr tmp = Buddy::ptr_to_phys (p) | PTT::ptab_attr();

                if (!Atomic::cmp_swap (e->val, pte.val, tmp)) {
                    delete p;
                    continue;
                }

                pte.val = tmp;
            }

            break;
        }
    }
}

template <typename PTT, unsigned L, unsigned B, typename I, typename O>
Paging::Permissions Pte<PTT,L,B,I,O>::lookup (IAddr v, OAddr &p, unsigned &o, Memtype::Index &mt, Memtype::Shareability &sh)
{
    unsigned l = L;

    for (PTT pte, *e = static_cast<PTT *>(this);; e = static_cast<PTT *>(Buddy::phys_to_ptr (pte.addr())) + (v >> (--l * B + PAGE_BITS) & (BIT (B) - 1))) {

        o = l * B;

        pte.val = Atomic::load (e->val);

        if (!pte.val)
            return Paging::Permissions (p = 0);

        if (pte.table (l))
            continue;

        p = pte.addr (l) | (v & page_mask (o));

        sh = PTT::page_sh (pte.attr());
        mt = PTT::page_mt (pte.attr());

        return PTT::page_pm (pte.attr());
    }
}

template <typename PTT, unsigned L, unsigned B, typename I, typename O>
void Pte<PTT,L,B,I,O>::update (IAddr v, OAddr p, unsigned ord, Paging::Permissions pm, Memtype::Index mt, Memtype::Shareability sh)
{
    assert ((v & page_mask (ord)) == 0);
    assert ((p & page_mask (ord)) == 0);

    unsigned o = min (ord, PTT::ord);
    unsigned l = o / B;

    Attributes a = PTT::page_attr (l, pm, sh, mt);

    for (unsigned i = 0; i < BIT (ord - o); i++, v += BIT (o + PAGE_BITS), p += BIT (o + PAGE_BITS)) {

        trace (TRACE_PTE, "%s: l=%u v=%#llx p=%#llx o=%#x a=%#lx", __func__, l, v, p, o, a);

        PTT pte, *e = walk (v, l, a);

        if (!e)
            return;

        OAddr n = a ? p | a : 0;
        OAddr s = a ? page_size (l * B) : 0;

        for (unsigned j = 0; j < BIT (o % B); j++, n += s) {

            pte.val = Atomic::exchange (e[j].val, n);

            if (!pte.val)
                continue;

            if (pte.table (l))
                static_cast<PTT *>(Buddy::phys_to_ptr (pte.addr()))->deallocate (l - 1);
        }
    }

    Barrier::fmb_sync();
}

template <typename PTT, unsigned L, unsigned B, typename I, typename O>
void Pte<PTT,L,B,I,O>::deallocate (unsigned l)
{
    if (!l)
        return;

    for (auto e = static_cast<PTT *>(this); e < static_cast<PTT *>(this) + BIT (B); e++) {

        if (!e->val)
            continue;

        if (e->table (l))
            static_cast<PTT *>(Buddy::phys_to_ptr (e->addr()))->deallocate (l - 1);
    }

    delete this;
}

template class Pte<Hpt, 4, 9, uint64, uint64>;
