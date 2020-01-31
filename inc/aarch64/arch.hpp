/*
 * Architecture Definitions
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

#include "suffix.hpp"

#define ARCH                "aarch64"
#define WORD                .quad
#define SIZE                8
#define PTE_BPL             9
#define IPA_BITS            39

#define DECR_STACK          sub      sp,  sp,      #16*19
#define INCR_STACK          add      sp,  sp,      #16*19

#define SAVE_STATE          stp      x0,  x1, [sp, #16* 0]  ;\
                            stp      x2,  x3, [sp, #16* 1]  ;\
                            stp      x4,  x5, [sp, #16* 2]  ;\
                            stp      x6,  x7, [sp, #16* 3]  ;\
                            stp      x8,  x9, [sp, #16* 4]  ;\
                            stp     x10, x11, [sp, #16* 5]  ;\
                            stp     x12, x13, [sp, #16* 6]  ;\
                            stp     x14, x15, [sp, #16* 7]  ;\
                            stp     x16, x17, [sp, #16* 8]  ;\
                            stp     x18, x19, [sp, #16* 9]  ;\
                            stp     x20, x21, [sp, #16*10]  ;\
                            stp     x22, x23, [sp, #16*11]  ;\
                            stp     x24, x25, [sp, #16*12]  ;\
                            stp     x26, x27, [sp, #16*13]  ;\
                            stp     x28, x29, [sp, #16*14]  ;\
                            mrs      x9,            sp_el0  ;\
                            mrs     x10,         tpidr_el0  ;\
                            mrs     x11,       tpidrro_el0  ;\
                            mrs     x12,           elr_el2  ;\
                            mrs     x13,          spsr_el2  ;\
                            mrs     x14,           esr_el2  ;\
                            mrs     x15,           far_el2  ;\
                            stp     x30,  x9, [sp, #16*15]  ;\
                            stp     x10, x11, [sp, #16*16]  ;\
                            stp     x12, x13, [sp, #16*17]  ;\
                            stp     x14, x15, [sp, #16*18]  ;

#define LOAD_STATE          ldp     x12, x13, [sp, #16*17]  ;\
                            ldp     x10, x11, [sp, #16*16]  ;\
                            ldp     x30,  x9, [sp, #16*15]  ;\
                            msr     spsr_el2, x13           ;\
                            msr      elr_el2, x12           ;\
                            msr  tpidrro_el0, x11           ;\
                            msr    tpidr_el0, x10           ;\
                            msr       sp_el0,  x9           ;\
                            ldp     x28, x29, [sp, #16*14]  ;\
                            ldp     x26, x27, [sp, #16*13]  ;\
                            ldp     x24, x25, [sp, #16*12]  ;\
                            ldp     x22, x23, [sp, #16*11]  ;\
                            ldp     x20, x21, [sp, #16*10]  ;\
                            ldp     x18, x19, [sp, #16* 9]  ;\
                            ldp     x16, x17, [sp, #16* 8]  ;\
                            ldp     x14, x15, [sp, #16* 7]  ;\
                            ldp     x12, x13, [sp, #16* 6]  ;\
                            ldp     x10, x11, [sp, #16* 5]  ;\
                            ldp      x8,  x9, [sp, #16* 4]  ;\
                            ldp      x6,  x7, [sp, #16* 3]  ;\
                            ldp      x4,  x5, [sp, #16* 2]  ;\
                            ldp      x2,  x3, [sp, #16* 1]  ;\
                            ldp      x0,  x1, [sp, #16* 0]  ;

#define BIT(B)               (SFX_U   (1) << (B))
#define BIT64(B)             (SFX_ULL (1) << (B))
#define BIT_RANGE(H,L)      ((SFX_U   (2) << (H)) - (SFX_U   (1) << (L)))
#define BIT64_RANGE(H,L)    ((SFX_ULL (2) << (H)) - (SFX_ULL (1) << (L)))

#define PSTATE_ALL_M        (BIT64_RANGE (3, 0))    // Mode
#define PSTATE_A64_SP       (BIT64  (0))            // Stack Pointer Select
#define PSTATE_A64_EL0      (SFX_ULL(0) << 2)       // EL0 (User)
#define PSTATE_A64_EL1      (SFX_ULL(1) << 2)       // EL1 (Kernel)
#define PSTATE_A64_EL2      (SFX_ULL(2) << 2)       // EL2 (Hypervisor)
#define PSTATE_A64_EL3      (SFX_ULL(3) << 2)       // EL3 (Monitor)
#define PSTATE_ALL_nRW      (BIT64  (4))            // Execution State
#define PSTATE_A32_T        (BIT64  (5))            // T32 State
#define PSTATE_A64_5        (BIT64  (5))
#define PSTATE_ALL_F        (BIT64  (6))            // Mask FIQ
#define PSTATE_ALL_I        (BIT64  (7))            // Mask IRQ
#define PSTATE_ALL_A        (BIT64  (8))            // Mask SError
#define PSTATE_A32_E        (BIT64  (9))            // Endianness
#define PSTATE_A64_D        (BIT64  (9))            // Mask Debug Exception
#define PSTATE_A32_IT_7_2   (BIT64_RANGE (15, 10))  // If-Then
#define PSTATE_A32_GE       (BIT64_RANGE (19, 16))  // Greater-Than or Equal
#define PSTATE_A64_BTYPE    (BIT64_RANGE (11, 10))  // Branch Type Indicator
#define PSTATE_A64_SSBS     (BIT64 (12))            // Speculative Store Bypass
#define PSTATE_A64_19_13    (BIT64_RANGE (19, 13))
#define PSTATE_ALL_IL       (BIT64 (20))            // Illegal Execution State
#define PSTATE_ALL_SS       (BIT64 (21))            // Software Step
#define PSTATE_ALL_PAN      (BIT64 (22))            // Privileged Access Never
#define PSTATE_A32_SSBS     (BIT64 (23))            // Speculative Store Bypass
#define PSTATE_A64_UAO      (BIT64 (23))            // User Access Override
#define PSTATE_ALL_DIT      (BIT64 (24))            // Data Independent Timing
#define PSTATE_A32_IT_1_0   (BIT64_RANGE (26, 25))  // If-Then
#define PSTATE_A32_Q        (BIT64 (27))            // Saturation
#define PSTATE_A64_TCO      (BIT64 (25))            // Tag Check Override
#define PSTATE_A64_27_26    (BIT64_RANGE (27, 26))
#define PSTATE_ALL_V        (BIT64 (28))            // Condition Overflow
#define PSTATE_ALL_C        (BIT64 (29))            // Condition Carry
#define PSTATE_ALL_Z        (BIT64 (30))            // Condition Zero
#define PSTATE_ALL_N        (BIT64 (31))            // Condition Negative
#define PSTATE_ALL_63_32    (BIT64_RANGE (63, 32))
#define PSTATE_A32_RES0     (PSTATE_ALL_63_32)
#define PSTATE_A64_RES0     (PSTATE_ALL_63_32 | PSTATE_A64_27_26 | PSTATE_A64_19_13 | PSTATE_A64_5)

#define SCTLR_ALL_M         (BIT64  (0))            // Enable MMU Stage 1
#define SCTLR_ALL_A         (BIT64  (1))            // Enable Alignment Check
#define SCTLR_ALL_C         (BIT64  (2))            // Enable D-Cache
#define SCTLR_A32_nTLSMD    (BIT64  (3))            // Don't trap Load/Store Multiple
#define SCTLR_A64_SA        (BIT64  (3))            // Enable Stack Alignment Check EL1/2
#define SCTLR_A32_LSMAOE    (BIT64  (4))            // Load/Store Multiple Atomicity/Ordering
#define SCTLR_A64_SA0       (BIT64  (4))            // Enable Stack Alignment Check EL0
#define SCTLR_ALL_CP15BEN   (BIT64  (5))            // Enable EL0 DMB, DSB, ISB (AArch32)
#define SCTLR_A32_6         (BIT64  (6))
#define SCTLR_A64_nAA       (BIT64  (6))            // Enable Non-Aligned Access
#define SCTLR_ALL_ITD       (BIT64  (7))            // Disable EL0 IT instructions (AArch32)
#define SCTLR_ALL_SED       (BIT64  (8))            // Disable EL0 SETEND (AArch32)
#define SCTLR_A32_9         (BIT64  (9))
#define SCTLR_A64_UMA       (BIT64  (9))            // User Mask Access
#define SCTLR_ALL_EnRCTX    (BIT64 (10))            // Enable EL0 RCTX instructions
#define SCTLR_A32_11        (BIT64 (11))
#define SCTLR_A64_EOS       (BIT64 (11))            // Exception Exit is Context-Synchronizing
#define SCTLR_ALL_I         (BIT64 (12))            // Enable I-Cache
#define SCTLR_A32_V         (BIT64 (13))            // Vectors
#define SCTLR_A64_EnDB      (BIT64 (13))            // Enable Pointer Authentication
#define SCTLR_A32_15_14     (BIT64_RANGE (15, 14))
#define SCTLR_A64_DZE       (BIT64 (14))            // Enable EL0 DC ZVA
#define SCTLR_A64_UCT       (BIT64 (15))            // Enable EL0 CTR_EL0
#define SCTLR_ALL_nTWI      (BIT64 (16))            // Don't trap WFI
#define SCTLR_ALL_17        (BIT64 (17))
#define SCTLR_ALL_nTWE      (BIT64 (18))            // Don't trap WFE
#define SCTLR_ALL_WXN       (BIT64 (19))            // Write implies XN
#define SCTLR_A32_UWXN      (BIT64 (20))            // Unprivileged write implies XN
#define SCTLR_A64_TSCXT     (BIT64 (20))            // Trap EL0 SXCTNUM
#define SCTLR_A32_21        (BIT64 (21))
#define SCTLR_A64_IESB      (BIT64 (21))            // Implicit Error Synchronization
#define SCTLR_A32_22        (BIT64 (22))
#define SCTLR_A64_EIS       (BIT64 (22))            // Exception Entry is Context-Synchronizing
#define SCTLR_ALL_SPAN      (BIT64 (23))            // Set Privilege Access Never
#define SCTLR_A32_24        (BIT64 (24))
#define SCTLR_A64_EOE       (BIT64 (24))            // Endianness of Data Accesses EL0
#define SCTLR_ALL_EE        (BIT64 (25))            // Endianness of Data Accesses EL1/EL2
#define SCTLR_A32_27_26     (BIT64_RANGE (27, 26))
#define SCTLR_A64_UCI       (BIT64 (26))            // Enable EL0 DC CVAU/CVAC/CVAP/CVADP/CIVAC, IC IVAU
#define SCTLR_A64_EnDA      (BIT64 (27))            // Enable Pointer Authentication
#define SCTLR_A32_TRE       (BIT64 (28))            // TEX Remap Enable
#define SCTLR_A64_nTLSMD    (BIT64 (28))            // Don't trap Load/Store Multiple
#define SCTLR_A32_AFE       (BIT64 (29))            // Access Flag Enable
#define SCTLR_A64_LSMAOE    (BIT64 (29))            // Load/Store Multiple Atomicity/Ordering
#define SCTLR_A32_TE        (BIT64 (30))            // T32 Exception Enable
#define SCTLR_A64_EnIB      (BIT64 (30))            // Enable Pointer Authentication
#define SCTLR_A32_DSSBS     (BIT64 (31))            // Default SSBS on Exception Entry
#define SCTLR_A64_EnIA      (BIT64 (31))            // Enable Pointer Authentication
#define SCTLR_A32_63_32     (BIT64_RANGE (63, 32))
#define SCTLR_A64_34_32     (BIT64_RANGE (34, 32))
#define SCTLR_A64_BT0       (BIT64 (35))            // PAC Branch Type Compatibility EL0
#define SCTLR_A64_BT1       (BIT64 (36))            // PAC Branch Type Compatibility EL1/EL2
#define SCTLR_A64_ITFSB     (BIT64 (37))            // Tag Check Synchronization
#define SCTLR_A64_TCF0      (BIT64_RANGE (39, 38))  // Tag Check Fail EL0
#define SCTLR_A64_TCF       (BIT64_RANGE (41, 40))  // Tag Check Fail EL1/EL2
#define SCTLR_A64_ATA0      (BIT64 (42))            // Allocation Tag Access EL0
#define SCTLR_A64_ATA       (BIT64 (43))            // Allocation Tag Access EL1/EL2
#define SCTLR_A64_DSSBS     (BIT64 (44))            // Default SSBS on Exception Entry
#define SCTLR_A64_TWEDEn    (BIT64 (45))            // TWE Delay Enable
#define SCTLR_A64_TWEDEL    (BIT64_RANGE (49, 46))  // TWE Delay
#define SCTLR_A64_TMT0      (BIT64 (50))            // Transactional Memory Trivial EL0
#define SCTLR_A64_TMT       (BIT64 (51))            // Transactional Memory Trivial EL1/EL2/EL3
#define SCTLR_A64_TME0      (BIT64 (52))            // Transactional Memory Enable EL0
#define SCTLR_A64_TME       (BIT64 (53))            // Transactional Memory Enable EL1/EL2/EL3
#define SCTLR_A64_63_54     (BIT64_RANGE (63, 54))
#define SCTLR_A32_RES0      (SCTLR_A32_63_32 | SCTLR_A32_27_26 | SCTLR_A32_24 | SCTLR_A32_21 | SCTLR_ALL_17 | SCTLR_A32_15_14 | SCTLR_A32_9 | SCTLR_A32_6)
#define SCTLR_A64_RES0      (SCTLR_A64_63_54 | SCTLR_A64_34_32 | SCTLR_ALL_17)
#define SCTLR_A32_RES1      (SCTLR_A32_22 | SCTLR_A32_11)
#define SCTLR_A64_RES1      (SFX_ULL(0))

#define TCR_A64_T0SZ(X)     ((64 - (X)) <<  0)
#define TCR_A32_3           (BIT64  (3))
#define TCR_A32_PD0         (BIT64  (4))
#define TCR_A32_PD1         (BIT64  (5))
#define TCR_A32_T2E         (BIT64  (6))            // TTBCR2 Enable
#define TCR_A64_6           (BIT64  (6))
#define TCR_ALL_EPD0        (BIT64  (7))            // TTBR0 Disable Translations
#define TCR_ALL_IRGN0_WB_WA (BIT64  (8))            // TTBR0 Inner Write-Back, Write-Allocate
#define TCR_ALL_ORGN0_WB_WA (BIT64 (10))            // TTBR0 Outer Write-Back, Write-Allocate
#define TCR_ALL_SH0_NS      (SFX_ULL(0) << 12)      // TTBR0 Non-Shareable
#define TCR_ALL_SH0_OS      (SFX_ULL(2) << 12)      // TTBR0 Outer Shareable
#define TCR_ALL_SH0_IS      (SFX_ULL(3) << 12)      // TTBR0 Inner Shareable
#define TCR_A32_15_14       (BIT64_RANGE (15, 14))
#define TCR_A64_TG0_4K      (SFX_ULL(0) << 14)      // TTBR0 Granule 4K
#define TCR_A64_T1SZ(X)     ((64 - (X)) << 16)
#define TCR_A32_21_19       (BIT64_RANGE (21, 19))
#define TCR_ALL_A1          (BIT64 (22))            // ASID Selector
#define TCR_ALL_EPD1        (BIT64 (23))            // TTBR1 Disable Translations
#define TCR_ALL_IRGN1_WB_WA (BIT64 (24))            // TTBR1 Inner Write-Back, Write-Allocate
#define TCR_ALL_ORGN1_WB_WA (BIT64 (26))            // TTBR1 Outer Write-Back, Write-Allocate
#define TCR_ALL_SH1_NS      (SFX_ULL(0) << 28)      // TTBR1 Non-Shareable
#define TCR_ALL_SH1_OS      (SFX_ULL(2) << 28)      // TTBR1 Outer Shareable
#define TCR_ALL_SH1_IS      (SFX_ULL(3) << 28)      // TTBR1 Inner Shareable
#define TCR_A64_TG1_4K      (SFX_ULL(2) << 30)      // TTBR1 Granule 4K
#define TCR_A32_EAE         (BIT64 (31))            // Extended Address Enable
#define TCR_A32_40_32       (BIT64_RANGE (40, 32))
#define TCR_A64_IPS         (SFX_ULL(7) << 32)      // Intermediate Physical Address Size
#define TCR_A64_35          (BIT64 (35))
#define TCR_A64_AS          (BIT64 (36))            // ASID Size
#define TCR_A64_TBI0        (BIT64 (37))            // TTBR0 Top Byte Ignored
#define TCR_A64_TBI1        (BIT64 (38))            // TTBR1 Top Byte Ignored
#define TCR_A64_HA          (BIT64 (39))            // Hardware Management Access Bit
#define TCR_A64_HD          (BIT64 (40))            // Hardware Management Dirty Bit
#define TCR_ALL_HPD0        (BIT64 (41))            // TTBR0 Hierarchical Permission Disable
#define TCR_ALL_HPD1        (BIT64 (42))            // TTBR1 Hierarchical Permission Disable
#define TCR_ALL_HWU059      (BIT64 (43))            // TTBR0 Hardware Use Bit59
#define TCR_ALL_HWU060      (BIT64 (44))            // TTBR0 Hardware Use Bit60
#define TCR_ALL_HWU061      (BIT64 (45))            // TTBR0 Hardware Use Bit61
#define TCR_ALL_HWU062      (BIT64 (46))            // TTBR0 Hardware Use Bit62
#define TCR_ALL_HWU159      (BIT64 (47))            // TTBR1 Hardware Use Bit59
#define TCR_ALL_HWU160      (BIT64 (48))            // TTBR1 Hardware Use Bit60
#define TCR_ALL_HWU161      (BIT64 (49))            // TTBR1 Hardware Use Bit61
#define TCR_ALL_HWU162      (BIT64 (50))            // TTBR1 Hardware Use Bit62
#define TCR_A32_63_51       (BIT64_RANGE (63, 51))
#define TCR_A64_TBID0       (BIT64 (51))            // TTBR0 Top Byte Data Access Only Matching
#define TCR_A64_TBID1       (BIT64 (52))            // TTBR1 Top Byte Data Access Only Matching
#define TCR_A64_NFD0        (BIT64 (53))            // TTBR0 Non-Fault Translation Table Walk Disable
#define TCR_A64_NFD1        (BIT64 (54))            // TTBR1 Non-Fault Translation Table Walk Disable
#define TCR_A64_E0PD0       (BIT64 (55))            // TTBR0 Fault Control Address Translation
#define TCR_A64_E0PD1       (BIT64 (56))            // TTBR1 Fault Control Address Translation
#define TCR_A64_TCMA0       (BIT64 (57))
#define TCR_A64_TCMA1       (BIT64 (58))
#define TCR_A64_63_59       (BIT64_RANGE (63, 59))
#define TCR_A32_RES0        (TCR_A32_63_51 | TCR_A32_40_32 | TCR_A32_21_19 | TCR_A32_15_14 | TCR_A32_3)
#define TCR_A64_RES0        (TCR_A64_63_59 | TCR_A64_35 | TCR_A64_6)

#define VTCR_T0SZ(X)        ((64 - (X)) << 0)
#define VTCR_SL0_L2         (SFX_ULL(0) << 6)
#define VTCR_SL0_L1         (SFX_ULL(1) << 6)
#define VTCR_SL0_L0         (SFX_ULL(2) << 6)
#define VTCR_SL0_L3         (SFX_ULL(3) << 6)
#define VTCR_VS             (BIT64 (19))            // VMID Size
#define VTCR_20             (BIT64 (20))
#define VTCR_HA             (BIT64 (21))            // Hardware Update A-Flag
#define VTCR_HD             (BIT64 (22))            // Hardware Update D-Flag
#define VTCR_24_23          (BIT64_RANGE (24, 23))
#define VTCR_HWU59          (BIT64 (25))            // Hardware Use
#define VTCR_HWU60          (BIT64 (26))            // Hardware Use
#define VTCR_HWU61          (BIT64 (27))            // Hardware Use
#define VTCR_HWU62          (BIT64 (28))            // Hardware Use
#define VTCR_NSW            (BIT64 (29))            // Non-Secure Stage 2 Translation Table Space
#define VTCR_NSA            (BIT64 (30))            // Non-Secure Stage 2 Output Address Space
#define VTCR_31             (BIT64 (31))
#define VTCR_63_32          (BIT64_RANGE (63, 32))
#define VTCR_RES0           (VTCR_63_32 | VTCR_24_23 | VTCR_20)
#define VTCR_RES1           (VTCR_31)

#define CPTR_7_0            (BIT64_RANGE (7, 0))
#define CPTR_TZ             (BIT64  (8))            // Trap SVE Functionality
#define CPTR_9              (BIT64  (9))
#define CPTR_TFP            (BIT64 (10))            // Trap SVE/SIMD/FP Functionality
#define CPTR_11             (BIT64 (11))
#define CPTR_13_12          (BIT64_RANGE (13, 12))
#define CPTR_19_14          (BIT64_RANGE (19, 14))
#define CPTR_TTA            (BIT64 (20))            // Trap Trace Register Access
#define CPTR_29_21          (BIT64_RANGE (29, 21))
#define CPTR_TAM            (BIT64 (30))            // Trap Activity Monitor Access
#define CPTR_TCPAC          (BIT64 (31))            // Trap CPACR
#define CPTR_63_32          (BIT64_RANGE (63, 32))
#define CPTR_RES0           (CPTR_63_32 | CPTR_29_21 | CPTR_19_14 | CPTR_11)
#define CPTR_RES1           (CPTR_13_12 | CPTR_9 | CPTR_7_0)

#define MDCR_HPMN           (BIT64_RANGE (4, 0))    // Accessible Event Counters
#define MDCR_TPMCR          (BIT64  (5))            // Trap PMCR Access
#define MDCR_TPM            (BIT64  (6))            // Trap Performance Monitor Access
#define MDCR_HPME           (BIT64  (7))            // Event Counters Enable
#define MDCR_TDE            (BIT64  (8))            // Trap Debug Exceptions
#define MDCR_TDA            (BIT64  (9))            // Trap Debug Access
#define MDCR_TDOSA          (BIT64 (10))            // Trap Debug OS-Related Register Access
#define MDCR_TDRA           (BIT64 (11))            // Trap Debug ROM Address Register Access
#define MDCR_E2PB           (BIT64_RANGE (13, 12))  // EL2 Profiling Buffer
#define MDCR_TPMS           (BIT64 (14))            // Trap Performance Monitor Sampling
#define MDCR_16_15          (BIT64_RANGE (16, 15))
#define MDCR_HPMD           (BIT64 (17))            // Guest Performance Monitor Disable
#define MDCR_18             (BIT64 (18))
#define MDCR_TTRF           (BIT64 (19))            // Trap Trace Filter Control
#define MDCR_22_20          (BIT64_RANGE (22, 20))
#define MDCR_HCCD           (BIT64 (23))            // Hypervisor Cycle Counter Disable
#define MDCR_E2TB           (BIT64_RANGE (25, 24))  // EL2 Trace Buffer
#define MDCR_HLP            (BIT64 (26))            // Hypervisor Long Event Counter Enable
#define MDCR_TDCC           (BIT64 (27))            // Trap Debug Comms Channel
#define MDCR_MTPME          (BIT64 (28))            // Multithreaded PMU Enable
#define MDCR_63_29          (BIT64_RANGE (63, 29))
#define MDCR_RES0           (MDCR_63_29 | MDCR_22_20 | MDCR_18 | MDCR_16_15)
#define MDCR_RES1           (SFX_ULL(0))

#define HCR_VM              (BIT64  (0))            // Enable MMU Stage 2
#define HCR_SWIO            (BIT64  (1))            // Set/Way Invalidation Override
#define HCR_PTW             (BIT64  (2))            // Protected Table Walk
#define HCR_FMO             (BIT64  (3))            // Physical FIQ Routing
#define HCR_IMO             (BIT64  (4))            // Physical IRQ Routing
#define HCR_AMO             (BIT64  (5))            // Physical SError Routing
#define HCR_VF              (BIT64  (6))            // Virtual FIQ
#define HCR_VI              (BIT64  (7))            // Virtual IRQ
#define HCR_VSE             (BIT64  (8))            // Virtual SError
#define HCR_FB              (BIT64  (9))            // Force Broadcast
#define HCR_BSU_NONE        (SFX_ULL(0) << 10)      // Barrier Shareability Upgrade
#define HCR_BSU_INNER       (SFX_ULL(1) << 10)      // Barrier Shareability Upgrade
#define HCR_BSU_OUTER       (SFX_ULL(2) << 10)      // Barrier Shareability Upgrade
#define HCR_BSU_SYSTEM      (SFX_ULL(3) << 10)      // Barrier Shareability Upgrade
#define HCR_DC              (BIT64 (12))            // Default Cacheability
#define HCR_TWI             (BIT64 (13))            // Trap WFI from EL0/EL1
#define HCR_TWE             (BIT64 (14))            // Trap WFE from EL0/EL1
#define HCR_TID0            (BIT64 (15))            // Trap ID Group 0
#define HCR_TID1            (BIT64 (16))            // Trap ID Group 1
#define HCR_TID2            (BIT64 (17))            // Trap ID Group 2
#define HCR_TID3            (BIT64 (18))            // Trap ID Group 3
#define HCR_TSC             (BIT64 (19))            // Trap SMC
#define HCR_TIDCP           (BIT64 (20))            // Trap Implementation-Defined
#define HCR_TACR            (BIT64 (21))            // Trap Auxiliary Control Registers
#define HCR_TSW             (BIT64 (22))            // Trap Set/Way Cache Instructions
#define HCR_TPCP            (BIT64 (23))            // Trap PoC or PoP Cache Instructions
#define HCR_TPU             (BIT64 (24))            // Trap PoU Cache Instructions
#define HCR_TTLB            (BIT64 (25))            // Trap TLB Maintenance Instructions
#define HCR_TVM             (BIT64 (26))            // Trap Writes of Virtual Memory Controls
#define HCR_TGE             (BIT64 (27))            // Trap General Exceptions
#define HCR_TDZ             (BIT64 (28))            // Trap DC ZVA
#define HCR_HCD             (BIT64 (29))            // Disable HVC
#define HCR_TRVM            (BIT64 (30))            // Trap Reads of Virtual Memory Controls
#define HCR_RW              (BIT64 (31))            // Execution State Lower EL
#define HCR_CD              (BIT64 (32))            // Disable Stage 2 Data Access Cacheability
#define HCR_ID              (BIT64 (33))            // Disable Stage 2 Inst Access Cacheability
#define HCR_E2H             (BIT64 (34))            // Enable EL2 Host
#define HCR_TLOR            (BIT64 (35))            // Trap LOR Registers
#define HCR_TERR            (BIT64 (36))            // Trap Error Record Accesses
#define HCR_TEA             (BIT64 (37))            // Trap External Aborts
#define HCR_MIOCNCE         (BIT64 (38))            // Trap Mismatched Inner/Outer Cacheable Non-Coherency
#define HCR_TME             (BIT64 (39))            // Transactional Memory Enable
#define HCR_APK             (BIT64 (40))            // Trap Key Values
#define HCR_API             (BIT64 (41))            // Trap Pointer Authentication Instructions
#define HCR_NV              (BIT64 (42))            // Nested Virtualization
#define HCR_NV1             (BIT64 (43))            // Nested Virtualization
#define HCR_AT              (BIT64 (44))            // Trap Address Translation
#define HCR_NV2             (BIT64 (45))            // Nested Virtualization
#define HCR_FWB             (BIT64 (46))            // Combined Cacheability
#define HCR_FIEN            (BIT64 (47))            // Fault Injection Enable
#define HCR_48              (BIT64 (48))
#define HCR_TID4            (BIT64 (49))            // Trap ID Group 4
#define HCR_TICAB           (BIT64 (50))            // Trap IC IALLUIS
#define HCR_AMVOFFEN        (BIT64 (51))            // Activity Monitor Virtual Offset Enable
#define HCR_TOCU            (BIT64 (52))            // Trap PoU Cache Instructions
#define HCR_ENSCXT          (BIT64 (53))            // Enable Access to SCXTNUM
#define HCR_TTLBIS          (BIT64 (54))            // Trap TLB Maintenance Inner Shareable
#define HCR_TTLBOS          (BIT64 (55))            // Trap TLB Maintenance Outer Shareable
#define HCR_ATA             (BIT64 (56))            // Allocation Tag Access
#define HCR_DCT             (BIT64 (57))            // Default Cacheability Tagging
#define HCR_TID5            (BIT64 (58))            // Trap ID Group 5
#define HCR_TWEDEn          (BIT64 (59))            // TWE Delay Enable
#define HCR_TWEDEL          (BIT64_RANGE (63, 60))  // TWE Delay
#define HCR_RES0            (HCR_48)

#define SCR_NS              (BIT64  (0))            // Non-Secure
#define SCR_IRQ             (BIT64  (1))            // Physical IRQ Routing
#define SCR_FIQ             (BIT64  (2))            // Physical FIQ Routing
#define SCR_EA              (BIT64  (3))            // External Abort and SError Routing
#define SCR_5_4             (BIT64_RANGE (5, 4))
#define SCR_6               (BIT64  (6))
#define SCR_SMD             (BIT64  (7))            // Disable SMC
#define SCR_HCE             (BIT64  (8))            // Enable HVC
#define SCR_SIF             (BIT64  (9))            // Secure Instruction Fetch
#define SCR_RW              (BIT64 (10))            // Execution State Lower EL
#define SCR_ST              (BIT64 (11))            // Trap Secure Timer
#define SCR_TWI             (BIT64 (12))            // Trap WFI from EL0/EL1/EL2
#define SCR_TWE             (BIT64 (13))            // Trap WFE from EL0/EL1/EL2
#define SCR_TLOR            (BIT64 (14))            // Trap LOR Registers
#define SCR_TERR            (BIT64 (15))            // Trap Error record Accesses
#define SCR_APK             (BIT64 (16))            // Trap Key Values
#define SCR_API             (BIT64 (17))            // Trap Pointer Authentication Instructions
#define SCR_EEL2            (BIT64 (18))            // Enable Secure EL2
#define SCR_EASE            (BIT64 (19))            // External Aborts to SError
#define SCR_NMEA            (BIT64 (20))            // Non-Maskable External Aborts
#define SCR_FIEN            (BIT64 (21))            // Fault Injection Enable
#define SCR_24_22           (BIT64_RANGE (24, 22))
#define SCR_ENSCXT          (BIT64 (25))            // Enable Access to SCXTNUM
#define SCR_ATA             (BIT64 (26))            // Allocation Tag Access
#define SCR_FGTEn           (BIT64 (27))            // FGT Enable
#define SCR_ECVEn           (BIT64 (28))            // ECV Enable
#define SCR_TWEDEn          (BIT64 (29))            // TWE Delay Enable
#define SCR_TWEDEL          (BIT64_RANGE (33, 30))  // TWE Delay
#define SCR_TME             (BIT64 (34))            // Transactional Memory Enable
#define SCR_AMVOFFEN        (BIT64 (35))            // Activity Monitor Virtual Offset Enable
#define SCR_63_36           (BIT64_RANGE (63, 36))
#define SCR_RES0            (SCR_63_36 | SCR_24_22 | SCR_6)
#define SCR_RES1            (SCR_5_4)
