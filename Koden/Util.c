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
void InitCPU(void)
{
	// Endast digitala ing�ngar nu
	//ANCON0 = 0xFF;                                    �NDRAT
	//ANCON1 = 0xFF;

    //18F45k22
    //Digitala ing�ngar
    
    // TRIS s�tter riktning p� datan
    // 1 Output
    // 0 Input
    
    //LAT s�tter pullup eller pulldown
    // 1 Pullup till Vdd
    // 0 Pulldown till Jord
    
    ANSELA = 0x00;  // S�tter om alla pinnar till digitala ist�llet f�r det initialt analoga
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    
    // S�tter voltage supply p� utg�ngar p� processorn
    LATCbits.LATC0 =  0;     // S�tter upp C0 som latch
    LATCbits.LATC1 =  0;     // S�tter upp C1 som latch 
    TRISCbits.RC0 = 0;      // S�tter RC0 som utg�ng
    TRISCbits.RC1 = 0;      // S�tter RC1 som utg�ng
    
    
	PORTA = 0b00000000;
	TRISA = 0b11100011;
	LATA  = 0b00011000;
	
	PORTB = 0b00000000;
	TRISB = 0b00011011;
    
    LATE  = 0b00000011; // S�tter CS kontakterna f�rst, drar de normalt h�ga
	TRISE = 0b00000000; // Alla �r utg�ngar

	LATD  = 0b00000000;	
	TRISD = 0b00000000; 	// LEDar, alla ut

//	LATF  = 0b00010000;		// RF4=MEMORY_ENABLE_0      �NDRAT
//	TRISF = 0b10000000;
//	LATF  = 0b00000100;	
//	TRISF = 0b00000010;

//	TRISG = 0b00000101; 
//	LATG  = 0b00000010;

//	TRISH = 0b00000100; 
//	LATH  = 0b00000010;

//	PORTJ = 0b00000000;
//	TRISJ = 0b11110000;
//	LATJ  = 0x00;

	// -------------------------------
	
//	MEMCONbits.EBDIS = 1;

	// PIC18F87J11
	// ********************************************************************************
	// ADCON1bits.PCFGn styr AD-kanalerna (bl.a. PORTH.7.6.5.4 �r ANALOGA ing�ngar default)
	// Endast digitala ing�ngar nu
	/*ANCON0 = 0xFF;
	ANCON1 = 0xFF;

	// Voltage Reference Configuration Bits
	ADCON0bits.VCFG1 = 0;	// VDD och VSS
	ADCON0bits.VCFG0 = 0;
	ADCON0bits.ADON  = 0;
	
	// H�gerjusterat
	ADCON1bits.ADFM  = 1;	
	ADCON1bits.ADCAL = 0;	

	// AD Acquisition Time Select Bits
	ADCON1bits.ACQT2 = 1;	// 8 Tad
	ADCON1bits.ACQT1 = 0;
	ADCON1bits.ACQT0 = 0;

	// AD Conversion Clock Select Bits
	ADCON1bits.ADCS2 = 1;	// Fosc/16
	ADCON1bits.ADCS1 = 0;
	ADCON1bits.ADCS0 = 1;
*/
	// PIC18F8722
	// ********************************************************************************
	// ADCON1bits.PCFGn styr AD-kanalerna (bl.a. PORTH.7.6.5.4 �r ANALOGA ing�ngar default)
	// M�tning p� AN_MUX_OUT_0 (AN0) och AN_MUX_OUT_1 (AN1)
	// Endast digitala ing�ngar nu
/*	ADCON1bits.PCFG3 = 1;	// 
	ADCON1bits.PCFG2 = 1; 	// 0;
	ADCON1bits.PCFG1 = 1;	// 0;
	ADCON1bits.PCFG0 = 1;

	// Voltage Reference Configuration Bits
	ADCON1bits.VCFG1 = 0;	// VDD och VSS
	ADCON1bits.VCFG0 = 0;
	
	// AD Conversion Clock Select Bits
	ADCON2bits.ADCS2 = 1;	// Fosc/16
	ADCON2bits.ADCS1 = 0;
	ADCON2bits.ADCS0 = 1;

	// AD Acquisition Time Select Bits
	ADCON2bits.ACQT2 = 1;	// 8 Tad
	ADCON2bits.ACQT1 = 0;
	ADCON2bits.ACQT0 = 0;

	// H�gerjusterat
	ADCON2bits.ADFM  = 1;	
*/
    /*
	// IRQ ---------------------------
	RCONbits.IPEN = 0;		// Ingen prioritering av IRQ
	
	INTCONbits.RBIF = 0;	// IRQ on change
	INTCONbits.RBIE = 1;	// IRQ on change
	INTCONbits.INT0IF = 0;	// IRQ p� RB0
	INTCONbits.INT0IE = 1;	// IRQ p� RB0
//	INTCON2bits.INTEDG3 = 0;                            �NDRAT

	// IRQ
	INTCON3bits.INT3IE = 1;	// IRQ p� RB3
	INTCON3bits.INT3IF = 0;	// Inte IRQ p� RB3

	INTCON2bits.INTEDG0 = 0;

	// IRQ
	INTCON3bits.INT2IE = 0;	// Inte IRQ p� RB2
	INTCON3bits.INT2IF = 0;	// Inte IRQ p� RB2

	// IRQ
	INTCON3bits.INT1IF = 0;	// Inte IRQ p� RB1
	INTCON3bits.INT1IE = 0;	// Inte IRQ p� RB1

	INTCONbits.GIE = 1;		// Global
	INTCONbits.PEIE = 1;	// Perferi

	// Timer0: Avstudsning f�r PINpad	210ms intervall
	// Timer1: Timeout f�r CCTalk		1s, pulsas fr�n RTC (ej vid batteridrift)
	// Timer2: Seriekommunikation		6.5ms intervall
	// Timer3: TICK?					500ms, pulsas fr�n RTC (ej vid batteridrift)
	// Timer4: Test						6.5ms intervall
*/
    
	OpenTimer2(TIMER_INT_ON & T2_PS_1_16 & T2_POST_1_16);	// 6.5ms
	WriteTimer2(0);
	PR2 = 0xFF;
	PIR1bits.TMR2IF = 0;

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

	FlagBits.bReceived_2 = 0;
	nTimeOutUSART_2 = 0;
	nByte_2 = 0;
	szUSART_2[0] = '\0';
}

