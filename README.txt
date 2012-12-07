This is a mass storage bootloader for the Stellaris Launchpad and other Stellaris chips.
Inspired by, and similar in use to, the bootloader found on some NXP LPC chips.
It appears as a regular external drive (formatted with FAT12) when plugged into a PC, no drivers or custom software needed!
It takes up 16kB of flash memory. 
Bootloader is entered when SW2 button is pressed during reset.
REMEMBER TO EDIT YOUR PROGRAM'S LINKER SCRIPT TO START THE CODE AT ADDRESS 0x4000

This software is still experimental, please report any issues!


INSTALLATION:

*Put the source in a dirctory inside stellarisware/boards/ek-lm4f120xl/
*Run make
*Flash gcc/boot_usb_msc.bin onto your Launchpad or other Stellaris board


HOW TO USE:

*Edit the linker script of the firmware you want to flash using the bootloader, so that the code starts at address 0x4000.
 usually it means you have to find this in projectname.ld:
	FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00040000
 and change it to:
	FLASH (rx) : ORIGIN = 0x00004000, LENGTH = 0x0003c000
 then rebuild your project
*Rename the resulting project binary to firmware.bin
*Plug in your board while holding down SW2, or press the reset button while holding SW2, the system should recognize it as a 512kB mass storage device
 The red LED will blink when the bootloader is running
*You can download the firmware.bin found on the drive to download the contents of flash memory
*You can upload your firmware to the board by copying your firmware.bin to the device and overwriting it
*Safely eject the drive and reset your board, it should jump to your code immediately


KNOWN ISSUES:

*On Ubuntu, automatic mounting of external media will mess up the bootloader, mount the drive manually.

Makefile, linker and startup code by Mauro Scomparin and others https://github.com/scompo/stellaris-launchpad-template-gcc
All other code is Copyright 2012 Andrzej Surowiec <emeryth@gmail.com>, released under MIT License.
http://emerythacks.blogspot.com/

 







