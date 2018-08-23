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
void InitCPU(void)
{
	
    // --- 18F46k22 -------
    
    // TRIS sätter riktning på datan
    // 0 Output
    // 1 Input
    
    //LAT sätter pullup eller pulldown
    // 1 Pullup till Vdd
    // 0 Pulldown till Jord
    
    // Endast digitala ingångar nu, sätter om alla pinnar till digitala istället för det initialt analoga
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;  
    
	//PORTA = 0b00000000;
    LATA  = 0b00100000; // VSEL1 & VSEL2 & VSEL3 // GREEN_LED
	TRISA = 0b00000000; // 0b11100011;
    
	//PORTB = 0b00000000;
	//TRISB = 0b11011111; // Acc_int, RTC_int, TCXO_EN, FREE, NIRQ_Radio, Hall_out,PGC,PGD 
    LATB  = 0b11001100; //
    
    //PORTC = 0b00000000;
    TRISC = 0b00000001; // XTAL, RC2 SDN_RADIO // RC3,RC4 & RC5 SPI // RC6 & RC7 TX/RX //
	LATC  = 0b00100000; // SND_radio hög
    
    //PORTE = 0b00000000;
    //TRISD = 0b00000000; 	// 
	LATD  = 0b11101000; //     // CS_ACC & Radio CS sätts hög vid uppstart
	
    
    //PORTE = 0b00000000; // 
    LATE  = 0b00000000; // RED_LED
	TRISE = 0b00000000; // Alla är utgångar

    
    /*
	// IRQ ---------------------------
	RCONbits.IPEN = 0;		// Ingen prioritering av IRQ
	
	INTCONbits.RBIF = 0;	// IRQ on change
	INTCONbits.RBIE = 1;	// IRQ on change
	INTCONbits.INT0IF = 0;	// IRQ på RB0
	INTCONbits.INT0IE = 1;	// IRQ på RB0
//	INTCON2bits.INTEDG3 = 0;                            ÄNDRAT

	// IRQ
	INTCON3bits.INT3IE = 1;	// IRQ på RB3
	INTCON3bits.INT3IF = 0;	// Inte IRQ på RB3

	INTCON2bits.INTEDG0 = 0;

	// IRQ
	INTCON3bits.INT2IE = 0;	// Inte IRQ på RB2
	INTCON3bits.INT2IF = 0;	// Inte IRQ på RB2

	// IRQ
	INTCON3bits.INT1IF = 0;	// Inte IRQ på RB1
	INTCON3bits.INT1IE = 0;	// Inte IRQ på RB1

	INTCONbits.GIE = 1;		// Global
	INTCONbits.PEIE = 1;	// Perferi

	// Timer0: Avstudsning för PINpad	210ms intervall
	// Timer1: Timeout för CCTalk		1s, pulsas från RTC (ej vid batteridrift)
	// Timer2: Seriekommunikation		6.5ms intervall
	// Timer3: TICK?					500ms, pulsas från RTC (ej vid batteridrift)
	// Timer4: Test						6.5ms intervall

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
     */
}

void OSCILLATOR_Initialize(void)    // ?
{
    // SCS FOSC; IRCF 1MHz_HFINTOSC/16; IDLEN disabled; 
    OSCCON = 0x30;
    // PRISD enabled; SOSCGO disabled; MFIOSEL disabled; 
    OSCCON2 = 0x04;
    // INTSRC disabled; PLLEN disabled; TUN 0; 
    OSCTUNE = 0x00;
}
// ------------------------------------------------------------------------------------------------------------------
// Returnerar veckodag, 0=söndag, 1=måndag, 2=tisdag osv.
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
	nB2BTmp = (nIn & 0xf0);		// Kopiera hög nibble till nTemp10
	nB2BTmp >>= 4;				// skifta 4 steg vänster)
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
	//	for (nLoop1 = 0; nLoop1 < 250; nLoop1++);	// HS
	//	for (nLoop1 = 0; nLoop1 < 99; nLoop1++);	// INTOSC
        for (nLoop1 = 0; nLoop1 < 24; nLoop1++);	// INTIO67
	}	
}

//---------------------------------------------------------------------------------------------
unsigned char ReadEEByte(int nEEAdr)
{
    EEADRH = ((nEEAdr & 0xff00) >> 8);    // Det måste finnas minst en instruktion mellan EEADR = .. och .RD = 1
    EEADR = (nEEAdr & 0x00ff);            // Det måste finnas minst en instruktion mellan EEADR = .. och .RD = 1

    EECON1bits.EEPGD = 0;        // EEPROM
    EECON1bits.CFGS = 0;        // EEPROM

    EECON1bits.RD = 1;

    return EEDATA;                // OBS: Läsning måste ske direkt efter .RD = 1
}

