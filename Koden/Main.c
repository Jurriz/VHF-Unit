#include <p18f46k22.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <timers.h>
#include <usart.h>
#include <spi.h>
#include <i2c.h>
#include <delays.h>
#include <adc.h>
#include "Header_h.h"

// -----------------------------------------------------------------------------
// PIC18LF46k22 
/*
#pragma config DEBUG = ON        // Debugger 
#pragma config XINST = ON       // Extended instruction set  

#pragma config STVREN = 1       // Stack overflow reset enable
#pragma config WDTEN = OFF      // Watchdog timer enable
#pragma config CP0 = OFF        // Code protection

#pragma config FCMEN = OFF
#pragma config IESO = OFF

#pragma config PRICLKEN = ON

//#pragma config FOSC = LFINTOSC


#pragma config WDTPS = 32
#pragma config FOSC = XT             //ÄNDRAT från HS till XT
//#pragma config WAIT = OFF             ÄNDRAT
//#pragma config BW = 8                 ÄNDRAT
//#pragma config MODE = MM              ÄNDRAT
#define _XTAL_FREQ  32678
// -----------------------------------------------------------------------------
*/

#pragma config FOSC = INTIO67    // Oscillator Selection bits->Internal oscillator block
#pragma config PLLCFG = OFF    // 4X PLL Enable->Oscillator used directly
#pragma config PRICLKEN = ON    // Primary clock enable bit->Primary clock is always enabled
#pragma config FCMEN = OFF    // Fail-Safe Clock Monitor Enable bit->Fail-Safe Clock Monitor disabled
#pragma config IESO = OFF    // Internal/External Oscillator Switchover bit->Oscillator Switchover mode disabled

// CONFIG2L
#pragma config PWRTEN = OFF    // Power-up Timer Enable bit->Power up timer disabled
#pragma config BOREN = OFF    // Brown-out Reset Enable bits->Brown-out Reset enabled in hardware only (SBOREN is disabled)
#pragma config BORV = 190    // Brown Out Reset Voltage bits->VBOR set to 1.90 V nominal

// CONFIG2H
#pragma config WDTEN = OFF    // Watchdog Timer Enable bits->WDT is controlled by SWDTEN bit of the WDTCON register
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits->1:32768

// CONFIG3H
#pragma config CCP2MX = PORTC1    // CCP2 MUX bit->CCP2 input/output is multiplexed with RC1
#pragma config PBADEN = ON    // PORTB A/D Enable bit->PORTB<5:0> pins are configured as analog input channels on Reset
#pragma config CCP3MX = PORTB5    // P3A/CCP3 Mux bit->P3A/CCP3 input/output is multiplexed with RB5
#pragma config HFOFST = ON    // HFINTOSC Fast Start-up->HFINTOSC output and ready status are not delayed by the oscillator stable status
#pragma config T3CMX = PORTC0    // Timer3 Clock input mux bit->T3CKI is on RC0
#pragma config P2BMX = PORTD2    // ECCP2 B output mux bit->P2B is on RD2
#pragma config MCLRE = EXTMCLR    // MCLR Pin Enable bit->MCLR pin enabled, RE3 input pin disabled

// CONFIG4L
#pragma config STVREN = ON    // Stack Full/Underflow Reset Enable bit->Stack full/underflow will cause Reset
#pragma config LVP = ON    // Single-Supply ICSP Enable bit->Single-Supply ICSP enabled if MCLRE is also 1
#pragma config XINST = ON    // Extended Instruction Set Enable bit->Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
#pragma config DEBUG = OFF    // Background Debug->Disabled

// CONFIG5L
#pragma config CP0 = OFF    // Code Protection Block 0->Block 0 (000800-003FFFh) not code-protected
#pragma config CP1 = OFF    // Code Protection Block 1->Block 1 (004000-007FFFh) not code-protected
#pragma config CP2 = OFF    // Code Protection Block 2->Block 2 (008000-00BFFFh) not code-protected
#pragma config CP3 = OFF    // Code Protection Block 3->Block 3 (00C000-00FFFFh) not code-protected

