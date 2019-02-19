/*
 * Paging
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

#define PTE_P           (1 << 0)        // Present
#define PTE_nL          (1 << 1)        // Not Block
#define PTE_MT(X)       (X << 2)        // Memory Type
#define PTE_SH          (3 << 8)        // Inner Shareable
#define PTE_A           (1 << 10)       // Accessed
#define PTE_nG          (1 << 11)       // Not Global
