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
//#pragma config WAIT = OFF             
//#pragma config BW = 8                 
//#pragma config MODE = MM              
// -----------------------------------------------------------------------------


#pragma config FOSC = INTIO67    // Oscillator Selection bits->Internal oscillator block
#pragma config PLLCFG = OFF    // 4X PLL Enable->Oscillator used directly
#pragma config PRICLKEN = ON    // Primary clock enable bit->Primary clock is always enabled
#pragma config FCMEN = OFF    // Fail-Safe Clock Monitor Enable bit->Fail-Safe Clock Monitor disabled
#pragma config IESO = OFF    // Internal/External Oscillator Switchover bit->Oscillator Switchover mode disabled

//// CONFIG2L
//#pragma config PWRTEN = OFF    // Power-up Timer Enable bit->Power up timer disabled
#pragma config BOREN = OFF    // Brown-out Reset Enable bits->Brown-out Reset enabled in hardware only (SBOREN is disabled)
//#pragma config BORV = 190    // Brown Out Reset Voltage bits->VBOR set to 1.90 V nominal
//
//// CONFIG2H
//#pragma config WDTEN = OFF    // Watchdog Timer Enable bits->WDT is controlled by SWDTEN bit of the WDTCON register
//#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits->1:32768

// CONFIG3H
#pragma config CCP2MX = PORTC1    // CCP2 MUX bit->CCP2 input/output is multiplexed with RC1
#pragma config PBADEN = ON    // PORTB A/D Enable bit->PORTB<5:0> pins are configured as analog input channels on Reset
#pragma config CCP3MX = PORTB5    // P3A/CCP3 Mux bit->P3A/CCP3 input/output is multiplexed with RB5
#pragma config HFOFST = ON    // HFINTOSC Fast Start-up->HFINTOSC output and ready status are not delayed by the oscillator stable status
#pragma config T3CMX = PORTC0    // Timer3 Clock input mux bit->T3CKI is on RC0
#pragma config P2BMX = PORTD2    // ECCP2 B output mux bit->P2B is on RD2
#pragma config MCLRE = EXTMCLR    // MCLR Pin Enable bit->MCLR pin enabled, RE3 input pin disabled

//// CONFIG4L
#pragma config STVREN = ON    // Stack Full/Underflow Reset Enable bit->Stack full/underflow will cause Reset
#pragma config LVP = ON    // Single-Supply ICSP Enable bit->Single-Supply ICSP enabled if MCLRE is also 1
#pragma config XINST = ON    // Extended Instruction Set Enable bit->Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
#pragma config DEBUG = ON    // Background Debug->Disabled
//
//// CONFIG5L
//#pragma config CP0 = OFF    // Code Protection Block 0->Block 0 (000800-003FFFh) not code-protected
//#pragma config CP1 = OFF    // Code Protection Block 1->Block 1 (004000-007FFFh) not code-protected
//#pragma config CP2 = OFF    // Code Protection Block 2->Block 2 (008000-00BFFFh) not code-protected
//#pragma config CP3 = OFF    // Code Protection Block 3->Block 3 (00C000-00FFFFh) not code-protected
//
//// CONFIG5H
//#pragma config CPB = OFF    // Boot Block Code Protection bit->Boot block (000000-0007FFh) not code-protected
//#pragma config CPD = OFF    // Data EEPROM Code Protection bit->Data EEPROM not code-protected
//
//// CONFIG6L
//#pragma config WRT0 = OFF    // Write Protection Block 0->Block 0 (000800-003FFFh) not write-protected
//#pragma config WRT1 = OFF    // Write Protection Block 1->Block 1 (004000-007FFFh) not write-protected
//#pragma config WRT2 = OFF    // Write Protection Block 2->Block 2 (008000-00BFFFh) not write-protected
//#pragma config WRT3 = OFF    // Write Protection Block 3->Block 3 (00C000-00FFFFh) not write-protected
//
//// CONFIG6H
//#pragma config WRTC = OFF    // Configuration Register Write Protection bit->Configuration registers (300000-3000FFh) not write-protected
//#pragma config WRTB = OFF    // Boot Block Write Protection bit->Boot Block (000000-0007FFh) not write-protected
//#pragma config WRTD = OFF    // Data EEPROM Write Protection bit->Data EEPROM not write-protected
//
//// CONFIG7L
//#pragma config EBTR0 = OFF    // Table Read Protection Block 0->Block 0 (000800-003FFFh) not protected from table reads executed in other blocks
//#pragma config EBTR1 = OFF    // Table Read Protection Block 1->Block 1 (004000-007FFFh) not protected from table reads executed in other blocks
//#pragma config EBTR2 = OFF    // Table Read Protection Block 2->Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks
//#pragma config EBTR3 = OFF    // Table Read Protection Block 3->Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks
//
//// CONFIG7H
//#pragma config EBTRB = OFF    // Boot Block Table Read Protection bit->Boot Block (000000-0007FFh) not protected from table reads executed in other blocks



