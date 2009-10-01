# ***************************************************************
# *     Makefile for Atmel AT91SAM7S256 - flash execution       *
# *                                                             *
# *                                                             *
# * James P Lynch  September 3, 2006                            *
# ***************************************************************

NAME   = demo_at91sam7_h256_blink_flash
TOOLCHAIN_PREFIX=/home/snegovick/dev/arm/usr/bin/arm-elf

# variables 
CC      = $(TOOLCHAIN_PREFIX)-gcc
LD      = $(TOOLCHAIN_PREFIX)-ld -v
AR      = $(TOOLCHAIN_PREFIX)-ar
AS      = $(TOOLCHAIN_PREFIX)-as
CP      = $(TOOLCHAIN_PREFIX)-objcopy
OD	= $(TOOLCHAIN_PREFIX)-objdump

CFLAGS  = -I./ -c -fno-common -O0 -g
AFLAGS  = -ahls -mapcs-32 -o crt.o
LFLAGS  =  -Map main.map -Tdemo_at91sam7_h256_blink_flash.cmd
CPFLAGS = --output-target=binary
ODFLAGS	= -x --syms

OBJECTS = crt.o	main.o timerisr.o timersetup.o isrsupport.o lowlevelinit.o blinker.o


# make target called by Eclipse (Project -> Clean ...)
         
#make target called by Eclipse  (Project -> Build Project)
all:  main.out
	@ echo "...copying"
	$(CP) $(CPFLAGS) main.out main.bin
	$(OD) $(ODFLAGS) main.out > main.dmp

main.out: $(OBJECTS)
	@ echo "..linking"
	$(LD) $(LFLAGS) -o main.out $(OBJECTS) libgcc.a

crt.o: crt.s
	@ echo ".assembling crt.s"
	$(AS) $(AFLAGS) crt.s > crt.lst

main.o: main.c
	@ echo ".compiling main.c"
	$(CC) $(CFLAGS) main.c

system.o: system.c
	@ echo ".compiling system.c"
	$(CC) $(CFLAGS) system.c

Cstartup_SAM7.o: Cstartup_SAM7.c
	@ echo ".compiling system.c"
	$(CC) $(CFLAGS) Cstartup_SAM7.c
	
timerisr.o: timerisr.c
	@ echo ".compiling timerisr.c"
	$(CC) $(CFLAGS) timerisr.c
	
lowlevelinit.o: lowlevelinit.c
	@ echo ".compiling lowlevelinit.c"
	$(CC) $(CFLAGS) lowlevelinit.c
	
timersetup.o: timersetup.c
	@ echo ".compiling timersetup.c"
	$(CC) $(CFLAGS) timersetup.c

isrsupport.o: isrsupport.c
	@ echo ".compiling isrsupport.c"
	$(CC) $(CFLAGS) isrsupport.c

blinker.o: blinker.c
	@ echo ".compiling blinker.c"
	$(CC) $(CFLAGS) blinker.c


clean:
	-rm $(OBJECTS) crt.lst main.lst main.out main.bin main.hex main.map main.dmp
	


# **********************************************************************************************
#                            FLASH PROGRAMMING      (using OpenOCD and Amontec JTAGKey)
#
# Alternate make target for flash programming only
#
# You must create a special Eclipse make target (program) to run this part of the makefile 
# (Project -> Create Make Target...  then set the Target Name and Make Target to "program")
#
# OpenOCD is run in "batch" mode with a special configuration file and a script file containing
# the flash commands. When flash programming completes, OpenOCD terminates.
#
# Note that the make file below creates the script file of flash commands "on the fly"
#
# Programmers: Martin Thomas, Joseph M Dupre, James P Lynch
# **********************************************************************************************

# specify output filename here (must be *.bin file)
TARGET = main.bin

# specify the directory where openocd executable resides (openocd-ftd2xx.exe or openocd-pp.exe)
OPENOCD_DIR = 'c:\Program Files\openocd-2006re93\bin\'

# specify OpenOCD executable (pp is for the wiggler, ftd2xx is for the USB debugger)
#OPENOCD = $(OPENOCD_DIR)openocd-pp.exe
OPENOCD = $(OPENOCD_DIR)openocd-ftd2xx.exe

# specify OpenOCD configuration file (pick the one for your device)
#OPENOCD_CFG = $(OPENOCD_DIR)at91sam7s256-wiggler-flash-program.cfg
#OPENOCD_CFG = $(OPENOCD_DIR)at91sam7s256-jtagkey-flash-program.cfg
OPENOCD_CFG = $(OPENOCD_DIR)at91sam7s256-armusbocd-flash-program.cfg

# specify the name and folder of the flash programming script file

# program the AT91SAM7S256 internal flash memory
program: $(TARGET)
	@echo "Preparing OpenOCD script..."
	@echo "Flash Programming with OpenOCD..."
	./openocd_go_flash.sh
	@echo "Flash Programming Finished."
