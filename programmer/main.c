/*
	Simple example to open a maximum of 4 devices - write some data then read it back.
	Shows one method of using list devices also.
	Assumes the devices have a loopback connector on them and they also have a serial number

	To build use the following gcc statement
	(assuming you have the static d2xx library in the /usr/local/lib directory
	and you have created a symbolic link to it in the current dir).
	gcc -o static_link main.c -lpthread libftd2xx.a
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "/home/pi/release/examples/ftd2xx.h"

int main()
{

FT_PROGRAM_DATA ftData = {
   0x00000000,         // Header - must be 0x00000000
   0xFFFFFFFF,         // Header - must be 0xffffffff
   0x00000002,         // Header - FT_PROGRAM_DATA version
   0x0403,            // VID
   0x6001,            // PID
   "FTDI",            // Manufacturer
   "FT",            // Manufacturer ID
   "USB HS Serial Converter",   // Description
   "FT000001",         // Serial Number
   44,            // MaxPower
   1,            // PnP
   0,            // SelfPowered
   1,            // RemoteWakeup
   1,            // non-zero if Rev4 chip, zero otherwise
   0,            // non-zero if in endpoint is isochronous
   0,            // non-zero if out endpoint is isochronous
   0,            // non-zero if pull down enabled
   1,            // non-zero if serial number to be used
   0,            // non-zero if chip uses USBVersion
   0x0110,            // BCD (0x0200 => USB2)
   //
   // FT2232C extensions (Enabled if Version = 1 or Version = 2)
   //
   0,            // non-zero if Rev5 chip, zero otherwise
   0,            // non-zero if in endpoint is isochronous
   0,            // non-zero if in endpoint is isochronous
   0,            // non-zero if out endpoint is isochronous
   0,            // non-zero if out endpoint is isochronous
   0,            // non-zero if pull down enabled
   0,            // non-zero if serial number to be used
   0,            // non-zero if chip uses USBVersion
   0x0,            // BCD (0x0200 => USB2)
   0,            // non-zero if interface is high current
   0,            // non-zero if interface is high current
   0,            // non-zero if interface is 245 FIFO
   0,            // non-zero if interface is 245 FIFO CPU target
   0,            // non-zero if interface is Fast serial
   0,            // non-zero if interface is to use VCP drivers
   0,            // non-zero if interface is 245 FIFO
   0,            // non-zero if interface is 245 FIFO CPU target
   0,            // non-zero if interface is Fast serial
   0,            // non-zero if interface is to use VCP drivers
   //
   // FT232R extensions (Enabled if Version = 2)
   //
   0,            // Use External Oscillator
   0,            // High Drive I/Os
   0,            // Endpoint size
   0,            // non-zero if pull down enabled
   0,            // non-zero if serial number to be used
   0,            // non-zero if invert TXD
   0,            // non-zero if invert RXD
   0,            // non-zero if invert RTS
   0,            // non-zero if invert CTS
   0,            // non-zero if invert DTR
   0,            // non-zero if invert DSR
   0,            // non-zero if invert DCD
   0,            // non-zero if invert RI
   0,            // Cbus Mux control
   0,            // Cbus Mux control
   0,            // Cbus Mux control
   0,            // Cbus Mux control
   0,            // Cbus Mux control
   0            // non-zero if using D2XX drivers
};

FT_HANDLE ftHandle;

FT_STATUS ftStatus = FT_Open(0, &ftHandle);
if (ftStatus == FT_OK) {
    ftStatus = FT_EE_Program(ftHandle, &ftData);
    if (ftStatus == FT_OK){
        printf("EE program success\n");
    }
    else {
        printf("EE program failed\n");
   }
} else {
	printf("couldn't find device\n");
}
return 0;
}