//---------------------------------------------------------------------------------------------
void Write2EE(const unsigned char nData, const int nAdress)
{
    int WDI; //
    static char nWriteLoop;
    unsigned char bOldINTCON;

    WDI = 1;
    Nop();
    Nop();
    WDI = 0;

    // Spara nuvarande status för GIE (INTCON = 7=GIE/GIEH 6=PEIE/GIEL 5=TMR0IE 4=INT0IE 3=RBIE 2=TMR0IF 1=INT0IF 0=RBIF
    bOldINTCON = INTCON;

    // Tidigare styrde följande
    // "if ( ( (DCDC_ENABLE == 1) && (BatFlag.bLowBat == 0) ) || ( (DCDC_ENABLE == 0) && (MAIN_LOW_BAT == 1) ) )"

    // GIE avstängd under hela operationen
    INTCONbits.GIE = 0;

    nWriteLoop = 0;
    do {
        EEADRH = ((nAdress & 0xff00) >> 8);
        EEADR = (nAdress & 0x00ff);

        EEDATA = nData;

        EECON1bits.EEPGD = 0;        // EEPROM
        EECON1bits.CFGS = 0;        // EEPROM
        EECON1bits.WREN = 1;

        // INTCONbits.GIE = 0;        // OBS VIKTIGT ******************

        EECON2 = 0x55;
        EECON2 = 0xAA;

        EECON1bits.WR = 1;            // Inled skrivning

        // INTCONbits.GIE = 1;        // OBS VIKTIGT ******************

        while(EECON1bits.WR == 1);  // Vänta tills skrivningen är klar, om inte EECON1bits.WR går låg tar watchDogTimern hand om det!

        EECON1bits.WREN = 0;
        nWriteLoop++;
    } while ( (ReadEEByte(nAdress) != nData) && (nWriteLoop < 4) );

    // Maska in det ursprunliga inehållet i INTCON, är GIE == 0 fortsätter den inställningen
    INTCON |= bOldINTCON;
    // INTCONbits.GIE = 1;
} 


// -----------------------------------------------------------------------------

void TestaVSEL(void){
    int nDelay1 = 2000;
    int nDelay2 = 50;
    
        sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n Testar VSEL\r\n 3.3 V \r\n");
        SkrivBuffert(szUSART_Out, 1);
        // Sätter 3.3 V
        VSEL1 = 1; 
        VSEL2 = 0; // Inverterad
        VSEL3 = 1;   
        GREEN_LED = 1;
        Delay(nDelay1);
        GREEN_LED = 0;
        
        sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n Testar VSEL\r\n 3 V \r\n");
        SkrivBuffert(szUSART_Out, 1);
        // Sätter 3.0 V
        LATAbits.LATA0 = 0; 
        LATAbits.LATA1 = 0;  // Inverterad
        LATAbits.LATA2 = 1; 
        RED_LED = 1;
        Delay(nDelay1);
        RED_LED = 0; Delay(nDelay2);
        RED_LED = 1; Delay(nDelay2);
        RED_LED = 0;
        
        sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n Testar VSEL\r\n 2.8 V \r\n");
        SkrivBuffert(szUSART_Out, 1);
        // Sätter 2.8 V
        LATAbits.LATA0 = 1; 
        LATAbits.LATA1 = 1;  // Inverterad
        LATAbits.LATA2 = 1; 
        GREEN_LED = 1;
        Delay(2000);
        GREEN_LED = 0; Delay(nDelay2);
        GREEN_LED = 1; Delay(nDelay2);
        GREEN_LED = 0; Delay(nDelay2);
        GREEN_LED = 1; Delay(nDelay2);
        GREEN_LED = 0;
        
        sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n Testar VSEL\r\n 2.5 V \r\n");
        SkrivBuffert(szUSART_Out, 1);
        // Sätter 2.5 V
        LATAbits.LATA0 = 0; 
        LATAbits.LATA1 = 1;  // Inverterad
        LATAbits.LATA2 = 1; 
        RED_LED = 1;
        Delay(nDelay1);
        RED_LED = 0;Delay(nDelay2);
        RED_LED = 1;Delay(nDelay2);
        RED_LED = 0;Delay(nDelay2);
        RED_LED = 1;Delay(nDelay2);
        RED_LED = 0;Delay(nDelay2);
        RED_LED = 1;Delay(nDelay2);
        RED_LED = 0;
        
        sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n Testar VSEL\r\n 2.1 V \r\n");
        SkrivBuffert(szUSART_Out, 1);
        // Sätter 2.1 V
        LATAbits.LATA0 = 1; 
        LATAbits.LATA1 = 0;  // Inverterad
        LATAbits.LATA2 = 0; 
        GREEN_LED = 1;
        Delay(nDelay1);
        GREEN_LED = 0; Delay(nDelay2);
        GREEN_LED = 1; Delay(nDelay2);
        GREEN_LED = 0; Delay(nDelay2);
        GREEN_LED = 1; Delay(nDelay2);
        GREEN_LED = 0; Delay(nDelay2);
        GREEN_LED = 1; Delay(nDelay2);
        GREEN_LED = 0; Delay(nDelay2);
        GREEN_LED = 1; Delay(nDelay2);
        GREEN_LED = 0;
        
        sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n Testar VSEL\r\n 1.8 V \r\n");
        SkrivBuffert(szUSART_Out, 1);
        // Sätter 1.8 V
        //LATAbits.LATA0 = 0; 
        //LATAbits.LATA1 = 0;  // Inverterad
        //LATAbits.LATA2 = 0;
        
        RED_LED = 1;
        Delay(nDelay1);
        RED_LED = 0;
        
        // 1.8 V verkar inte fungera särskilt bra, processorn stannar precis när den instructionen körs -----
        
        VSEL1 = 1; // 2.1 V 
        VSEL3 = 1; // 3.3 V
}

