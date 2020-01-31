/*
 * Generic Timer
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

#include "compiler.hpp"
#include "types.hpp"

class Timer
{
    private:
        static uint32 freq CPULOCAL;

        enum Control : uint64
        {
            ENABLE      = 1ULL << 0,    // Timer enabled
            IMASK       = 1ULL << 1,    // Timer interrupt is masked
            ISTATUS     = 1ULL << 2,    // Timer condition is met
        };

        static uint32 frequency()
        {
            uint64 val;
            asm volatile ("isb; mrs %0, cntfrq_el0" : "=r" (val) : : "memory");
            return static_cast<uint32>(val);
        }

        static inline void set_access (uint64 val)
        {
            asm volatile ("msr cnthctl_el2, %0" : : "r" (val));
        }

        static inline void set_control (Control val)
        {
            asm volatile ("msr cnthp_ctl_el2, %0" : : "r" (val));
        }

    public:
        ALWAYS_INLINE
        static inline uint64 time()
        {
            uint64 val;
            asm volatile ("isb; mrs %0, cntpct_el0" : "=r" (val) : : "memory");
            return val;
        }

        ALWAYS_INLINE
        static inline void set_dln (uint64 val)
        {
            asm volatile ("msr cnthp_cval_el2, %0; isb" : : "r" (val));
        }

        ALWAYS_INLINE
        static inline uint64 ticks_to_ms (uint64 ticks)
        {
            return ticks * 1000 / freq;
        }

        ALWAYS_INLINE
        static inline uint64 ms_to_ticks (unsigned ms)
        {
            return static_cast<uint64>(ms) * freq / 1000;
        }

        static void interrupt();
        static void init();
};
