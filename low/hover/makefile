TCHAIN = /home/snegovick/dev/arm/usr/bin
OPENOCD = /home/snegovick/dev/arm/openocd/openocd.r204/src/openocd
NAME   = demo2106_blink_ram

CC      = $(TCHAIN)/arm-elf-gcc
LD      = $(TCHAIN)/arm-elf-ld -v
AR      = $(TCHAIN)/arm-elf-ar
AS      = $(TCHAIN)/arm-elf-as
CP      = $(TCHAIN)/arm-elf-objcopy
OD	= $(TCHAIN)/arm-elf-objdump

ARM_SOURCE = main.c tr24a/tr24a.c
ARM_OBJS = $(ARM_SOURCE:.c=.o)

CFLAGS  = -I./ -O0 -fno-common -g -D LPC2103
#	 -fomit-frame-pointer \
#	 -mthumb-interwork \


AFLAGS  = -ahls -mapcs-32 -o crt.o
LFLAGS  =  -Map main.map -T2103_link_flash.cmd 
CPFLAGS = -O ihex
BPFLAGS = -O binary
ODFLAGS	= -x --syms

all: main.bin

main.bin : main.hex
	$(CP) main.elf -O binary main.bin

main.hex : main.elf
	$(CP) main.elf -O ihex main.hex

main.elf : $(ARM_OBJS) crt.o 2103_link_flash.cmd
	$(LD) $(LFLAGS) -o main.elf crt.o  $(ARM_OBJS) $(THUMB_OBJS)

$(ARM_OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

crt.o: crt.s
	@ echo ".assembling"
	$(AS) $(AFLAGS) crt.s > crt.lst

.PHONY: flash

flash:
	@ echo "FLASHING device"
	sudo $(OPENOCD) -f lpc2103_armusbocd_tiny.cfg 

clean:
	-rm crt.lst crt.o main.o main.elf main.hex main.map main.bin tr24a/tr24a.o
