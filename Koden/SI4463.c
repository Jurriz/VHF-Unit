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
    Delay(1);   // 350 us
    RADIO_SDN = 0;
    Delay(6);   // 5ms
}

// -----------------------------------------------------------------------------
void DoStartRadio(void)
{
    RADIO_EN = 0;
    
    MyWriteSPI(0x02);       // Skriv på funktionen POWER_UP  

    MyWriteSPI(0x81);       // Skriv in funktionen BOOT_OPTIONS 0x81
    MyWriteSPI(0x01);       // Skriv in funktionen XTAL_OPTIONS 0x01

    MyWriteSPI(0x01);       // Skicka in en 32-bitars adress som beskriver hastigheten på den interna klockan på radiokortet
    MyWriteSPI(0x8C);       // Denna är satt till ~28 Mhz eller nått
    MyWriteSPI(0xBA);
    MyWriteSPI(0x80);

    Delay(60);  // 14ms     // Viktigt att den hinner starta ordentligt

   RADIO_EN = 1;
}

// -----------------------------------------------------------------------------
unsigned char ReadFromRadio (unsigned char nProp, unsigned char nLen)
{
	unsigned char nTmp, nLoop;
    long nReturn;
    
    RADIO_EN=0;
    MyWriteSPI(0x01);       
    RADIO_EN = 1;
    
    Delay(1);
    
    RADIO_EN = 0;
    
    MyWriteSPI(0x44);		
    nTmp = MyReadSPI();

    nLoop = 0;
    while ( (nTmp != 0xFF) && (nLoop < 10) )
    {   
        RADIO_EN = 0;		
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