// -----------------------------------------------------------------------------

	// Enligt Microchip gäller följande:
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


	// INTOSC => 166.7kHz mätt på LED_0
	// HS     => 416.6kHz mätt på LED_0
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



// -----------------------------------------------------------------------------
//char DoResetRTC_INT(void);

/*// -----------------------------------------------------------------------------
#pragma code HighVector=0x0008

void atHighVector (void)
{
	_asm goto HandleIRQ _endasm
}

#pragma code

#pragma interrupt HandleIRQ

/*
void HandleIRQ(void)
{
	unsigned char nLoop, nPortTmp, nIn;
	
	// ***********************************************************************************************************
	if (RCSTA1bits.OERR == 1)	// OverRunError
	{	
		RCSTA1bits.CREN = 0;
		RCSTA1bits.CREN = 1;
	}

	if (RCSTA1bits.FERR == 1)	// FramingError
	{
		nPortTmp = getc1USART();
	}

	if ( (PIE1bits.RC1IE == 1) && (PIR1bits.RC1IF == 1)	)	// Serie Interrupt från USART 1
	{
		nIn = getc1USART();

		if (FlagBits.bPassThrough == 1)
		{
			if (!Busy2USART())
			{
				putc2USART(nIn);
			}
		}	

		szUSART_1[nByte_1] = nIn;
		if (nByte_1 < 510)			// Max 512 tecken
		{
			nByte_1++;
		}

		// Underlättar läsning av text-strängar			
		szUSART_1[nByte_1] = '\0';
		
		nTimeOutUSART_1 = 0;
	}

	// ***********************************************************************************************************		
	if (RCSTA2bits.OERR == 1)	// OverRunError
	{
		RCSTA2bits.CREN = 0;		
		RCSTA2bits.CREN = 1;
	}

	if (RCSTA2bits.FERR == 1)	// OverRunError
	{
		nPortTmp = getc2USART();
	}

	if ( (PIE3bits.RC2IE == 1) && (PIR3bits.RC2IF == 1) )		// Serie Interrupt från USART 2
	{
		nIn = getc2USART();

		if (FlagBits.bPassThrough == 1)
		{
			if (!Busy1USART())
			{
				putc1USART(nIn);
			}
		}	
		
		szUSART_2[nByte_2] = nIn;
		if (nByte_2 < 254)			// Max 256 tecken
		{
			nByte_2++;
		}		

		// Underlättar läsning av text-strängar			
		szUSART_2[nByte_2] = '\0';
		
		nTimeOutUSART_2 = 0;
	}
	

	// ***********************************************************************************************************		
	if ( (INTCON3bits.INT3IF == 1) && (INTCON3bits.INT3IE == 1) )
	{
		LED_6 = !LED_6;

//		DoResetRTC_INT();

		FlagBits.bTimerIRQ = 1;
		// FlagBits.bReadInc = 1;
		
		INTCON3bits.INT3IF = 0;
	}

	// ***********************************************************************************************************		
	if ( (INTCONbits.INT0IF == 1) && (INTCONbits.INT0IE == 1) )
	{
		while (RB0_SWITCH == 0);
		
		FlagBits.bReadInc = 1;
		INTCONbits.INT0IF = 0;
		LED_0 = !LED_0;
	}

	// ***********************************************************************************************************		
	if ( (INTCONbits.TMR0IF == 1) && (INTCONbits.TMR0IE == 1) )		// Avstudsare för PIN-pad
	{
		INTCONbits.TMR0IF = 0;
 		CloseTimer0();
		LED_0 = !LED_0;
	}

	// ***********************************************************************************************************		
	// IRQ var 6.56ms
	if ( (PIR1bits.TMR2IF == 1) && (PIE1bits.TMR2IE == 1) )
	{
		LED_7 = ACC_IRQ;

		LED_1 = !LED_1;
		PIR1bits.TMR2IF = 0;

		nTimeOutUSART_1++;
		nTimeOutUSART_2++;

		if ((nTimeOutUSART_1 >= 3) && (nByte_1 != 0) )
		{
			FlagBits.bReceived_1 = 1;
			nByte_1 = 0;
		}

		if ( (nTimeOutUSART_2 >= 3) && (nByte_2 != 0) )
		{
			FlagBits.bReceived_2 = 1;
			nByte_2 = 0;
		}
	}

	// ***********************************************************************************************************		
	if ( (PIR2bits.TMR3IF == 1) && (PIE2bits.TMR3IE == 1) )			// TICK
	{
		PIR2bits.TMR3IF = 0;
		WriteTimer3(0x0C00);	// 200ms
		nCountToFive++;
		if (nCountToFive > 4)
		{
			LED_3 = !LED_3;		// 200 x 5 = 1000ms
			nCountToFive = 0;
		}	

		if (SEND_NMEA_0 == 1)
		{
			FlagBits.bNMEA = 1;
			LED_4 = 1;
			LED_5 = 0;
		}
		else
		{
			FlagBits.bNMEA = 0;
			LED_4 = 0;
			LED_5 = 1;
		}
	}

	// ***********************************************************************************************************		
	if ( (PIR3bits.TMR4IF == 1) && (PIE3bits.TMR4IE == 1) )			// 6.5ms max, till vad?
	{
		CloseTimer4();
		PIR3bits.TMR4IF = 0;

		// LED_4 = !LED_4;
	}
		
	// ***********************************************************************************************************		
	// IRQ On Change
	if 	( (INTCONbits.RBIE == 1) && (INTCONbits.RBIF == 1) )
	{
		nPortTmp = PORTB;
		INTCONbits.RBIF = 0;
		LED_7 = !LED_7;
		FlagBits.bReadInc = 1;
	}  
}
*/ 


	// 57600bps
	/*
    Open1USART( USART_TX_INT_OFF &
		USART_RX_INT_ON &
		USART_ASYNCH_MODE &
		USART_EIGHT_BIT &
		USART_CONT_RX &
		USART_BRGH_HIGH,
		42 );				// HS      10MHz 
//		16 );				// INTOSC  4MHz

	BAUDCON1bits.BRG16 = 1;
*/
	/*// 9600bps
	Open2USART( USART_TX_INT_OFF &
		USART_RX_INT_ON &
		USART_ASYNCH_MODE &
		USART_EIGHT_BIT &
		USART_CONT_RX &
		USART_BRGH_HIGH,
		259);				// INTOSC  4MHz
//		103);				// HS     10MHz

	BAUDCON2bits.BRG16 = 1;
*/
    //Blink1();
    
	//lData = DoStartST_ACC();        // Kör itiieringen
	
	//Nop();
	//sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n LSM9DS1 Initiering:\r\n\r\n WHO AM I? \t0x%02X (0x68)\r\n\r\n", lData);
	//SkrivBuffert(szUSART_Out, 1);
   
    
