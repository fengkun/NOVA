/*
 * System Memory Management Unit (SMMUv2)
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

#include "memory.hpp"
#include "space.hpp"
#include "spinlock.hpp"
#include "types.hpp"

class Pd;

class Smmu
{
    private:
        static Spinlock lock;
        static unsigned page_size;
        static unsigned pa_size;

        static mword gr0_base;      // Global Register Space 0
        static mword gr1_base;      // Global Register Space 1
        static mword ctx_base;      // Translation Context Bank Space

        enum class GR0_Register32
        {
            CR0         = 0x000,    // rw Configuration Register 0
            CR1         = 0x004,    // rw Configuration Register 1
            CR2         = 0x008,    // rw Configuration Register 2
            ACR         = 0x010,    // rw Auxiliary Configuration Register
            IDR0        = 0x020,    // r- Identification Register 0
            IDR1        = 0x024,    // r- Identification Register 1
            IDR2        = 0x028,    // r- Identification Register 2
            IDR3        = 0x02c,    // r- Identification Register 3
            IDR4        = 0x030,    // r- Identification Register 4
            IDR5        = 0x034,    // r- Identification Register 5
            IDR6        = 0x038,    // r- Identification Register 6
            IDR7        = 0x03c,    // r- Identification Register 7
            GFSR        = 0x048,    // rw Global Fault Status Register
            GFSRRESTORE = 0x04c,    // -w Global Fault Status Restore Register
            GFSYNR0     = 0x050,    // rw Global Fault Syndrom Register 0
            GFSYNR1     = 0x054,    // rw Global Fault Syndrom Register 1
            GFSYNR2     = 0x058,    // rw Global Fault Syndrom Register 2
            TLBIALL     = 0x060,    // -w TLB Invalidate All
            TLBIVMID    = 0x064,    // -w TLB Invalidate by VMID
            TLBIALLNSNH = 0x068,    // -w TLB Invalidate All, Non-Secure, Non-Hyp
            TLBIALLH    = 0x06c,    // -w TLB Invalidate All, Hyp
            TLBGSYNC    = 0x070,    // -w Global Synchronize TLB Invalidate
            TLBGSTATUS  = 0x074,    // r- Global TLB Status Register
            TLBIVAH     = 0x078,    // -w TLB Invalidate by VA, Hyp
        };

        enum class GR0_Register64
        {
            GFAR        = 0x040,    // rw Global Fault Address Register
        };

        enum class GR0_Array32
        {
            SMR         = 0x800,    // rw Stream Match Register
            S2CR        = 0xc00,    // rw Stream-to-Context Register
        };

        enum class GR1_Array32
        {
            CBAR        = 0x000,    // rw Context Bank Attribute Register
            CBFRSYNRA   = 0x400,    // rw Context Bank Fault Restricted Syndrome Register A
            CBA2R       = 0x800,    // rw Context Bank Attribute Register
        };

        enum class Ctx_Array32
        {
            SCTLR       = 0x000,    // s1 s2 rw System Control Register
            ACTLR       = 0x004,    // s1 s2 rw Auxiliary Control Register
            RESUME      = 0x008,    // s1 s2 -w Transaction Resume Register
            TCR2        = 0x010,    // s1 -- rw Translation Control Register 2
            TCR         = 0x030,    // s1 s2 rw Translation Control Register
            CONTEXTIDR  = 0x034,    // s1 -- rw Context Identification Register
            MAIR0       = 0x038,    // s1 -- rw Memory Attribute Indirection Register 0
            MAIR1       = 0x03c,    // s1 -- rw Memory Attribute Indirection Register 1
            FSR         = 0x058,    // s1 s2 rw Fault Status Register
            FSRRESTORE  = 0x05c,    // s1 s2 -w Fault Status Restore Register
            FSYNR0      = 0x068,    // s1 s2 rw Fault Syndrome Register 0
            FSYNR1      = 0x06c,    // s1 s2 rw Fault Syndrome Register 1
            TLBIASID    = 0x610,    // s1 -- -w TLB Invalidate by ASID
            TLBIALL     = 0x618,    // s1 -- -w TLB Invalidate All
            TLBSYNC     = 0x7f0,    // s1 s2 -w TLB Synchronize Invalidate
            TLBSTATUS   = 0x7f4,    // s1 s2 r- TLB Status
            ATSR        = 0x8f0,    // s1 -- r- Address Translation Status Register
        };

        enum class Ctx_Array64
        {
            TTBR0       = 0x020,    // s1 s2 rw Translation Table Base Register 0
            TTBR1       = 0x028,    // s1 -- rw Translation Table Base Register 1
            PAR         = 0x050,    // s1 -- rw Physical Address Register
            FAR         = 0x060,    // s1 s2 rw Fault Address Register
            IPAFAR      = 0x070,    // -- s2 rw IPA Fault Address Register
            TLBIVA      = 0x600,    // s1 -- -w TLB Invalidate by VA
            TLBIVAA     = 0x608,    // s1 -- -w TLB Invalidate by VA, All ASIDs
            TLBIVAL     = 0x620,    // s1 -- -w TLB Invalidate by VA, Last Level
            TLBIVAAL    = 0x628,    // s1 -- -w TLB Invalidate by VA, All ASIDs, Last Level
            TLBIIPAS2   = 0x630,    // -- s2 -w TLB Invalidate by IPA
            TLBIIPAS2L  = 0x638,    // -- s2 -w TLB Invalidate by IPA, Last Level
            ATS1PR      = 0x800,    // s1 -- -w Address Translation Stage 1, Privileged Read
            ATS1PW      = 0x808,    // s1 -- -w Address Translation Stage 1, Privileged Write
            ATS1UR      = 0x810,    // s1 -- -w Address Translation Stage 1, Unprivileged Read
            ATS1UW      = 0x818,    // s1 -- -w Address Translation Stage 1, Unprivileged Write
        };

        static inline auto read  (GR0_Register32 r)               { return *reinterpret_cast<uint32 volatile *>(gr0_base                         + static_cast<mword>(r)); }
        static inline auto read  (GR0_Register64 r)               { return *reinterpret_cast<uint64 volatile *>(gr0_base                         + static_cast<mword>(r)); }
        static inline auto read  (unsigned smg, GR0_Array32 r)    { return *reinterpret_cast<uint32 volatile *>(gr0_base + smg * sizeof (uint32) + static_cast<mword>(r)); }
        static inline auto read  (unsigned ctx, GR1_Array32 r)    { return *reinterpret_cast<uint32 volatile *>(gr1_base + ctx * sizeof (uint32) + static_cast<mword>(r)); }
        static inline auto read  (unsigned ctx, Ctx_Array32 r)    { return *reinterpret_cast<uint32 volatile *>(ctx_base + ctx * page_size       + static_cast<mword>(r)); }
        static inline auto read  (unsigned ctx, Ctx_Array64 r)    { return *reinterpret_cast<uint64 volatile *>(ctx_base + ctx * page_size       + static_cast<mword>(r)); }

        static inline void write (GR0_Register32 r,            uint32 v) { *reinterpret_cast<uint32 volatile *>(gr0_base                         + static_cast<mword>(r)) = v; }
        static inline void write (GR0_Register64 r,            uint64 v) { *reinterpret_cast<uint64 volatile *>(gr0_base                         + static_cast<mword>(r)) = v; }
        static inline void write (unsigned smg, GR0_Array32 r, uint32 v) { *reinterpret_cast<uint32 volatile *>(gr0_base + smg * sizeof (uint32) + static_cast<mword>(r)) = v; }
        static inline void write (unsigned ctx, GR1_Array32 r, uint32 v) { *reinterpret_cast<uint32 volatile *>(gr1_base + ctx * sizeof (uint32) + static_cast<mword>(r)) = v; }
        static inline void write (unsigned ctx, Ctx_Array32 r, uint32 v) { *reinterpret_cast<uint32 volatile *>(ctx_base + ctx * page_size       + static_cast<mword>(r)) = v; }
        static inline void write (unsigned ctx, Ctx_Array64 r, uint64 v) { *reinterpret_cast<uint64 volatile *>(ctx_base + ctx * page_size       + static_cast<mword>(r)) = v; }

    public:
        static unsigned sidb;       // Stream ID Bits
        static unsigned smrg;       // Stream Mapping Register Groups
        static unsigned ctxb;       // Context Banks

        static void init();

        static bool configure (unsigned, unsigned, unsigned, Pd *, Space::Index);

        static void interrupt();
};