// -----------------------------------------------------------------------------
char nByte_1, nByte_2;
char nTimeOutUSART_1, nTimeOutUSART_2;
char nCountToFive;
char filter;

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

//const rom unsigned char RTCControl = 0b00000000, RTCClock = 0b00001000, RTCAlarm = 0b00010000, RTCTimer = 0b00011000;
//const rom unsigned char RTCTemp    = 0b00100000, RTCEE    = 0b00101000, RTCEECtrl = 0b00110000, RTCRAM = 0b00111000;

//short long lTid, lDatum;
//unsigned char nWeekDay;
//short long lGPSTid, lGPSDatum;
//short long lDefaultTid, lDefaultDatum;

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
    
    
    unsigned char nLoop, nTmp;
    unsigned char FREQ_CTRL_INTE, FREQ_CTRL_FRAC_H, FREQ_CTRL_FRAC_M, FREQ_CTRL_FRAC_L;
    unsigned char FREQ_CTRL_INTE_433, FREQ_CTRL_FRAC_H_433, FREQ_CTRL_FRAC_M_433, FREQ_CTRL_FRAC_L_433;
	unsigned char szData[];
    unsigned char X_L, X_H, Y_L, Y_H, Z_L, Z_H;

    signed int xVal, yVal, zVal, xVal_100, yVal_100, zVal_100, i=0, j=0;

    
	
    
    //FlagBits.bTimerIRQ = 0;  
	InitCPU();
    OSCILLATOR_Initialize();  
    
    // S�tter 3.3 V direkt
    VSEL1 = 1; 
    VSEL2 = 0; // Inverterad
    VSEL3 = 1;
    GREEN_LED = 0;
    Delay(5000);        // Testa str�mmen
    
    
//	OpenTimer3(TIMER_INT_ON & T3_16BIT_RW & T3_SOURCE_INT & T3_PS_1_8);
//	WriteTimer3(0x7FFF);	// 1 sekund
//	WriteTimer3(0xBFFF);	// 500ms
	WriteTimer3(0);	
	
//	OpenTimer4(TIMER_INT_ON & T4_PS_1_16 & T4_POST_1_16);	// 6.5ms
//	WriteTimer4(0x00);
	
	nByte_1 = 0;
	nByte_2 = 0;
	nTimeOutUSART_1 = 0;
	nTimeOutUSART_2 = 0;

	// Ta bort eventuellt falskt IRQ 
	getc1USART();	
	getc2USART();	

	// Weak pull-up
	INTCON2bits.RBPU = 0;

	FlagBits.bReceived_1 = 0;
	nTimeOutUSART_1 = 0;
	nByte_1 = 0;
	szUSART_1[0] = '\0';
    
// 19200bps ------------------------
	
    Open1USART( USART_TX_INT_OFF &
		USART_RX_INT_ON &
		USART_ASYNCH_MODE &
		USART_EIGHT_BIT &
		USART_CONT_RX &
		USART_BRGH_HIGH,
//		42 );				// HS      10MHz 
//		16 );				// INTOSC  4MHz
      12  );                 // INTIO67 1MHz 

	BAUDCON1bits.BRG16 = 1;

    
    FlagBits.bReceived_2 = 0;
	nTimeOutUSART_2 = 0;
	nByte_2 = 0;
	szUSART_2[0] = '\0';
    
//	// 9600bps -------------------------
    
//	Open2USART( USART_TX_INT_OFF &
//		USART_RX_INT_ON &
//		USART_ASYNCH_MODE &
//		USART_EIGHT_BIT &
//		USART_CONT_RX &
//		USART_BRGH_HIGH,
//		259);				// INTOSC  4MHz
////	103);				// HS     10MHz

	//BAUDCON2bits.BRG16 = 1;
	
	//INTCONbits.GIE = 0;		// Global

    