//    OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);
//    Delay(1);
//    ACC_ENABLE = 0;
//    Delay(1);
//    SPI1_Exchange8bit(0x8F);
//    lData = SPI1_Exchange8bit(0x00);
//    sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n LSM9DS1 läst med annan SPI funktion \r\n\r\n WHO AM I? \t0x%02X (0x68)\r\n\r\n", lData);
//    SkrivBuffert(szUSART_Out, 1);
//    Delay(1);
//    ACC_ENABLE = 1;
//    Delay(1);
//    CloseSPI();
    
    
    
	//FlagBits.bReadInc = 0;
    /*
	strcpypgm2ram(szUSART_Out, (const rom far char *)" NMEA Generator \r\n\r\n\r\n\r\n");
	SkrivBuffert(szUSART_Out, 1);
	
	INTCONbits.GIE = 1;		// Global

	strcpypgm2ram(szUSART_Out, (const rom far char *)"\r\n RV3049:\r\n\r\n");
	SkrivBuffert(szUSART_Out, 1);


	// Ställ in tid och datum enlig nedan, värdet är sparat sedan tidigare
	lDefaultTid = 0x143000;
	lDefaultDatum = 0x180302;

	// Kontrollera om RTC:n behöver initieras om
	nTmp = DoCheckRV3049Start();
	
	Nop();
	Nop();
	
	//if ( (nTmp & 0x20) != 0)	// PON = bit 5 = 0x20
	{
		DoInitRV3049();
	
		lGPSTid = lDefaultTid;
		lGPSDatum = lDefaultDatum;
		DoSetTimeRV3049();
	}

	// En läsning måste göras för att få igång klockan
	DoReadRTC();
	DoResetRTC_INT();

	FlagBits.bTimerIRQ = 0;
    
//  lData = DoStartADXL362();
//  nLo = (lData & 0x000000FF);		// REVID
//	lData >>= 8;
//	nMLo = (lData & 0x000000FF);	// PARTID
//	lData >>= 8;
//	nMHi = (lData & 0x000000FF);	// DEVID_MST
//	lData >>= 8;
//	nHi = (lData & 0x000000FF);		// DEVID_AD
	*/
	//Nop();
    
