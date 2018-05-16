#include <p18f46k22.h>
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
	
	//ACC_ENABLE = 0;

	Nop(); Nop(); Nop();

	MyWriteSPI(0x8F);		// Läs från... 
	//MyWriteSPI(0x08);			// ...adress 0x08 (X-axeln) 
	nX = MyReadSPI();
	nY = MyReadSPI();
	nZ = MyReadSPI();

	//ACC_ENABLE = 1;

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

	//FlagBits.bSPIbusy = 1;
	
    //ACC_POW_VDD_IO = 1;   Delay(1);     //  Output POWER TO VDD_IO
    //ACC_POW_VDD = 1;                    //  Output POWER TO VDD
    //Delay(1000);            // Viktigt att accelerometern hinner stara upp ordentligt
	//OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);	// Farten minskad för att försäkra sig om att skrivinstruktionen ska gå fram (0x03)
	ACC_ENABLE = 0; // CS dras låg
    
    //Delay(1);
	Blink2;
	MyWriteSPI(0x22);		// Skriv till 0x22 REG8
	MyWriteSPI(0x81);		// Software Reset och Reboot
    Delay(1);
    ACC_ENABLE = 1;
	Delay(300); // Viktigt att ha denna tillräckligt stor så att kretsen hinner starta om helt
	ACC_ENABLE = 0; 
    
    WriteSPI1(0x20);         // Skriv till CTRL_REG6_XL..  
    WriteSPI1(0x98);         // 1001 1000 vilket ger accelerometern hastigheten 238 Hz
    
    ToggleACC();
    
	WriteSPI1(0x8F);               // Läs från WHO_AM_I 
	lReturn = MyReadSPI();			// Data skickas till lReturn
    
	ACC_ENABLE = 1;

	//CloseSPI();

	Nop();
	
    //FlagBits.bSPIbusy = 0;

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




//      Y_L = MyReadSPI();		// Data skickas sedan till TempL
//      Y_H = MyReadSPI();
//
//      Z_L = MyReadSPI();		// Data skickas toll Z_L och Z_H
//      Z_H = MyReadSPI();

//        ACC_ENABLE = 1;
//        CloseSPI1();
        
        // Räkna ut accelerometerdatan, klumpa ihop de lägre bitarna med de högre
//        xVal = AccDataCalc(X_L, X_H);
//        yVal = AccDataCalc(Y_L, Y_H);
//        zVal = AccDataCalc(Z_L, Z_H);

        // Skriver ut antalet x,y,z för hur stort utslag de har.
//        PrintAccData(xVal, "x");
//        PrintAccData(yVal, "x");
//        PrintAccData(zVal, "x");
        
        // Delar alla värden med 100 för att kunna visa data på skärmen lite enklare
//        xVal_100 = xVal / 100;
//        yVal_100 = yVal / 100;
//        zVal_100 = zVal / 100;

        // Skriv ut ett antal "X" på skärmen som motsvarar absolutbeloppett av värdet på X-axeln delat på 100
//        for (nHi = 0; xVal_100 > nHi; nHi++)
//        {
//            printf(szUSART_Out, (const rom far char *)"X"); // Utskrift på skärmen
//            SkrivBuffert(szUSART_Out, 1);
//        }
//        
//         for (nHi = 0; yVal_100 > nHi; nHi++)
//        {
//            printf(szUSART_Out, (const rom far char *)"Y"); // Utskrift på skärmen
//            SkrivBuffert(szUSART_Out, 1);
//        }
//        
//         for (nHi = 0; zVal_100 > nHi; nHi++)
//        {
//            printf(szUSART_Out, (const rom far char *)"Z"); // Utskrift på skärmen
//            SkrivBuffert(szUSART_Out, 1);
//        }
        
        //Skriver ut X, Y och Z med deras värden
        //sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n X \t %04d \r\n Y \t %04d \n\r Z \t %04d \r\n\r\n", 
        //        xVal, yVal, zVal); // Utskrift på skärmen
        //SkrivBuffert(szUSART_Out, 1);
//            for (i=0; i<xVal; i++){
//                GREEN_LED = 0; Delay(20);
//                GREEN_LED = 1; Delay(20);
//                GREEN_LED = 0; Delay(20);
//                GREEN_LED = 1; Delay(20);
//            }
//Blink2();
  //sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n LSM9DS1 Initiering:\r\n\r\n WHO AM I? \t0x%02X (0x68)\r\n\r\n", lData);
        //SkrivBuffert(szUSART_Out, 1);