// ************************************************************************************************** Startar programmet h�r
    
    
    while(1)        // Ta bort all testkod f�r att bara k�ra radio
    {
    // ---------- L�s fr�n Accelerometern ----------------------------------
    OpenSPI1(SPI_FOSC_4, MODE_11, SMPMID);   
    nTmp = DoStartST_ACC();
    CloseSPI1();

    sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n LIS2DW12 INITIERING:\r\n\r\n WHO_AM_I? \t0X%02X (0X44)\r\n", nTmp);
    SkrivBuffert(szUSART_Out, 1);
    Delay(10);
    
    
    sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n K�r aceleromtern 50 ggr: \r\n");
    SkrivBuffert(szUSART_Out, 1);
    Delay(500);
    for(i=0;i<50;i++)
    {
        RED_LED = 1;
        OpenSPI1(SPI_FOSC_4, MODE_11, SMPMID);    
        ACC_ENABLE = 0;

        WriteSPI1(0xA8);        //(0xA8) L�s fr�n adress 0x28 "OUT_X_L", data l�ses automatiskt ut i en linj�r r�relse, bit 0 & 1 m�ste vara satta
        X_L = MyReadSPI();		// Data l�ses fr�n den �vre delen av X registrer
        X_H = MyReadSPI();      // Data l�ses fr�n den nedre delen av X registrer
        Y_L = MyReadSPI();
        Y_H = MyReadSPI();
        Z_L = MyReadSPI();
        Z_H = MyReadSPI();

        ACC_ENABLE = 1;
        CloseSPI1();

        // R�kna ut accelerometerdatan, klumpa ihop de l�gre bitarna med de h�gre
        xVal = AccDataCalc(X_L, X_H);
        yVal = AccDataCalc(Y_L, Y_H);
        zVal = AccDataCalc(Z_L, Z_H);

        // Dela v�rdet p� 100 f�r att l�ttare analysera data
        xVal_100 = xVal / 100;
        yVal_100 = yVal / 100;
        zVal_100 = zVal / 100;

        sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n X VAL: \t %d \r\n Y VAL: \t %d \r\n Z VAL: \t %d \r\n", xVal_100, yVal_100, zVal_100);
        SkrivBuffert(szUSART_Out, 1);
        RED_LED = 0;
        Delay(25);
    }
 //while(1){
    // ---------------------------------------------------- Testar Hall Brytaren
    RED_LED =1;
    for(j=0; j<75; j++)
    {
        i = Hall_Out; //PORTBbits.RB5
        if (i==0)
            {
            sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n Hallswitchen �r aktiv! \r\n");
            SkrivBuffert(szUSART_Out, 1);    
            }
        else
        {
            sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n Avst�ngd. \r\n");
            SkrivBuffert(szUSART_Out, 1);  
        }
            sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n Test #%d ", j);
            SkrivBuffert(szUSART_Out, 1); 
        GREEN_LED = 0;    
        Delay(50);
        GREEN_LED = 1;
    }

    // TESTA Sp�nningsregulatorn  ---------------------------------------------
    TestaVSEL();
    
   }  
    
    
    // Ans�tt variablerna redan i koden, dessa skall senare s�ttas via datorn direkt
    // S�tter upp variabler med information om hastighet f�r radion
    //-----------------------------------------------
    FREQ_CTRL_INTE = 0x44;          // hastigheten skall vara 151.123
    FREQ_CTRL_FRAC_H = 0x0D;    
    FREQ_CTRL_FRAC_M = 0xFE;
    FREQ_CTRL_FRAC_L = 0x1C;
    // ------------------------------------------------
    FREQ_CTRL_INTE_433 = 0x41;          // hastigheten skall vara 433.0515
    FREQ_CTRL_FRAC_H_433 = 0x0C;    
    FREQ_CTRL_FRAC_M_433 = 0xFC;
    FREQ_CTRL_FRAC_L_433 = 0x88;
    // ------------------------------------------------

    // Skriver in v�rdena f�r hastigheten 151.123 till EEProm
    Write2EE(FREQ_CTRL_INTE, 0x0000);       // 0
    Write2EE(FREQ_CTRL_FRAC_H, 0x0001);     // 1
    Write2EE(FREQ_CTRL_FRAC_M, 0x0002);     // 2
    Write2EE(FREQ_CTRL_FRAC_L, 0x0003);     // 3

    // Skriver in v�rdena f�r hastigheten 433.0515 till EEProm
    Write2EE(FREQ_CTRL_INTE_433, 0x0004);   // 4
    Write2EE(FREQ_CTRL_FRAC_H_433, 0x0005); // 5
    Write2EE(FREQ_CTRL_FRAC_M_433, 0x0006); // 6
    Write2EE(FREQ_CTRL_FRAC_L_433, 0x0007); // 7 
    
    // ----------------------------------------------------  Initierar radion

    TCXO_EN = 1;        //Startar TCXO

    filter = 1; // S�tter switchen till VHF
    InitTRXAndGotoSleep(); 
    
    OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);

    TRX_EN = 0;     // Drar chip select f�r radion l�g

    DoCheckCTSManyTimes1();

    ToggleRadio();

    WriteSPI1(0x01);    // CTS

    ToggleRadio();

    nTmp = DoCheckCTSManyTimes1();

    // H�mtar PART_INFO
    if (nTmp == 0xFF)               // Om CTS �r h�g �r allting r�tt
    {
        nLoop = 0;                  
        while (nLoop < 4)           // L�ser in fyra register.
        {
            szData[nLoop] = MyReadSPI();
            nLoop++;
        }
        
        // Skriver ut relevant data, svaret skall vara 44 60 f�r denna radio. 
        sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n PART_ID_RADIO: - %02X%02X - \r\n\r\n", szData[1], szData[2]);
        SkrivBuffert(szUSART_Out, 1);
    }

    TRX_EN = 1;     // CS f�r radion dras h�g
    CloseSPI1(); 

    while(1)    // Skickar pulser med radion
    {
    for(i = 0; i < 20; i++) // Hur m�nga g�nger skall radio skall s�nda pulser
        {
            
            sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n V�nda %d av (10) \r\n\r\n -- UHF Signal --\r\n\r\n", i);
            SkrivBuffert(szUSART_Out, 1); Delay(10);
            
            filter = 0;    // UHF GR�N
            DoInitBeacon();
            GreenLedPulse(30);
            Delay(50);
            GreenLedPulse(30);
            Delay(500);
            
            sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n -- VHF Signal --\r\n\r\n");
            SkrivBuffert(szUSART_Out, 1); Delay(10);
            
            filter = 1;     // VHF R�D
            DoInitBeacon();
            RedLedPulse(30);
            Delay(50);
            RedLedPulse(30); 
            Delay(500);
        }
    }
           
    
    // I2C kod till accelerometern som jag testade:  
    // ------------------------------------------------------
    
