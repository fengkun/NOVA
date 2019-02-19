/*
 * Memory Types
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

#include "suffix.hpp"

#define MT_ATTR_DEV     0x00    // Device
#define MT_ATTR_DEV_E   0x04    // Device, Early Ack
#define MT_ATTR_DEV_RE  0x08    // Device, Early Ack + Reordering
#define MT_ATTR_DEV_GRE 0x0c    // Device, Early Ack + Reordering + Gathering
#define MT_ATTR_MEM_NC  0x44    // Memory, Outer + Inner NC
#define MT_ATTR_MEM_WT  0xbb    // Memory, Outer + Inner WT Non-Transient
#define MT_ATTR_MEM_WB  0xff    // Memory, Outer + Inner WB Non-Transient

#define MT_IDX_DEV      0
#define MT_IDX_DEV_E    1
#define MT_IDX_DEV_RE   2
#define MT_IDX_DEV_GRE  3
#define MT_IDX_MEM_NC   5
#define MT_IDX_MEM_WT   6
#define MT_IDX_MEM_WB   7

#define MT_ENC(ATTR, IDX)  (SFX_ULL(ATTR) << ((IDX) << 3))

#define MAIR_VAL        MT_ENC(MT_ATTR_DEV,     MT_IDX_DEV)     |   \
                        MT_ENC(MT_ATTR_DEV_E,   MT_IDX_DEV_E)   |   \
                        MT_ENC(MT_ATTR_DEV_RE,  MT_IDX_DEV_RE)  |   \
                        MT_ENC(MT_ATTR_DEV_GRE, MT_IDX_DEV_GRE) |   \
                        MT_ENC(MT_ATTR_MEM_NC,  MT_IDX_MEM_NC)  |   \
                        MT_ENC(MT_ATTR_MEM_WT,  MT_IDX_MEM_WT)  |   \
                        MT_ENC(MT_ATTR_MEM_WB,  MT_IDX_MEM_WB)

#ifndef __ASSEMBLER__

#include "types.hpp"

class Memtype
{
    private:
        static uint64 const mair = MAIR_VAL;

    public:
        enum class Index
        {
            DEV         = MT_IDX_DEV,
            DEV_E       = MT_IDX_DEV_E,
            DEV_RE      = MT_IDX_DEV_RE,
            DEV_GRE     = MT_IDX_DEV_GRE,
            MEM_NC      = MT_IDX_MEM_NC,
            MEM_WT      = MT_IDX_MEM_WT,
            MEM_WB      = MT_IDX_MEM_WB,
        };

        enum class Shareability
        {
            NONE        = 0,
            OUTER       = 2,
            INNER       = 3,
        };

        static unsigned npt_attr (Index idx)
        {
            unsigned attr = mair >> (static_cast<unsigned>(idx) << 3) & 0xff;
            return (attr >> 4 & 0xc) | (attr >> 2 & 0x3);
        }
};

#endif
