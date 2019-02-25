/*
 * Atomic Operations
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

class Atomic_base
{
    public:
        template <typename T>
        static T load (T &ptr) { return __atomic_load_n (&ptr, __ATOMIC_SEQ_CST); }

        template <typename T>
        static void store (T &ptr, T val) { __atomic_store_n (&ptr, val, __ATOMIC_SEQ_CST); }

        template <typename T>
        static bool cmp_swap (T &ptr, T &o, T n) { return __atomic_compare_exchange_n (&ptr, &o, n, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST); }

        template <typename T>
        static T exchange (T &ptr, T v) { return __atomic_exchange_n (&ptr, v, __ATOMIC_SEQ_CST); }

        template <typename T>
        static T add (T &ptr, T v) { return __atomic_add_fetch (&ptr, v, __ATOMIC_SEQ_CST); }

        template <typename T>
        static T sub (T &ptr, T v) { return __atomic_sub_fetch (&ptr, v, __ATOMIC_SEQ_CST); }

        template <typename T>
        static void set_mask (T &ptr, T v) { __atomic_or_fetch (&ptr, v, __ATOMIC_SEQ_CST); }

        template <typename T>
        static void clr_mask (T &ptr, T v) { __atomic_and_fetch (&ptr, ~v, __ATOMIC_SEQ_CST); }
};
