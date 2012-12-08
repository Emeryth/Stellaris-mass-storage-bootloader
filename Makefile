# Copyright (c) 2012, Mauro Scomparin
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Mauro Scomparin nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY Mauro Scomparin ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Mauro Scomparin BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# File:			Makefile.
# Author:		Mauro Scomparin <http://scompoprojects.worpress.com>.
# Version:		1.0.0.
# Description:	Sample makefile.

#==============================================================================
#           Cross compiling toolchain / tools specifications
#==============================================================================

# Prefix for the arm-eabi-none toolchain.
# I'm using codesourcery g++ lite compilers available here:
# http://www.mentor.com/embedded-software/sourcery-tools/sourcery-codebench/editions/lite-edition/
PREFIX_ARM = arm-none-eabi

# Microcontroller properties.
PART=LM4F120H5QR
CPU=-mcpu=cortex-m4
FPU=-mfpu=fpv4-sp-d16 -mfloat-abi=softfp

# Stellarisware path
STELLARISWARE_PATH=../../../

# Program name definition for ARM GNU C compiler.
CC      = ${PREFIX_ARM}-gcc
# Program name definition for ARM GNU Linker.
LD      = ${PREFIX_ARM}-ld
# Program name definition for ARM GNU Object copy.
CP      = ${PREFIX_ARM}-objcopy
# Program name definition for ARM GNU Object dump.
OD      = ${PREFIX_ARM}-objdump

# Option arguments for C compiler.
CFLAGS=-mthumb ${CPU} ${FPU} -Os -ffunction-sections -fdata-sections -MD -std=c99 -Wall -pedantic -c -g
# Library stuff passed as flags!
CFLAGS+= -I ${STELLARISWARE_PATH} -DPART_$(PART) -c -DTARGET_IS_BLIZZARD_RA1 -Dgcc 

#Uncomment this to enable debug:
#CFLAGS+= -DDEBUG

# Flags for LD
LFLAGS  = --gc-sections

# Flags for objcopy
CPFLAGS = -Obinary

# flags for objectdump
ODFLAGS = -S

# I want to save the path to libgcc, libc.a and libm.a for linking.
# I can get them from the gcc frontend, using some options.
# See gcc documentation
LIB_GCC_PATH=${shell ${CC} ${CFLAGS} -print-libgcc-file-name}
LIBC_PATH=${shell ${CC} ${CFLAGS} -print-file-name=libc.a}
LIBM_PATH=${shell ${CC} ${CFLAGS} -print-file-name=libm.a}
#LIBC_PATH=/home/cypher/stellaris/newlib/arm-none-eabi/lib/thumb2/libc.a
#LIBM_PATH=/home/cypher/stellaris/newlib/arm-none-eabi/lib/thumb2/libm.a


# Uploader tool path.
# Set a relative or absolute path to the upload tool program.
# I used this project: https://github.com/utzig/lm4tools
FLASHER=lm4flash
# Flags for the uploader program.
FLASHER_FLAGS=

#==============================================================================
#                         Project properties
#==============================================================================

# Project name (W/O .c extension eg. "main")
PROJECT_NAME = boot_msc_usb
# Startup file name (W/O .c extension eg. "LM4F_startup")
STARTUP_FILE = LM4F_startup
# Linker file name
LINKER_FILE = LM4F.ld

SRC = $(wildcard *.c)
SRC += ../../../utils/uartstdio.c
OBJS = $(SRC:.c=.o)

#==============================================================================
#                      Rules to make the target
#==============================================================================

#make all rule
all: $(OBJS) ${PROJECT_NAME}.axf ${PROJECT_NAME}

%.o: %.c
	@echo
	@echo Compiling $<...
	$(CC) -c $(CFLAGS) ${<} -o ${@}

${PROJECT_NAME}.axf: $(OBJS)
	@echo
	@echo Making driverlib
	$(MAKE) -C ${STELLARISWARE_PATH}driverlib/
	@echo
	@echo Making usblib
	$(MAKE) -C ${STELLARISWARE_PATH}usblib/
	@echo
	@echo Linking...
	$(LD) -T $(LINKER_FILE) $(LFLAGS) -o ${PROJECT_NAME}.axf $(OBJS) ${STELLARISWARE_PATH}usblib/gcc-cm4f/libusb-cm4f.a ${STELLARISWARE_PATH}driverlib/gcc-cm4f/libdriver-cm4f.a $(LIBM_PATH) $(LIBC_PATH) $(LIB_GCC_PATH)

${PROJECT_NAME}: ${PROJECT_NAME}.axf
	@echo
	@echo Copying...
	$(CP) $(CPFLAGS) ${PROJECT_NAME}.axf ${PROJECT_NAME}.bin
	@echo
	@echo Creating list file...
	$(OD) $(ODFLAGS) ${PROJECT_NAME}.axf > ${PROJECT_NAME}.lst

# make clean rule
clean:
	rm *.bin *.o *.d *.axf *.lst

# Rule to load the project to the board
# I added a sudo because it's needed without a rule.
load:
	sudo ${FLASHER} ${PROJECT_NAME}.bin ${FLASHER_FLAGS}
