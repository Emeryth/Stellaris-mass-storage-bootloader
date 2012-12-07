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
* File:			usb_config.c
* Author:		Andrzej Surowiec <emeryth@gmail.com>
* Version:		1.0
* Decription:	USB mass storage structures
*
*/

#include "inc/hw_types.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdmsc.h"
#include "usb_config.h"
#include "ramdisk.h"


const unsigned char languageDescriptor[] ={
	4, //descriptor length
	0x03, //descriptor type - String
	0x09,0x04 //supported language code English - United States
};


const unsigned char manufacturerString[] ={
	(17 + 1) * 2,
    0x03,
    'T', 0, 'e', 0, 'x', 0, 'a', 0, 's', 0, ' ', 0, 'I', 0, 'n', 0, 's', 0,
    't', 0, 'r', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0, 's', 0
};

const unsigned char productString[] ={
	(33+1)*2,
    0x03,
    'S', 0, 't', 0, 'e', 0, 'l', 0, 'l', 0, 'a', 0, 'r', 0, 'i', 0, 's', 0,
    ' ', 0, 'M', 0, 'a', 0, 's', 0, 's', 0, ' ', 0, 'S', 0, 't', 0, 'o', 0,
	'r', 0, 'a', 0, 'g', 0, 'e', 0, ' ', 0, 'B', 0, 'o', 0, 'o', 0, 't', 0,
	'l', 0, 'o', 0, 'a', 0, 'd', 0, 'e', 0, 'r', 0
};


const unsigned char serialNumber[] ={
	4,
    0x03,
    '0', 0
};


const unsigned char * const stringDescriptors[] =
{
    languageDescriptor,
    manufacturerString,
    productString,
    serialNumber,
    productString,
    productString
};


tMSCInstance massStorageInstance;

const tUSBDMSCDevice massStroageDevice =
{
    USB_VID_STELLARIS,
    USB_PID_MSC,
    "TI      ",
    "MSC Bootloader  ",
    "1.00",
    500,
    USB_CONF_ATTR_SELF_PWR,
    stringDescriptors,
    6,
    {
        massStorageOpen,
        massStorageClose,
        massStorageRead,
        massStorageWrite,
        massStorageNumBlocks
    },
    massStorageEventCallback,
    &massStorageInstance
};