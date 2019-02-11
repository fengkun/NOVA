#
# Makefile
#
# Copyright (C) 2009-2011 Udo Steinberg <udo@hypervisor.org>
# Economic rights: Technische Universitaet Dresden (Germany)
#
# Copyright (C) 2012-2013 Udo Steinberg, Intel Corporation.
# Copyright (C) 2019 Udo Steinberg, BedRock Systems, Inc.
#
# This file is part of the NOVA microhypervisor.
#
# NOVA is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# NOVA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License version 2 for more details.
#

# The following file should be customized for your environment
-include Makefile.conf

# Defaults
ARCH	?= aarch64
COMP	?= gcc

# Tools
INSTALL	:= install
MKDIR	:= mkdir
SIZE	:= size
ifeq ($(COMP),gcc)
CC	:= $(PREFIX-$(ARCH))gcc
LD	:= $(PREFIX-$(ARCH))ld
OC	:= $(PREFIX-$(ARCH))objcopy
else
$(error $(COMP) is not a valid compiler type)
endif
H2E	:= $(H2E-$(ARCH))
H2B	:= $(H2B-$(ARCH))
RUN	:= $(RUN-$(ARCH))

# Directories
SRC_DIR	:= src/$(ARCH) src
INC_DIR	:= inc/$(ARCH) inc

# Files
SRC	:= hypervisor.ld $(sort $(notdir $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.S)))) $(sort $(notdir $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.cpp))))
OBJ	:= $(patsubst %.ld,$(BLD_DIR)/$(ARCH)-%.o, $(patsubst %.S,$(BLD_DIR)/$(ARCH)-%.o, $(patsubst %.cpp,$(BLD_DIR)/$(ARCH)-%.o, $(SRC))))
DEP	:= $(patsubst %.o,%.d, $(OBJ))
HYP	:= $(BLD_DIR)/$(ARCH)-hypervisor
ELF	:= $(HYP).elf
BIN	:= $(HYP).bin

# Messages
ifneq ($(findstring s,$(MAKEFLAGS)),)
message = @echo $(1) $(2)
endif

# Feature check
check = $(shell if $(CC) $(1) -c -xc++ /dev/null -o /dev/null >/dev/null 2>&1; then echo "$(1)"; fi)

# Version check
gitrv = $(shell (git rev-parse HEAD 2>/dev/null || echo 0) | cut -c1-7)

# Search path
VPATH	:= $(SRC_DIR)

# Optimization options
DFLAGS	:= -MP -MMD -pipe
OFLAGS	:= -Os
ifeq ($(ARCH),x86_64)
AFLAGS	:= -m64 -march=core2 $(or $(call check,-mno-sse -mpreferred-stack-boundary=3), $(call check,-mpreferred-stack-boundary=4)) -mcmodel=kernel -mno-red-zone
else ifeq ($(ARCH),x86_32)
AFLAGS	:= -m32 -march=i686 -mpreferred-stack-boundary=2 -mregparm=3
else
$(error $(ARCH) is not a valid architecture)
endif

# Preprocessor options
PFLAGS	:= $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INC_DIR))

# Language options
FFLAGS	:= $(or $(call check,-std=gnu++17), $(call check,-std=gnu++14), $(call check,-std=gnu++11))
FFLAGS	+= -fdata-sections -ffreestanding -ffunction-sections -fomit-frame-pointer -freg-struct-return -freorder-blocks -funit-at-a-time
FFLAGS	+= -fno-asynchronous-unwind-tables -fno-exceptions -fno-rtti
FFLAGS	+= $(call check,-fno-pie)
FFLAGS	+= $(call check,-fno-stack-protector)
FFLAGS	+= $(call check,-fvisibility-inlines-hidden)
FFLAGS	+= $(call check,-fdiagnostics-color=auto)

# Warning options
WFLAGS	:= -Wall -Wextra -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization -Wformat=2 -Wmissing-format-attribute -Wmissing-noreturn -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wwrite-strings
WFLAGS	+= -Wctor-dtor-privacy -Wno-non-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wsign-promo
WFLAGS	+= $(call check,-Wlogical-op)
WFLAGS	+= $(call check,-Wstrict-null-sentinel)
WFLAGS	+= $(call check,-Wstrict-overflow=5)
WFLAGS	+= $(call check,-Wsuggest-final-methods)
WFLAGS	+= $(call check,-Wsuggest-final-types)
WFLAGS	+= $(call check,-Wsuggest-override)
WFLAGS	+= $(call check,-Wvolatile-register-var)
WFLAGS	+= $(call check,-Wzero-as-null-pointer-constant)

ifeq ($(ARCH),aarch64)
WFLAGS	+= $(call check,-Wpedantic)
endif

# Compiler flags
SFLAGS	:= $(PFLAGS) $(DFLAGS) $(AFLAGS) $(FFLAGS)
CFLAGS	:= $(PFLAGS) $(DFLAGS) $(AFLAGS) $(FFLAGS) $(OFLAGS) $(WFLAGS)

# Linker flags
LFLAGS	:= --defsym=GIT_VER=0x$(call gitrv) --gc-sections --warn-common -static -n -T

# Rules
$(HYP):			$(OBJ)
			$(call message,LNK,$@)
			$(LD) $(LFLAGS) $^ -o $@

$(ELF):			$(HYP)
			$(call message,ELF,$@)
			$(H2E) $< $@

$(BIN):			$(HYP)
			$(call message,BIN,$@)
			$(H2B) $< $@

$(OBJ):			| $(BLD_DIR)

$(BLD_DIR):
			@$(MKDIR) -p $@

$(BLD_DIR)/$(ARCH)-%.o:	%.ld $(MAKEFILE_LIST)
			$(call message,PRE,$@)
			$(CC) $(SFLAGS) -xc++ -E -P -MT $@ $< -o $@

$(BLD_DIR)/$(ARCH)-%.o:	%.S $(MAKEFILE_LIST)
			$(call message,ASM,$@)
			$(CC) $(SFLAGS) -c $< -o $@

$(BLD_DIR)/$(ARCH)-%.o:	%.cpp $(MAKEFILE_LIST)
			$(call message,CMP,$@)
			$(CC) $(CFLAGS) -c $< -o $@

Makefile.conf:
			$(call message,CFG,$@)
			@cp $@.example $@

.PHONY:			run install clean

run:			$(ELF)
			$(RUN) $<

install:		$(ELF) $(BIN)
			$(call message,INS,$@)
			$(INSTALL) -m 644 $(ELF) $(BIN) $(INS_DIR)
			@$(SIZE) $<

clean:
			$(call message,CLN,$@)
			$(RM) $(DEP) $(OBJ) $(HYP) $(ELF) $(BIN)

# Include Dependencies
ifneq ($(MAKECMDGOALS),clean)
-include		$(DEP)
endif
