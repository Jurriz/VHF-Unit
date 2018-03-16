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

	OpenSPI(SPI_FOSC_16, MODE_11, SMPMID);	// Farten minskad för att försäkra sig om att skrivinstruktionen ska gå fram (0x03)
	
	ACC_ENABLE = 0;

	Nop(); Nop(); Nop();

	MyWriteSPI(0x8F);		// Läs från... 
	//MyWriteSPI(0x08);			// ...adress 0x08 (X-axeln) 
	nX = MyReadSPI();
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
char DoStartST_ACC(void)
{
	unsigned char lReturn;
	//long lReturn;

	FlagBits.bSPIbusy = 1;
	
    ACC_POW_VDD_IO = 1;   Delay(1);     //  Output POWER TO VDD_IO
    ACC_POW_VDD = 1;                    //  Output POWER TO VDD
    Delay(1000);            // Viktigt att accelerometern hinner stara upp ordentligt
	OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);	// Farten minskad för att försäkra sig om att skrivinstruktionen ska gå fram (0x03)
	//ACC_ENABLE = 0; // CS dras låg
    
    //Delay(1);
	
	//MyWriteSPI(0x22);		// Skriv till 0x22 REG8
	//MyWriteSPI(0x81);		// Software Reset och Reboot
    //Delay(1);
    //ACC_ENABLE = 1;
	//Delay(300); // Viktigt att ha denna tillräckligt stor så att kretsen hinner starta om helt
	ACC_ENABLE = 0; 
    
    MyWriteSPI(0x20);         // Skriv till CTRL_REG6_XL..  
    MyWriteSPI(0x98);         // 1001 1000 vilket ger accelerometern hastigheten 238 Hz
    
    ToggleACC();
    
	MyWriteSPI(0x8F);               // Läs från WHO_AM_I 
	lReturn = MyReadSPI();			// Data skickas till lReturn
    
	ACC_ENABLE = 1;

	CloseSPI();

	Nop();
	
	FlagBits.bSPIbusy = 0;

	return lReturn;
}
// AccWrite = 0x0A, AccRead = 0x0B, Fifo = 0x0D;

// ADXL362
// CKP = 0
// CKE = 0

// Enligt Microchip gäller följande:
//           CKP  CKE
// MODE_00    0    1
//      01    0    0
//      10    1    1
//      11    1    0
//
// SMP
//    SPI Master mode:
//        1 = Input data sampled at end of data output time
//        0 = Input data sampled at middle of data output time
//    SPI Slave mode:
//        SMP must be cleared when SPI is used in Slave mode

// CKP: Clock Polarity Select bit
// In SPI mode:
// 1 = Idle state for clock is a high level
// 0 = Idle state for clock is a low level

// CKE: SPI Clock Edge Select bit (SPI mode only)
// In SPI Master or Slave mode:
// 1 = Transmit occurs on transition from active to Idle clock state
// 0 = Transmit occurs on transition from Idle to active clock state

// SMP: SPI Data Input Sample bit
// SPI Master mode:
// 1 = Input data sampled at end of data output time
// 0 = Input data sampled at middle of data output time

// SPI1CON1bits.SMP = 0; // Input data sampled at middle of data output time
// SPI1CON1bits.CKP = 1; // CKP and CKE is subject to change ...
// SPI1CON1bits.CKE = 0; // ... based on your communication mode.

// #define   MODE_00       0b00000000   // Setting for SPI bus Mode 0,0
// CKE    1      0x40                   // SSPSTAT register
// CKP    0      0x00                   // SSPCON1 register

// #define   MODE_01       0b00000001   // Setting for SPI bus Mode 0,1
// CKE    0      0x00                   // SSPSTAT register
// CKP    0      0x00                   // SSPCON1 register

// #define   MODE_10       0b00000010   // Setting for SPI bus Mode 1,0
// CKE    1      0x40                   // SSPSTAT register
// CKP    1      0x10                   // SSPCON1 register

// #define   MODE_11       0b00000011   // Setting for SPI bus Mode 1,1
// CKE    0      0x00                   // SSPSTAT register
// CKP    1      0x10                   // SSPCON1 register


// -----------------------------------------------------------------------------
// CKP = 0 = Idle state for clock is a low level
// CKE = 0 = Transmit occurs on transition from Idle to active clock state
// CKP = 1 = Idle state for clock is a high level
// CKE = 1 = Transmit occurs on transition from active to Idle clock state
// -----------------------------------------------------------------------------