//    OpenI2C2(MASTER, SLEW_ON); 
//    
//    StartI2C2();        // Start  kommando
//    WriteI2C2(0x64); //SAD + W (D6) // 0110 010 (R/W) 
//    
//    AckI2C2();   // Get acknowledgement
//    
//    WriteI2C2(0x22); // Adress 0x0f (Who_AM_I), CTRL_REG_1 
//    
//    AckI2C2();   // Get Akcnowledgement
//    
//    RestartI2C2(); // Repeted Start
//    WriteI2C2(0xD4); //SAD + R (D7)
//    
//    AckI2C2();   // Get acknowledgement
//    
//    WriteI2C2(0x81); // SW_RESET och BOOT
//    
//    AckI2C2();
//    
//    StopI2C2();
//    CloseI2C2();
//
//    // ---- K�R IGEN -----------------------------------------------------------
//    
//    OpenI2C(MASTER, SLEW_ON); 
//    StartI2C();        // Start  kommando
//
//    IdleI2C();
//    nTmp = WriteI2C(0xD4); //SAD + W (D6)
//       
//    IdleI2C();
//    
//    AckI2C();   // Get acknowledgement
//    
//    IdleI2C();
//    
//    nTmp = WriteI2C(0x22); // Read from adress 0x0f (Who_AM_I)
//
//    IdleI2C();
//    
//    AckI2C();   // Get Akcnowledgement
//    
//    IdleI2C();
//    
//    RestartI2C(); // Repeted Start
//
//    IdleI2C();
//    
//    nTmp = WriteI2C(0xD5); //SAD + R (D7)
//
//    IdleI2C();
//    
//    AckI2C();   // Get acknowledgement
//    
//    IdleI2C();
//    
//    nTmp = ReadI2C();
//
//    IdleI2C();
//    
//    NotAckI2C();
//    
//    StopI2C();
//    CloseI2C();  
}	