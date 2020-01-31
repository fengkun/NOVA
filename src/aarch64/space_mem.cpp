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

#include "space_mem.hpp"

void Space_mem::update (uint64 v, uint64 p, unsigned o, Paging::Permissions pm, Memtype::Index mt, Memtype::Shareability sh, Space::Index si)
{
    switch (si) {
        case Space::Index::MEM_HST: mem_hst.update (v, p, o, pm, mt, sh); break;
        case Space::Index::MEM_GST: mem_gst.update (v, p, o, pm, mt, sh); break;
        case Space::Index::DMA_HST: dma_hst.update (v, p, o, pm, mt, sh); break;
        case Space::Index::DMA_GST: dma_gst.update (v, p, o, pm, mt, sh); break;
    }
}

void Space_mem::flush (Space::Index si)
{
    switch (si) {
        case Space::Index::MEM_HST: mem_hst.flush (id_hst); break;
        case Space::Index::MEM_GST: mem_gst.flush (id_gst); break;
        case Space::Index::DMA_HST: dma_hst.flush (id_hst); break;
        case Space::Index::DMA_GST: dma_gst.flush (id_gst); break;
    }
}