// CONFIG5H
#pragma config CPB = OFF    // Boot Block Code Protection bit->Boot block (000000-0007FFh) not code-protected
#pragma config CPD = OFF    // Data EEPROM Code Protection bit->Data EEPROM not code-protected

// CONFIG6L
#pragma config WRT0 = OFF    // Write Protection Block 0->Block 0 (000800-003FFFh) not write-protected
#pragma config WRT1 = OFF    // Write Protection Block 1->Block 1 (004000-007FFFh) not write-protected
#pragma config WRT2 = OFF    // Write Protection Block 2->Block 2 (008000-00BFFFh) not write-protected
#pragma config WRT3 = OFF    // Write Protection Block 3->Block 3 (00C000-00FFFFh) not write-protected

// CONFIG6H
#pragma config WRTC = OFF    // Configuration Register Write Protection bit->Configuration registers (300000-3000FFh) not write-protected
#pragma config WRTB = OFF    // Boot Block Write Protection bit->Boot Block (000000-0007FFh) not write-protected
#pragma config WRTD = OFF    // Data EEPROM Write Protection bit->Data EEPROM not write-protected

// CONFIG7L
#pragma config EBTR0 = OFF    // Table Read Protection Block 0->Block 0 (000800-003FFFh) not protected from table reads executed in other blocks
#pragma config EBTR1 = OFF    // Table Read Protection Block 1->Block 1 (004000-007FFFh) not protected from table reads executed in other blocks
#pragma config EBTR2 = OFF    // Table Read Protection Block 2->Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks
#pragma config EBTR3 = OFF    // Table Read Protection Block 3->Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks

// CONFIG7H
#pragma config EBTRB = OFF    // Boot Block Table Read Protection bit->Boot Block (000000-0007FFh) not protected from table reads executed in other blocks

void HandleIRQ(void);

// -----------------------------------------------------------------------------
char nByte_1, nByte_2;
char nTimeOutUSART_1, nTimeOutUSART_2;
char nCountToFive;

volatile union 
{
	struct 
	{
		unsigned bTimerIRQ		:1;
		unsigned bReceived_1	:1;
		unsigned bReceived_2	:1;
		unsigned bReadInc		:1;
		unsigned bToggle		:1;
		unsigned bPassThrough	:1;
		unsigned bNMEA			:1;
		unsigned bNMEA_ON_Once	:1;
		unsigned bNMEA_OFF_Once	:1;
		unsigned bSPIbusy		:1;
	};
	long int AllMyFlags;
} FlagBits;


//#pragma udata USART_Buffer0
	char szUSART_1[32];
//#pragma udata

//#pragma udata USART_Buffer1
	char szUSART_Out[128];
//#pragma udata

//#pragma udata USART_Buffer2
	char szUSART_2[32];
//#pragma udata

// unsigned char nTICK;

const rom unsigned char AccWrite = 0x0A, AccRead = 0x0B, Fifo = 0x0D;
const rom unsigned char RTCWrite = 0x00, RTCRead = 0x80;

const rom unsigned char RTCControl = 0b00000000, RTCClock = 0b00001000, RTCAlarm = 0b00010000, RTCTimer = 0b00011000;
const rom unsigned char RTCTemp    = 0b00100000, RTCEE    = 0b00101000, RTCEECtrl = 0b00110000, RTCRAM = 0b00111000;

short long lTid, lDatum;
unsigned char nWeekDay;
short long lGPSTid, lGPSDatum;
short long lDefaultTid, lDefaultDatum;

//----------------------------------------------------------------------------
void SkrivBuffert(char *szUt, char nVal)
{
	int nLoop = 0;
	
	if (nVal == 1)
	{
		while (szUt[nLoop] != NULL)
		{
			putc1USART(szUt[nLoop]);
			while (Busy1USART() );
			Delay100TCYx(5);
			nLoop++;
		}
	}
	else	
	{
		while (szUt[nLoop] != NULL)
		{
			putc2USART(szUt[nLoop]);
			while (Busy2USART() );
			Delay100TCYx(5);
			nLoop++;
		}
	}	
}

