/*
 * retarget_uart.cpp
 *
 *  Created on: 28.9.2021
 *      Author: keijo
 */

#include "LpcDebugUart.h"

static LpcDebugUart *dbgu;

void retarget_init()
{
	//LpcPinMap none = {-1, -1}; // unused pin has negative values in it
	//Sadly, it seems that only USART0 is redirected to USB.
	//It means that those are pins PIO0_18 and PIO0_19
	LpcPinMap txpin = { 0, 19 }; // transmit pin that goes to debugger's UART->USB converter
	LpcPinMap rxpin = { 0, 18 }; // receive pin that goes to debugger's UART->USB converter
	LpcDebugUartConfig cfg = { LPC_USART0, 115200, (UART0_LCR_WLEN8 | UART0_LCR_SBS_1BIT), txpin, rxpin };
	dbgu = new LpcDebugUart(cfg);
}

extern "C" {

// ******************************************************************
// Redlib C Library function : __sys_write
// Newlib C library function : _write
//
// Function called by bottom level of printf routine within C library.
// With the default semihosting stub, this would write the
// character(s) to the debugger console window (which acts as
// stdout). But this version writes the character(s) to UART
// ******************************************************************
#if defined (__REDLIB__)
int __sys_write(int iFileHandle, char *pcBuffer, int iLength) {
#elif defined (__NEWLIB__)
int _write(int iFileHandle, char *pcBuffer, int iLength) {
#endif
	if(dbgu) {
		int len = iLength;
		while(len > 0) {
			if(*pcBuffer == '\n') { // send a CR before every LF
				while(dbgu->write('\r')==0);
			}
			while(dbgu->write(*pcBuffer)==0);
			++pcBuffer;
			--len;
		}
	}
	// Function returns number of bytes written
	return iLength;
}

#if defined (__REDLIB__)
// ******************************************************************
// Redlib C Library function : __sys_readc
//
// Called by bottom level of scanf routine within RedLib C library
// to read a character. With the default semihosting stub, this
// would read the character from the debugger console window (which
// acts as stdin). But this version reads the character from UART
// ******************************************************************

int __sys_readc(void) {
	char c;
	if(dbgu && dbgu->read(c)) return c;
	else return -1;

}
// #endif REDLIB __sys_readc()

#elif defined (__NEWLIB__)
// ******************************************************************
// Function _read
//
// Called by bottom level of scanf routine within Newlib C library
// to read multiple characters. With the default semihosting stub, this
// would read  characters from the debugger console window (which
// acts as stdin). But this version reads the characters from UART
// ******************************************************************
int _read(int iFileHandle, char *pcBuffer, int iLength) {

	// TODO : Should potentially check that iFileHandle == 0 to confirm
	// that read is from stdin
	if(!dbgu) return 0;
	return dbgu->read(pcBuffer, iLength);
}

#endif // NEWLIB _read()

}