//DoStartST_ACC();        // Kör initieringen    

                                      // ÄNTRAT Kommenterat bort allt som har med NMEA att göra
    /*
	FlagBits.AllMyFlags	= 0L;
	FlagBits.bPassThrough = 0;
	FlagBits.bNMEA = 0;
	FlagBits.bNMEA_OFF_Once = 0;
	FlagBits.bNMEA_ON_Once = 0;
	nCountToFive = 0;

	while (1)
	{
		if (FlagBits.bReadInc == 1)
		{
			FlagBits.bReadInc = 0;
			lData = DoReadInc();

			nLo = (lData & 0x000000FF);		// X
			lData >>= 8;
			nMLo = (lData & 0x000000FF);	// Y
			lData >>= 8;
			nMHi = (lData & 0x000000FF);	// Z

			sprintf(szUSART_Out, (const rom far char *)"%3d\t%3d\t%3d\t%d\t", (signed char)nLo, (signed char)nMLo, (signed char)nMHi, (char)FlagBits.bToggle);
			SkrivBuffert(szUSART_Out, 1);
			
			FlagBits.bToggle = !FlagBits.bToggle;
		}	
		
		if (FlagBits.bReceived_1 == 1)
		{
			FlagBits.bReceived_1 = 0;
			nByte_1 = 0;

			if (FlagBits.bNMEA == 1) 
			{
				RCSTA2bits.SPEN = 1;		// USART2
				TRISGbits.TRISG1 = 0;		// RX

				SkrivBuffert(szUSART_1, 2);
			}
			else
			{
				RCSTA2bits.SPEN = 0;		// USART2
				TRISGbits.TRISG1 = 1;		// RX
			}					
		}

		if (FlagBits.bReceived_2 == 1)
		{
			FlagBits.bReceived_2 = 0;
			nByte_2 = 0;
		}	

		if ( (FlagBits.bNMEA == 1) && (FlagBits.bNMEA_ON_Once == 0) )
		{
			FlagBits.bNMEA_ON_Once = 1;
			FlagBits.bNMEA_OFF_Once = 0;

			RCSTA2bits.SPEN = 1;		// USART2
			TRISGbits.TRISG1 = 0;		// RX

			strcpypgm2ram(szUSART_Out, (const rom far char *)" RUN\r\n\r\n");
			SkrivBuffert(szUSART_Out, 1);
			Delay(500);		
		}
		
		if ( (FlagBits.bNMEA == 0) && (FlagBits.bNMEA_OFF_Once == 0) )
		{
			FlagBits.bNMEA_OFF_Once = 1;
			FlagBits.bNMEA_ON_Once = 0;

			RCSTA2bits.SPEN = 0;		// USART2
			TRISGbits.TRISG1 = 1;		// RX

			strcpypgm2ram(szUSART_Out, (const rom far char *)" WAIT\r\n\r\n");
			SkrivBuffert(szUSART_Out, 1);
			Delay(500);
		}					

		Nop();
		Nop();
     */

