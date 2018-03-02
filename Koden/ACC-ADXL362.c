#include <p18f45k22.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <timers.h>
#include <usart.h>
#include <spi.h>
#include <delays.h>
#include <adc.h>

#include "Header_h.h"

// -----------------------------------------------------------------------------
void ToggleACC(void)
{
	ACC_ENABLE = 1;
	Nop(); Nop(); Nop();
	ACC_ENABLE = 0;
}

// -----------------------------------------------------------------------------
long int DoReadInc(void)
{
	unsigned char nX, nY, nZ;
	long lReturn;

	FlagBits.bSPIbusy = 1;

	OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);	// Farten minskad för att försäkra sig om att skrivinstruktionen ska gå fram (0x03)
	
	ACC_ENABLE = 0;

	Nop(); Nop(); Nop();

	MyWriteSPI(AccRead);		// Läs från... 
	MyWriteSPI(0x08);			// ...adress 0x08 (X-axeln) 
	nX  = MyReadSPI();
	nY = MyReadSPI();
	nZ = MyReadSPI();

	ACC_ENABLE = 1;

	CloseSPI();
	
	lReturn = nZ;
	lReturn <<= 8;
	lReturn |= nY;
	lReturn <<= 8;
	lReturn |= nX;

	FlagBits.bSPIbusy = 0;

	return lReturn;
}

// -----------------------------------------------------------------------------
long int DoStartADXL362(void)
{
	unsigned char nDEVID_AD, nDEVID_MST, nPARTID, nREVID;
	long lReturn;

	FlagBits.bSPIbusy = 1;
	
	OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);	// Farten minskad för att försäkra sig om att skrivinstruktionen ska gå fram (0x03)

	ACC_ENABLE = 0; // CS dras låg

	// 
	MyWriteSPI(AccWrite);		// Skriv ... 
	MyWriteSPI(0x1F);			// ...från adress 0x1F

	MyWriteSPI(0x52);			// 0x1F 0x52 = 'R' = Soft Reset

	Delay(1);

	ToggleACC();

	Delay(1);

	MyWriteSPI(AccWrite);		// Skriv ... 
	MyWriteSPI(0x20);			// ...från adress 0x20 och framåt

//	MyWriteSPI(0xFA);			// 0x20 THRESH_ACT
	MyWriteSPI(0x7A);			// 0x20 THRESH_ACT
	MyWriteSPI(0x00);			// 0x21

	MyWriteSPI(0x01);			// 0x22 TIME_ACT

	MyWriteSPI(0x96);			// 0x23 THRESH_INACT
	MyWriteSPI(0x00);			// 0x24

	MyWriteSPI(0x00);			// 0x25
	MyWriteSPI(0x00);			// 0x26

	MyWriteSPI(0x3F);			// 0x27 ACTIVITY/INACTIVITY

	MyWriteSPI(0x00);			// 0x28
	MyWriteSPI(0x80);			// 0x29
	
	MyWriteSPI(0x90);			// 0x2A INTMAP1
	MyWriteSPI(0x00);			// 0x2B INTMAP2

	MyWriteSPI(0x50);			// 0x2C 10.x.1.0.000	±8g 12.5Hz
//	MyWriteSPI(0x50);			// 0x2C 01.x.1.0.000	±4g 12.5Hz
	MyWriteSPI(0x0A);			// 0x2D x.0.00.1.1.10	Autosleep Enabled
//	MyWriteSPI(0x02);			// 0x2D x.0.00.0.0.10	Autosleep Disabled

	ToggleACC();

	MyWriteSPI(AccRead);		// Läs från... 
	MyWriteSPI(0x00);			// ...adress 0x00 
	nDEVID_AD  = MyReadSPI();
	nDEVID_MST = MyReadSPI();
	nPARTID = MyReadSPI();
	nREVID = MyReadSPI();

	ACC_ENABLE = 1;

	CloseSPI();

	lReturn = nDEVID_AD;
	lReturn <<= 8;
	lReturn |= nDEVID_MST;
	lReturn <<= 8;
	lReturn |= nPARTID;
	lReturn <<= 8;
	lReturn |= nREVID;

	Nop();
	
	FlagBits.bSPIbusy = 0;

	return lReturn;
}