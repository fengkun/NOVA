/*
 * Memory Space
 *
 * Copyright (C) 2009-2011 Udo Steinberg <udo@hypervisor.org>
 * Economic rights: Technische Universitaet Dresden (Germany)
 *
 * Copyright (C) 2012-2013 Udo Steinberg, Intel Corporation.
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

#include "compiler.hpp"
#include "dpt.hpp"
#include "hpt.hpp"
#include "npt.hpp"
#include "space.hpp"
#include "types.hpp"

class Space_mem : public Space
{
    private:
        Vmid    id_hst;
        Vmid    id_gst;
        Nptp    mem_hst;
        Nptp    mem_gst;
        Dptp    dma_hst;
        Dptp    dma_gst;

    public:
        ALWAYS_INLINE
        inline Space_mem()
        {
            mem_hst.init_root();
            mem_gst.init_root();
        }

        inline auto vmid_hst() const { return id_hst; }
        inline auto vmid_gst() const { return id_gst; }

        inline auto ptab_hst() { return dma_hst.init_root(); }
        inline auto ptab_gst() { return dma_gst.init_root(); }

        inline void make_current_hst() { mem_hst.make_current (id_hst); }
        inline void make_current_gst() { mem_gst.make_current (id_gst); }

        Paging::Permissions lookup (uint64 v, uint64 &p, unsigned &o, Memtype::Index &mt, Memtype::Shareability &sh) { return mem_hst.lookup (v, p, o, mt, sh); }

        void update (uint64, uint64, unsigned, Paging::Permissions, Memtype::Index, Memtype::Shareability, Space::Index = Space::Index::MEM_HST);

        void flush (Space::Index);
};
