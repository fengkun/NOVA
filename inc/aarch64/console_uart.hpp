/*
 * UART Console
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

#include "console.hpp"
#include "hpt.hpp"
#include "lowlevel.hpp"
#include "memory.hpp"

template <typename U, typename R>
class Console_uart : private Console
{
    private:
        bool fini() override final
        {
            auto uart = static_cast<U *>(this);

            while (EXPECT_FALSE (uart->tx_busy()))
                pause();

            return true;
        }

        void outc (char c) override final
        {
            auto uart = static_cast<U *>(this);

            while (EXPECT_FALSE (uart->tx_full()))
                pause();

            uart->tx (c);
        }

    protected:
        static unsigned const baudrate = 115200;

        template <typename T> inline auto read  (R r) { return *reinterpret_cast<T volatile *>(DEV_GLOBL_UART + static_cast<mword>(r)); }
        template <typename T> inline void write (R r, T v)   { *reinterpret_cast<T volatile *>(DEV_GLOBL_UART + static_cast<mword>(r)) = v; }

        Console_uart (Hpt::OAddr phys)
        {
            if (!phys)
                return;

            Hptp::master.update (DEV_GLOBL_UART, phys, 0, Paging::Permissions (Paging::R | Paging::W | Paging::G), Memtype::Index::DEV, Memtype::Shareability::NONE);

            static_cast<U *>(this)->init();

            enable();
        }
};
