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
// PIC18F87J11 (j10)
#pragma config DEBUG = ON
#pragma config XINST = ON          //ÄNDRAT FRÅN ON till OFF

#pragma config STVREN = ON
#pragma config WDTEN = OFF
#pragma config CP0 = OFF

#pragma config FCMEN = OFF
#pragma config IESO = OFF

#pragma config PRICLKEN = ON
#pragma config FOSC = XT             //ÄNDRAT från HS till XT
//#pragma config FOSC = INTOSC

#pragma config WDTPS = 32
//#pragma config WAIT = OFF             ÄNDRAT
//#pragma config BW = 8                 ÄNDRAT
//#pragma config MODE = MM              ÄNDRAT

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
char DoResetRTC_INT(void);

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
	unsigned char nLoop, nPek, nTmp, nSlask, nPoll, nTemp, WAI;
	unsigned char nHi, nMHi, nMLo, nLo;	
	unsigned char nTICK, TempL, TempH;
    unsigned char X_L, X_H, Y_L, Y_H, Z_L, Z_H;

	signed char nOldX, nOldY, nOldZ;
	char lData;

	FlagBits.bTimerIRQ = 0;
        
	InitCPU();

	//ACC_ENABLE = 1;
	
	RTC_ENABLE = 0;

	Delay(100);
	
	INTCONbits.GIE = 0;		// Global

	// **********************************************************************
 	TRISCbits.TRISC7 = 1;		// RX
	TRISCbits.TRISC6 = 0;		// TX
	
	// 57600bps
	Open1USART( USART_TX_INT_OFF &
		USART_RX_INT_ON &
		USART_ASYNCH_MODE &
		USART_EIGHT_BIT &
		USART_CONT_RX &
		USART_BRGH_HIGH,
		42 );				// HS      10MHz 
//		16 );				// INTOSC  4MHz

	BAUDCON1bits.BRG16 = 1;

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
    //Blink();
    
	lData = DoStartST_ACC();        // Do start gör ingen skillnad.
	
	Nop();
	sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n LSM9DS1 Initiering:\r\n\r\n WHO AM I? \t0x%02X (0x68)\r\n\r\n", lData);
	SkrivBuffert(szUSART_Out, 1);
   
