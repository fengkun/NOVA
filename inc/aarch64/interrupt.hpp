/*
 * Interrupt Handling
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

#include "event.hpp"
#include "intid.hpp"
#include "types.hpp"

class Sm;

class Interrupt : private Intid
{
    private:
        static Event::Selector handle_sgi (uint32, bool);
        static Event::Selector handle_ppi (uint32, bool);
        static Event::Selector handle_spi (uint32, bool);

    public:
        enum Sgi
        {
            RRQ,
            RKE,
        };

        Sm *    sm  { nullptr };
        uint16  cpu { 0xffff };
        bool    gst { false };
        bool    dir { false };

        static Interrupt int_table[SPI_NUM];

        static void init();

        static Event::Selector handler (bool);

        static void conf_sgi (unsigned, bool);
        static void conf_ppi (unsigned, bool, bool);
        static void conf_spi (unsigned, unsigned, bool, bool, bool);

        static void send_sgi (Sgi, unsigned);

        static void deactivate_spi (unsigned);
};