// -----------------------------------------------------------------------------
void main(void)
{
    unsigned char nLoop, nLoop1, nPek, nTmp, nSlask, nPoll, nTemp, WAI, nTest;
	unsigned char nHi, nMHi, nMLo, nLo;	
	unsigned char nTICK, TempL, TempH, test, CTS_OK, szData[];
    unsigned char X_L, X_H, Y_L, Y_H, Z_L, Z_H;

	signed char nOldX, nOldY, nOldZ;
    signed int xVal, yVal, zVal, xVal_100, yVal_100, zVal_100, i=0;
	char lData;

	FlagBits.bTimerIRQ = 0;
        
	InitCPU();
    OSCILLATOR_Initialize();  // VET EJ
	
	//RTC_ENABLE = 0;
    

    //LATAbits.LATA0 = 1; // 2.1 V 
    LATAbits.LATA2 = 1; // 3.3 V   
    
	//Delay(100);
	
	//INTCONbits.GIE = 0;		// Global

	// **********************************************************************
 	TRISCbits.TRISC7 = 1;		// RX
	TRISCbits.TRISC6 = 0;		// TX
	
//---------------------------------------------------------------------------------------    
    OpenSPI1(SPI_FOSC_16, MODE_11, SMPMID);     

        ACC_ENABLE = 0; // CS dras låg

        WriteSPI1(0x24);           // Skriv till 0x22 REG8 // CTRL_REG5 (0x24) 
        WriteSPI1(0x80);           // Software Reset och Reboot (0x81) // BOOT (1000 0000) (0x80)
        
        Delay(100); // Viktigt att ha denna tillräckligt stor så att kretsen hinner starta om helt
        
        ToggleACC();

        WriteSPI1(0x20);            // Skriv till CTRL_REG6_XL.. (0x20) // (CTRL_REG1) 
        WriteSPI1(0x97);            // 0111 1000 vilket ger accelerometern hastigheten 238 Hz (0x98) // 1001 0111 (0x97)
        Delay(100);
        
        ToggleACC();

        WriteSPI1(0x8F);            // Läs från WHO_AM_I 
        nTmp = MyReadSPI();         // Spara värdet i nTmp
        ACC_ENABLE = 1;
        CloseSPI1();
    
        RED_LED = 1;
        Delay(300);                 // Tänd  
        RED_LED = 0;
        
    
    // Testkod för att se så att kretsen gör som den ska och fungerar 
    while (0)
    {
        
        // ---------- Läs från Accelerometern ----------------------------------
        ACC_ENABLE = 0;
        OpenSPI1(SPI_FOSC_16, MODE_11, SMPMID);   
        Delay(1);
        WriteSPI1(0xA8);       //(0xA8) Läs från adress 0x28 "OUT_X_L", data läses automatiskt ut i en linjär rörelse, bit 0 & 1 måste vara satta
        X_L = MyReadSPI();		// Data skickas sedan till TempL
        
        RED_LED = 1; Delay(5);
        RED_LED = 0;
        
        WriteSPI1(0xA9);
        X_H = MyReadSPI();

        ACC_ENABLE = 1;
        CloseSPI1();
        
        // Räkna ut accelerometerdatan, klumpa ihop de lägre bitarna med de högre
        xVal = AccDataCalc(X_L, X_H);

        xVal_100 = xVal / 100;

        GREEN_LED = 1; Delay(5); 
        GREEN_LED = 0;
    } 
       
       while(1) { 
        // --------- Läs från radion: ------------------------------------------
        
        // Startar TCXO
        //TCXO_EN = 1;
        //LATCbits.LATC7 = 1;
        GPIO_RC1 = 1;
        Delay(3);
        OpenSPI1(SPI_FOSC_4, MODE_00, SMPEND);
        
        DoResetRadio();
        
        DoCheckCTSManyTimes();
        
        DoStartRadio();
        
        DoCheckCTSManyTimes();
         
        // ----- Kör en läsning av info ----
        ToggleRadio();
        
        WriteSPI1(0x01);       
        
        RADIO_EN = 1;
        Delay(1);
        
        nTmp = DoCheckCTSManyTimes();
        
        if (nTmp == 0xFF)   // Om CTS är hög är allting rätt
        {
            CTS_OK = 1;     // Kollar så att CTS är ok
            nLoop = 0;
            while (nLoop < 8)
            {
                szData[nLoop] = MyReadSPI();
                nLoop++;
            }	
        }		
        
        RADIO_EN = 1;
        
        CloseSPI1();
        RED_LED = 1; Delay(10); 
        RED_LED = 0;
        
    }  
        // --------- Läs från radion: ------------------------------------------
        OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);
        
        DoResetRadio();
        
        DoCheckCTSManyTimes();
        
        DoStartRadio();
        
        DoCheckCTSManyTimes();
         
        // ----- Kör en läsning av info ----
        ToggleRadio();
        
        WriteSPI1(0x01);       
        
        RADIO_EN = 1;
        Delay(1);
        
        nTmp = DoCheckCTSManyTimes();
        
        if (nTmp == 0xFF)   // Om CTS är hög är allting rätt
        {
            CTS_OK = 1;     // Kollar så att CTS är ok
            nLoop = 0;
            while (nLoop < 8)
            {
                szData[nLoop] = MyReadSPI();
                nLoop++;
            }	
        }		
        
        RADIO_EN = 1;
        CloseSPI1();
        
        // ---------------------------------------------------------------------
        
        for (i=0; i<nLoop; i++)     // Skriv ut utläst data från PART_INFO
        {
        //    sprintf(szUSART_Out, (const rom far char *)"Index %d - %02X \r\n ", i, szData[i]); // Visar part_info
        //    SkrivBuffert(szUSART_Out, 1);
        }
        //sprintf(szUSART_Out, (const rom far char *)"\r\n");     //Lägger in lite radbrytningar
        //SkrivBuffert(szUSART_Out, 1);
        
        //Blink1();
    
    

    i=0;
    
    
    while(1)
    {
        
    // Kod till I2C accelerometern:
    SSP1ADD = 0x18;
    
    //RTC_ENABLE = 1;     // För att se på skåpet
    
    OpenI2C(MASTER, SLEW_ON); 
    
    StartI2C();        // Start  kommando

    WriteI2C(0xD4); //SAD + W (D6)
    
    AckI2C();   // Get acknowledgement
    
    WriteI2C(0x22); // Adress 0x0f (Who_AM_I), CTRL_REG_1 
    
    AckI2C();   // Get Akcnowledgement
    
    RestartI2C(); // Repeted Start
    
    WriteI2C(0xD4); //SAD + R (D7)
    
    AckI2C();   // Get acknowledgement
    
    WriteI2C(0x81); // SW_RESET och BOOT
    
    AckI2C();
    
    StopI2C();
    
    CloseI2C();
    
    // ---- KÖR IGEN -----------------------------------------------------------
    
    OpenI2C(MASTER, SLEW_ON); 
    
    StartI2C();        // Start  kommando

    IdleI2C();
    
    nTmp = WriteI2C(0xD4); //SAD + W (D6)
       
    IdleI2C();
    
    AckI2C();   // Get acknowledgement
    
    IdleI2C();
    
    nTmp = WriteI2C(0x22); // Read from adress 0x0f (Who_AM_I)

    IdleI2C();
    
    AckI2C();   // Get Akcnowledgement
    
    IdleI2C();
    
    RestartI2C(); // Repeted Start

    IdleI2C();
    
    nTmp = WriteI2C(0xD5); //SAD + R (D7)

    IdleI2C();
    
    AckI2C();   // Get acknowledgement
    
    IdleI2C();
    
    nTmp = ReadI2C();

    IdleI2C();
    
    NotAckI2C();
    
    StopI2C();
    
    CloseI2C();
    
    //RTC_ENABLE = 0;
    
	Blink1();
    i++;
   
    }
//    
}	