//    OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);
//    Delay(1);
//    ACC_ENABLE = 0;
//    Delay(1);
//    SPI1_Exchange8bit(0x8F);
//    lData = SPI1_Exchange8bit(0x00);
//    sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n LSM9DS1 läst med annan SPI funktion \r\n\r\n WHO AM I? \t0x%02X (0x68)\r\n\r\n", lData);
//	SkrivBuffert(szUSART_Out, 1);
//    Delay(1);
//    ACC_ENABLE = 1;
//    Delay(1);
//    CloseSPI();
    
    
    
	FlagBits.bReadInc = 0;
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
	Nop();
	
    //while (1)
        {     
            FlagBits.bSPIbusy = 1;
            OpenSPI(SPI_FOSC_16, MODE_11, SMPEND);
            ACC_ENABLE = 0;
            Delay(10);
            MyWriteSPI(0x8F);           // Läs från adress 0x0F "WHO AM I"
            WAI = MyReadSPI();			// Data skickas sedan till WAI
            Delay(10);
            //ACC_ENABLE = 1;
            //CloseSPI();
            //FlagBits.bSPIbusy = 0;
            
            Delay(1);
            ToggleACC();
            Delay(1);
            
            //FlagBits.bSPIbusy = 1;
            //OpenSPI(SPI_FOSC_16, MODE_11, SMPMID);
            //ACC_ENABLE = 0;
            //Delay(10);
            
            // X Pitch axis
            MyWriteSPI(0x98);           // Läs från adress 0x15 "OUT_X_L_G"
            X_L = MyReadSPI();		// Data skickas sedan till TempL
            ToggleACC();
            MyWriteSPI(0x99); 
            X_H = MyReadSPI();
            ToggleACC();
            
            // Y Roll axis
            MyWriteSPI(0x9A);           // Läs från adress 0x15 "OUT_X_L_G"
            Y_L = MyReadSPI();		// Data skickas sedan till TempL
            ToggleACC();
            MyWriteSPI(0x9B); 
            Y_H = MyReadSPI();
            ToggleACC();
             
            // Z Yaw axis
            MyWriteSPI(0x9C);           // Läs från adress 0x15 "OUT_X_L_G"
            Z_L = MyReadSPI();		// Data skickas sedan till TempL
            ToggleACC();
            MyWriteSPI(0x9D); 
            Z_H = MyReadSPI();
           

//            ACC_ENABLE = 1;
//            CloseSPI();
//            FlagBits.bSPIbusy = 0;
//            
//            Delay(1000);
//            ToggleACC();
//            Delay(1000);
//             
//            FlagBits.bSPIbusy = 1;
//            OpenSPI(SPI_FOSC_16, MODE_11, SMPMID);
//            ACC_ENABLE = 0;
//            Delay(10);
//            MyWriteSPI(0x99);           // Läs från adress 0x15 "OUT_TEMP_L"
//            TempH = MyReadSPI();        // Läs OUT_TEMP_H som kommer efter i registret
            
            ACC_ENABLE = 1;
            CloseSPI();
            FlagBits.bSPIbusy = 0;
            
            sprintf(szUSART_Out, (const rom far char *)"\x0C\r\n Who am I?\t0x%02X \r\n X_L \t0x%02X \r\n X_H \t0x%02X \n\r\n Y_L \t0x%02X \r\n Y_H \t0x%02X \n\r\n Z_L \t0x%02X \r\n Z_H \t0x%02X \r\n", 
                    WAI, X_L, X_H, Y_L, Y_H, Z_L, Z_H); // Utskrift på skärmen
            SkrivBuffert(szUSART_Out, 1);

            Blink();    // Blinka lamporna
            
            Nop();
            Nop();
        }		
    
    LATDbits.LATD0 = 1;
    Delay(1000);
    LATDbits.LATD1 = 0;
    Delay(1000);
    DoReadAllRTC_Regs();
	
	nTmp = DoReadRTC();
	if (nTmp != 0)
	{
		Nop();

		// Initiera om klockan
		DoInitRV3049();
	}

	Nop();

	// Sätt klockticket
	nTICK = 8;
	DoChangeTickRV3049(nTICK);

	// Ställ in tid och datum enlig nedan
	lGPSTid = 0x133030;
	lGPSDatum = 0x180308;

	Nop();
	Nop();

	DoSetTimeRV3049();

	// Kontroll
	nTmp = DoReadRTC();
	Delay(1000);
	nTmp = DoReadRTC();

	// DoResetRTC_INT();
	FlagBits.bTimerIRQ = 0;
	nLoop = 0;
	nTICK = 2;
	//while (1)
	{	
		if (FlagBits.bTimerIRQ == 1)
		{
			// Interrupt-flaggorna blir lästa och clearade i DoReadRTC()
			LED_0 = 1;
//			DoReadRTC();
			
			// Interrupt-flaggorna clearas i DoResetRTC_INT()
			// DoResetRTC_INT();
			FlagBits.bTimerIRQ = 0;
			
			nLoop++;
			if ( (nLoop % 5) == 0)
			{
				nLoop = 0;
				DoChangeTickRV3049(nTICK);
				nTICK += 2;
				if (nTICK > 12)
				{
					nTICK = 2;
				}	
			} 

			lData = DoReadInc();

			nLo = (lData & 0x000000FF);		// X
			lData >>= 8;
			nMLo = (lData & 0x000000FF);	// Y
			lData >>= 8;
			nMHi = (lData & 0x000000FF);	// Z

			sprintf(szUSART_Out, (const rom far char *)" %3d\t%3d\t%3d\t%d\r\n\r\n", (signed char)nLo, (signed char)nMLo, (signed char)nMHi, (char)FlagBits.bToggle);
			SkrivBuffert(szUSART_Out, 1);

			FlagBits.bToggle = !FlagBits.bToggle;
		}	
		
		Nop();
		Nop();
	}

	Delay(1000);
    LATDbits.LATD4 = 1;
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
}	

 

