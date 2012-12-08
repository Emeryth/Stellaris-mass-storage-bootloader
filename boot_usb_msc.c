/* 
* Copyright 2012 Andrzej Surowiec
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*
* File:			boot_ucb_msc.c
* Author:		Andrzej Surowiec <emeryth@gmail.com>
* Version:		1.0
* Decription:	Mass Storage bootloader main source
*
*/

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/udma.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"

#include "usblib/usblib.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdmsc.h"

#include "usb_config.h"
#include "ramdisk.h"


tDMAControlTable uDMAControlTable[64] __attribute__ ((aligned(1024)));

unsigned long massStorageEventCallback(void* callback, unsigned long event, unsigned long messageParameters, void* messageData)
{
	switch(event)
	{
	case USBD_MSC_EVENT_WRITING:
		{
			break;
		}
	case USBD_MSC_EVENT_READING:
		{
			break;
		}
	case USBD_MSC_EVENT_IDLE:
		{	
			break;
		}
	default:
		{
			break;
		}
	}
	return(0);
}

void __attribute__((naked))
CallApplication(unsigned long ulStartAddr)
{
	//
	// Set the vector table to the beginning of the app in flash.
	//
	HWREG(NVIC_VTABLE) = ulStartAddr;

	//
	// Load the stack pointer from the application's vector table.
	//
	__asm("    ldr     r1, [r0]\n"
	"    mov     sp, r1");

	//
	// Load the initial PC from the application's vector table and branch to
	// the application's entry point.
	//
	__asm("    ldr     r0, [r0, #4]\n"
	"    bx      r0\n");
}

int
main(void)
{


	
	// Setup GPIO for the SW2 button
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	
	ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);	
	ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	
	//Check if button is pressed, otherwise jump to program
	if (ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)){
		//Not pressed	
		CallApplication(USER_PROGRAM_START);
	}
	
	//Button pressed, start up the bootloader
	
	// Enable lazy stacking for interrupt handlers.  This allows floating-point
	// instructions to be used within interrupt handlers, but at the expense of
	// extra stack usage.
	ROM_FPULazyStackingEnable();

	// Set the clocking.
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2);
	
	// Configure and enable uDMA
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
	SysCtlDelay(10);
	ROM_uDMAControlBaseSet(&uDMAControlTable[0]);
	ROM_uDMAEnable();
	
	

	// Configure the required pins for USB operation.
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	ROM_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);
	
	// Set the USB stack mode to Device mode with VBUS monitoring.
	USBStackModeSet(0, USB_MODE_DEVICE, 0);
	
	// Pass our device information to the USB library and place the device
	// on the bus.
	USBDMSCInit(0, (tUSBDMSCDevice*)&massStroageDevice);

#ifdef DEBUG
//In debug mode, the bootloader prints out reads and writes via UART	
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);
	
	UARTprintf("Starting bootloader.\n");
	
#endif

	while(1)
	{
		//Blinking the LED so the user knows everything is working
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
		SysCtlDelay(SysCtlClockGet() / 10 / 3);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
		SysCtlDelay(SysCtlClockGet() / 10 / 3);
	}
}
