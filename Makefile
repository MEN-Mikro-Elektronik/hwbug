#***************************  M a k e f i l e  *******************************
#
#         Author: Yi.Xu@men.de
#
#    Description: Makefile for hwbug and hwbug_cmd under linux
#
#-----------------------------------------------------------------------------
#   Copyright (c) 2018-2020, MEN Mikro Elektronik GmbH
#*****************************************************************************
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

BUILDDIR    = ./
SRCDIR      = ./

SOURCES     = $(filter-out $(WINFILE), $(wildcard *.c))
OBJECTS_    = hwbug.o linux.o change.o display.o history.o line_args.o strsave.o
OBJECTS     = $(patsubst %,$(BUILDDIR)%,$(OBJECTS_))
OBJ_        = hwbug_cmd_lin.o
OBJ         = $(BUILDDIR)hwbug_cmd_lin.o
WINFILE     = hwbug_cmd_win.c
LINFILE     = hwbug_cmd_lin.c
HEADER      = hwbug.h
SRC_        = hwbug.c 
SRC         = hwbug_cmd_lin.c 
TARGET_     = hwbug
TARGET      = hwbug_cmd

MAK_SWITCH  := -DLINUX
ifneq "$(shell uname -m)" "ppc"
MAK_SWITCH  += -DMAC_IO_MAPPED_EN
endif
CFLAGS      := -g -s $(MAK_SWITCH) 
LDFLAGS     := -static
INC_        := -I./ -I/usr/src/linux/include
INC         := -I/usr/src/linux/include
WARN_LEVEL  := -Wall -Wno-format

RM          = rm -rfv
CC          = gcc


# Build .o first
$(BUILDDIR)%.o : $(SRCDIR)%.c
	@echo [CC] $<
	$(CC) $(CFLAGS) -c $< -o $@

# Build hwbug 
$(TARGET_) : $(OBJECTS)
	@echo [INFO] Creating Binary [$(TARGET_)]
	$(CC) $(INC_) $(WARN_LEVEL) $(OBJECTS) -o $@ $(LDFLAGS)

# Build hwbug_cmd 
$(TARGET) : $(OBJ)
	@echo [CC] $<
	@echo [INFO] Creating Binary [$(TARGET)]
	$(CC) $(CFLAGS) $(INC) $(WARN_LEVEL) $(SRCDIR)$(SRC) -o $@ $(LDFLAGS)


all: $(TARGET_) $(TARGET) 

.DEFAULT_GOAL := all

.PHONY: all
.PHONY: clean
clean:
	@echo "[Cleaning]"
	$(RM) $(BUILDDIR)*o
	$(RM) $(TARGET_) $(TARGET)

