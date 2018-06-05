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
void ToggleRadio(void)
{
	TRX_EN = 1;
	Delay(1);
	TRX_EN = 0;
}

// -----------------------------------------------------------------------------
void DoResetRadio(void)
{
    RADIO_SDN = 1;
    Delay(1);   // 350 us ??
    RADIO_SDN = 0;
    Delay(10);   // 5ms ??
}

// -----------------------------------------------------------------------------
void DoStartRadio(void)
{
    TRX_EN = 0;

    MyWriteSPI(0x02);       // Skriv på funktionen POWER_UP  
    
    MyWriteSPI(0x01);       // Skriv in funktionen BOOT_OPTIONS 0x81
    MyWriteSPI(0x01);       // Skriv in funktionen XTAL_OPTIONS 0x01

    MyWriteSPI(0x01);       // Skicka in en 32-bitars adress som beskriver hastigheten på den interna klockan på radiokortet
    MyWriteSPI(0x8C);       // 
    MyWriteSPI(0xBA);       // Denna är satt till 26 Mhz
    MyWriteSPI(0x80);       //

    Delay(20);  // 14ms     // Viktigt att den hinner starta ordentlig

   TRX_EN = 1;
}

// -----------------------------------------------------------------------------
unsigned char ReadFromRadio (unsigned char nProp, unsigned char nLen) // Vilket register samt hur många  
{
	unsigned char nTmp, nLoop;
    long nReturn; // Fungerar den som det ska?
    
    TRX_EN=0;
    MyWriteSPI(nProp);      // Skickar in valt register
    
    ToggleRadio();
    
    MyWriteSPI(0x44);		// Fråga efter CTS
    nTmp = MyReadSPI();

    nLoop = 0;
    while ( (nTmp != 0xFF) && (nLoop < 10) )    // Så länge inte radion svarar CTS skickas frågan igen
    {   
        TRX_EN = 0;	
        MyWriteSPI(0x44);
        nTmp = MyReadSPI();
        TRX_EN = 1;
        nLoop++;
    }	
    
    // Efter 10 försök kollas det om radion har svarat CTS
    TRX_EN = 0;
    if (nTmp == 0xFF)
	{
		nReturn = 1;
        TRX_EN = 0;
		nLoop = 0;
		while (nLoop < nLen)
		{
//			szData[nLoop] = MyReadSPI();
            nTmp = MyReadSPI();
            sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n NUM: %d \r\n READ FROM RADIO: \t0X%02X \r\n\r\n", nLoop, nTmp);
            SkrivBuffert(szUSART_Out, 1);
			nLoop++;
		}	
	}

    TRX_EN = 1;
	return nReturn;
}

// ------------------------------------------------------------------------------------
unsigned char DoCheckCTSManyTimes(void)
{
    unsigned char nTmp, nLoop;
    
    Delay(1);
    MyWriteSPI(0x44);		
    nTmp = MyReadSPI();

    nLoop = 0;
    while ( (nTmp != 0xFF) && (nLoop < 10) )
    {
        nLoop++;
       TRX_EN = 1;
        Nop();
        TRX_EN = 0;
        MyWriteSPI(0x44);
        nTmp = MyReadSPI();
    }	
    return nTmp;
}

        // --------- Läs från radion: ------------------------------------------

//        // ----------------------------------------------------  Startar TX
//        
//        MyWriteSPI(0x31);           // Skcikar till START_TX registeret
//        
//        MyWriteSPI(0x77);           // Channel to transmit package on
//        MyWriteSPI(0x31);           // Condition  (0111 0000)
//        MyWriteSPI(0x00);           // TX_LEN
//        MyWriteSPI(0x00);           // TX_LEN
//        
//        DoCheckCTSManyTimes();
//        
//        // ------------------------------------------------------ Skickar data på TX
        

        //Startar TCXO
       
//                
//        OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);
//        
//        DoResetRadio();
//        
//        DoCheckCTSManyTimes();
//        
//        DoStartRadio();
//        
//        DoCheckCTSManyTimes();
//         
//        // ----- Kör en läsning av info ----
//        ToggleRadio();
//        
//        WriteSPI1(0x01);       
//        //ReadFromRadio(1,8);
//        
//        RADIO_EN = 1;
//        Delay(1);
//        
//        nTmp = DoCheckCTSManyTimes();
//        
//        if (nTmp == 0xFF)   // Om CTS är hög är allting rätt
//        {
//            CTS_OK = 1;     // Kollar så att CTS är ok
//            nLoop = 0;
//            while (nLoop < 8)
//            {
//                szData[nLoop] = MyReadSPI();
//                nLoop++;
//            }	
//        }		
//        
//        RADIO_EN = 1;
//        
//        CloseSPI1();
//        RED_LED = 1; Delay(1); 
//        RED_LED = 0;
//        
//        sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n SI4460:\r\n\r\n nTmp \t0x%02X \r\n\r\n", nTmp);
//        SkrivBuffert(szUSART_Out, 1);
//       
     