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
	Nop(); Nop(); Nop(); Nop();
	ACC_ENABLE = 0;
}

// -----------------------------------------------------------------------------
long int DoReadInc(void)                    // 
{
	unsigned char nX, nY, nZ;
	long lReturn;

	FlagBits.bSPIbusy = 1;                  // SPI �r upptagen

	OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);	// Hastigheten blir 250Khz
	
	//ACC_ENABLE = 0;

	Nop(); Nop(); Nop();

	MyWriteSPI(0x8F);                       // L�s fr�n... 
	//MyWriteSPI(0x08);                     // ...adress 0x08 (X-axeln) 
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
	ACC_ENABLE = 0;                 // CS dras l�g
    
	MyWriteSPI(0x21);               // LIS2DW12 CTRL2 (0x22)
	MyWriteSPI(0xC0);               // LIS2DW12 Software Reset och Reboot (0x81)
    
    //MyWriteSPI(0x24);               // LIS2DH12 CTRL_REG5
    //MyWriteSPI(0x80);               // LIS2DH12 Boot
    
    Delay(100);
    
    ToggleACC(); 
    
    WriteSPI1(0x20);                // LIS2DW12 CTRL1  (0x20)
    WriteSPI1(0x70);                // LIS2DW12 0111 0000 vilket ger accelerometern hastigheten 400/200 Hz (0x98)
    
    //WriteSPI1(0x20);                // LIS2DH12 CTRL_REG1  (0x20)
    //WriteSPI1(0x77);                // LIS2DH12 0111 0111 vilket ger accelerometern hastigheten 400/200 Hz (0x98)
    
    Delay(100);
    
    ToggleACC();
    
	WriteSPI1(0x8F);                // L�s fr�n WHO_AM_I 
	lReturn = MyReadSPI();          // Data skickas till lReturn
    
	ACC_ENABLE = 1;
	
	return lReturn;
}

// -----------------------------------------------------------------------------
signed int AccDataCalc(unsigned char val_L, unsigned char val_H)
{
    signed int nTmp;
    nTmp = val_H;
    nTmp = nTmp << 8;
    nTmp = nTmp | val_L;
    
    if(nTmp < 0)
           nTmp =- nTmp;
    return nTmp;
}

// -----------------------------------------------------------------------------
//void PrintAccData(signed int Acc_val, char text)
//{
//    for (int i = 0; Acc_val > i; i++)
//    {
//        sprintf(szUSART_Out, (const rom far char *), text); // Utskrift p� sk�rmen
//        SkrivBuffert(szUSART_Out, 1);
//    }
//}

// CKP = 0
// CKE = 0

// Enligt Microchip g�ller f�ljande:
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
        
        // R�kna ut accelerometerdatan, klumpa ihop de l�gre bitarna med de h�gre
//        xVal = AccDataCalc(X_L, X_H);
//        yVal = AccDataCalc(Y_L, Y_H);
//        zVal = AccDataCalc(Z_L, Z_H);

        // Skriver ut antalet x,y,z f�r hur stort utslag de har.
//        PrintAccData(xVal, "x");
//        PrintAccData(yVal, "x");
//        PrintAccData(zVal, "x");
        
        // Delar alla v�rden med 100 f�r att kunna visa data p� sk�rmen lite enklare
//        xVal_100 = xVal / 100;
//        yVal_100 = yVal / 100;
//        zVal_100 = zVal / 100;

        // Skriv ut ett antal "X" p� sk�rmen som motsvarar absolutbeloppett av v�rdet p� X-axeln delat p� 100
//        for (nHi = 0; xVal_100 > nHi; nHi++)
//        {
//            printf(szUSART_Out, (const rom far char *)"X"); // Utskrift p� sk�rmen
//            SkrivBuffert(szUSART_Out, 1);
//        }
//        
//         for (nHi = 0; yVal_100 > nHi; nHi++)
//        {
//            printf(szUSART_Out, (const rom far char *)"Y"); // Utskrift p� sk�rmen
//            SkrivBuffert(szUSART_Out, 1);
//        }
//        
//         for (nHi = 0; zVal_100 > nHi; nHi++)
//        {
//            printf(szUSART_Out, (const rom far char *)"Z"); // Utskrift p� sk�rmen
//            SkrivBuffert(szUSART_Out, 1);
//        }
        
        //Skriver ut X, Y och Z med deras v�rden
        //sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n X \t %04d \r\n Y \t %04d \n\r Z \t %04d \r\n\r\n", 
        //        xVal, yVal, zVal); // Utskrift p� sk�rmen
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

