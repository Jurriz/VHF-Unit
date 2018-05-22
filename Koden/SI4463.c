#include <p18f46k22.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <timers.h>
//#include <usart.h>
#include <spi.h>
#include <delays.h>
#include <adc.h>

#include "Header_h.h"



// -----------------------------------------------------------------------------
void ToggleRadio(void)
{
	RADIO_EN = 1;
	Delay(1);
	RADIO_EN = 0;
}

// -----------------------------------------------------------------------------
void DoResetRadio(void)
{
    RADIO_SDN = 1;
    Delay(2);   // 350 us ??
    RADIO_SDN = 0;
    Delay(10);   // 5ms ??
}

// -----------------------------------------------------------------------------
void DoStartRadio(void)
{
    RADIO_EN = 0;
    Delay(1);
    MyWriteSPI(0x02);       // Skriv p� funktionen POWER_UP  

    MyWriteSPI(0x01);       // Skriv in funktionen BOOT_OPTIONS 0x81
    MyWriteSPI(0x01);       // Skriv in funktionen XTAL_OPTIONS 0x01

    MyWriteSPI(0x01);       // Skicka in en 32-bitars adress som beskriver hastigheten p� den interna klockan p� radiokortet
    MyWriteSPI(0x8C);       // Denna �r satt till 26 Mhz 
    MyWriteSPI(0xBA);
    MyWriteSPI(0x80);

    Delay(20);  // 14ms     // Viktigt att den hinner starta ordentlig

   RADIO_EN = 1;
}

// -----------------------------------------------------------------------------
unsigned char ReadFromRadio (unsigned char nProp, unsigned char nLen)
{
	unsigned char nTmp, nLoop;
    long nReturn; // Fungerar det som det ska?
    
    RADIO_EN=0;
    Delay(1);
    MyWriteSPI(0x01);       
    RADIO_EN = 1;
    
    Delay(1);
    
    RADIO_EN = 0;
    Delay(1);
    MyWriteSPI(0x44);		
    nTmp = MyReadSPI();

    nLoop = 0;
    while ( (nTmp != 0xFF) && (nLoop < 10) )
    {   
        RADIO_EN = 0;	
        Delay(1);
        MyWriteSPI(0x44);
        nTmp = MyReadSPI();
        RADIO_EN = 1;
        nLoop++;
    }	
    if (nTmp == 0xFF)
	{
		nReturn = 1;
		nLoop = 0;
		while (nLoop < nLen)
		{
//			szData[nLoop] = MyReadSPI();
            nTmp = MyReadSPI();
            sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n L�sning: %d \r\n Read from radio: \t0x%02X \r\n\r\n", nLoop, nTmp);
            SkrivBuffert(szUSART_Out, 1);
			nLoop++;
		}	
	}

    RADIO_EN = 1;
	return nReturn;
}

// ------------------------------------------------------------------------------------
unsigned char DoCheckCTSManyTimes(void)
{
    unsigned char nTmp, nLoop;
    
    RADIO_EN = 0;
    Delay(1);
    MyWriteSPI(0x44);		
    nTmp = MyReadSPI();

    nLoop = 0;
    while ( (nTmp != 0xFF) && (nLoop < 10) )
    {
        nLoop++;
        RADIO_EN = 1;
        Nop();
        RADIO_EN = 0;
        MyWriteSPI(0x44);
        nTmp = MyReadSPI();
    }	
    return nTmp;
}