// ------------------------------------------------------------------------------------------------------------------
// Returnerar veckodag, 0=s�ndag, 1=m�ndag, 2=tisdag osv.
int dow(int y, int m, int d)  /* 1 <= m <= 12, y > 1752 */
{
	const unsigned int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	int  nReturn;

	y -= m < 3;

	nReturn = (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
	Nop();
	return nReturn;
}

//---------------------------------------------------------------------------------------------
// Omvandla nIn i BCD och returnera som vanlig hex
unsigned char BCD2Byte(unsigned char nIn)
{ 
	unsigned char nB2BTmp;
	
	// Omvandla nIn i BCD och returnera som vanlig hex ----------------------------------------
	nB2BTmp = (nIn & 0xf0);		// Kopiera h�g nibble till nTemp10
	nB2BTmp >>= 4;				// skifta 4 steg v�nster)
	nB2BTmp *= 10;
	
	nB2BTmp += (nIn & 0x0f);

	return nB2BTmp;
}

//---------------------------------------------------------------------------------------------
unsigned char MyReadSPI(void)
{
	SSPBUF = 0x00;
	
	TMR0L = 0;										// initiate bus cycle

	// TMR0L < 20 ger 640us TimeOut
	while ( (!SSPSTATbits.BF) && (TMR0L < 20) );	// wait until cycle complete

	return (SSPBUF);								// return with byte read 
}

unsigned char SPI1_Exchange8bit(unsigned char data)
{
    // Clear the Write Collision flag, to allow writing
    SSP1CON1bits.WCOL = 0;

    SSP1BUF = data;

    while(SSP1STATbits.BF == 0x0)
    {
    }

    return (SSP1BUF);
}

//---------------------------------------------------------------------------------------------
unsigned char MyWriteSPI(unsigned char data_out)
{
	char nSlask;

	SSPBUF = data_out;				// write byte to SSPBUF register
	if ( SSPCON1 & 0x80 )			// test if write collision occurred
	{
		return (-1);				// if WCOL bit is set return negative #
	}
	else
	{
		TMR0L = 0;

		// TMR0L < 20 ger 640us TimeOut
		while ( (!SSPSTATbits.BF) && (TMR0L < 20) );	// wait until bus cycle complete 
	}
	
	// Nytt 2016-02
	nSlask = SSPBUF;
	
	return (nSlask);					// if WCOL bit is not set return non-negative#
}

// -----------------------------------------------------------------------------
void Delay(unsigned int nDelay)
{
	unsigned int nLoop0, nLoop1;
	
	for (nLoop0 = 0; nLoop0 < nDelay; nLoop0++)
	{
//		for (nLoop1 = 0; nLoop1 < 250; nLoop1++);	// HS
		for (nLoop1 = 0; nLoop1 < 99; nLoop1++);	// INTOSC
	}	
}

void Blink(void)
{
    LATDbits.LATD0 = 1;
    Delay(500);
    LATDbits.LATD1 = 1;
    Delay(500);
    LATDbits.LATD2 = 1;
    Delay(500);
    LATDbits.LATD3 = 1;
    Delay(500);
    LATDbits.LATD4 = 1;
    Delay(500);
    LATDbits.LATD5 = 1;
    Delay(500);
    LATDbits.LATD0 = 0;
    Delay(500);
    LATDbits.LATD1 = 0;
    Delay(500);
    LATDbits.LATD2 = 0;
    Delay(500);
    LATDbits.LATD3 = 0;
    Delay(500);
    LATDbits.LATD4 = 0;
    Delay(500);
    LATDbits.LATD5 = 0;
}

// -----------------------------------------------------------------------------

	// Enligt Microchip g�ller f�ljande:
	//
	// 				CKP		CKE
	// MODE_00		0		1
	//		01		0		0
	//		10		1		1
	//		11		1		0


	//  CKP = Clock Polarity Select bit
	//		1 = Idle state for clock is a high level
	//		0 = Idle state for clock is a low level


	//	CKE: SPI Clock Select bit
	//		1 = Transmit occurs on transition from active to Idle clock state
	//		0 = Transmit occurs on transition from Idle to active clock state

	
	// SMP	
	//	SPI Master mode:
	//		1 = Input data sampled at end of data output time
	//		0 = Input data sampled at middle of data output time
	// 
	//	SPI Slave mode:
	//		SMP must be cleared when SPI is used in Slave mode


	// INTOSC => 166.7kHz m�tt p� LED_0
	// HS     => 416.6kHz m�tt p� LED_0
//	while (1)
//	{
//		Delay(10);
//		LED_0 = !LED_0;
//	}	
	


/*
	01 10 01 10
	-----------
44  01 00 01 00
42	01 00 00 10
41	01 00 00 01
48	01 00 10 00
60	01 10 00 00
50	01 01 00 00
*/