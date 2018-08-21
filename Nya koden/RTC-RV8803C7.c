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
/*

// -----------------------------------------------------------------------------
void ToggleRTC(void)
{
	//RTC_ENABLE = 0;
	Nop(); Nop(); Nop();
	//RTC_ENABLE = 1;
}

//---------------------------------------------------------------------------------------------
// const rom unsigned char RTCControl = 0b00000000, RTCClock = 0b00001000; RTCAlarm = 0b00010000; RTCTimer = 0b00011000;
// const rom unsigned char RTCTemp    = 0b00100000, RTCEE    = 0b00101000; RTCEECtrl = 0b00110000; RTCRAMM = 0b00111000;

// -----------------------------------------------------------------------------
char DoInitRV3049(void)
{
	// CKP = 0	Idle state for clock is a low level
	// CKE = 0	Transmit occurs on transition from Idle to active clock state
	// MODE = 01
	
	FlagBits.bSPIbusy = 1;

	OpenSPI(SPI_FOSC_4, MODE_01, SMPMID);

	//RTC_ENABLE = 1;

	// =======================================================
	MyWriteSPI(RTCWrite | RTCControl | 0x04);	// Skriv till Control-Page
	MyWriteSPI(0b00010000);						// CONTROL_RESET, (xxx 0/1 xxxx) SysR = 1 ger "System Reset"

	Nop();

	//RTC_ENABLE = 0;
	
	Delay(500);

	//RTC_ENABLE = 1;

	MyWriteSPI(RTCWrite | RTCControl | 0x03);	// Skriv till Control-Page
	MyWriteSPI(0b00000000);						// CONTROL_Status, återställ PON och SR

	Nop();
	
	// =======================================================

	// Värdet i RTCControl | 0x00 är efter en reset:
	// Clk/Int TD1 TD0 SROn EERE TAR TE WE
	//    1     0   0    1    1   0   0  1

	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control_1
	MyWriteSPI(0b10111000);						// CLK/INT = 1, TD1 TD0 = 01, SROn = 1, EERE = 1	Control_1
	MyWriteSPI(0b00011110);						// SRIE = 1, V2IE = 1, V1IE = 1, TIE = 1			Control_INT
	MyWriteSPI(0x00);							// Cleara IRQ										Control_INT Flag
	MyWriteSPI(0x00);							// Cleara IRQ										Control_Status

	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCTimer | 0x00);	// Skriv till Timer-page 
	MyWriteSPI(0x07);							// Low	7+1=1Hz
	MyWriteSPI(0x00);							// High

	ToggleRTC();

//	MyWriteSPI(RTCWrite | RTCControl | 0x01);	// Skriv till Control_INT
//	MyWriteSPI(0b00000010);						// TIE = 1
//	MyWriteSPI(0b00011110);						// SRIE = 1, V2IE = 1, V1IE = 1, TIE = 1
//	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control_1
	MyWriteSPI(0b10111111);						// CLK/INT = 1, TD1 TD0 = 01, SROn = 1, EERE = 1, TAR = 1, TE = 1, WE = 1
	// Clk/Int: 1 = Applies CLKOUT function on CLKOUT pin
	// TD1 TD0: 01 = Timer Source Clock Frequency: 8 Hz
	// SROn: 	1 = Enables Self Recovery function
	// EERE:	1 = Enables automatic EEPROM refresh every hour
	// TAR: 	1 = Enables Countdown Timer Auto-Reload mode
	// TE: 		1 = Enables Countdown Timer
	// WE:		1 = Enables 1Hz Clock Source for Watch

/*	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control_1
	MyWriteSPI(0b10100100);						// TAR = 1

	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control_1
	MyWriteSPI(0b10100110);						// TE = 1
*/
/*	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x02);	// Skriv till Control_1
	MyWriteSPI(0b00000000);						// TF = 0
	// ================================================================================================
	// ================================================================================================
	// Fortsätt med initieringen
	
	ToggleRTC();
	
	MyWriteSPI(RTCWrite | RTCAlarm | 0x00);	// Skriv till Alarm-page 
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	
	//RTC_ENABLE = 0;

	CloseSPI();

	FlagBits.bSPIbusy = 0;
}

//---------------------------------------------------------------------------------------------
/*
	MyWriteSPI(0x25);						// seconds
	MyWriteSPI(0x29);						// minutes
	MyWriteSPI(0x07);						// hour
	MyWriteSPI(0x14);						// day
	MyWriteSPI(0x02);						// weekday 1=Sön, 2=Mån, 3=Tis, 4=Ons, 5=Tors, 6=Fre, 7=Lör
	MyWriteSPI(0x03);						// month
	MyWriteSPI(0x16);						// year
*/

//---------------------------------------------------------------------------------------------
/*
void DoSetTimeRV3049(void)
{
	unsigned char nTmp, nLoop, szInitData[7];
	int niYear, niMonth, niDay;

	FlagBits.bSPIbusy = 1;

	niYear = (int)BCD2Byte( (lGPSDatum & 0xff0000) >> 16);
	niYear += 2000;
	niMonth = (int)BCD2Byte( (lGPSDatum & 0x00ff00) >> 8);
	niDay = (int)BCD2Byte( (lGPSDatum & 0x0000ff) );
	
	// dow(...) returnerar veckodag, 0=söndag, 1=måndag, 2=tisdag osv.
	nTmp = (unsigned char)dow(niYear, niMonth, niDay);

	// Realtidsklockan vill ha värden mellan 1 och 7
	nTmp += 1;
	
	szInitData[0x00] = (lGPSTid & 0x0000ff);			// sekunder
	szInitData[0x01] = ((lGPSTid & 0x00ff00) >> 8);		// minuter
	szInitData[0x02] = ((lGPSTid & 0xff0000) >> 16);	// timmar

	szInitData[0x03] = (lGPSDatum & 0x0000ff);			// dag
	szInitData[0x04] = nTmp;							// veckodag (1-7, 1=Söndag)
	szInitData[0x05] = ((lGPSDatum & 0x00ff00) >> 8);	// månad
	szInitData[0x06] = ((lGPSDatum & 0xff0000) >> 16);	// år

	OpenSPI(SPI_FOSC_4, MODE_01, SMPMID);

	//RTC_ENABLE = 1;
	
	// SKA WE SÄTTAS TILL NOLL VID INSTÄLLNING AV KLOCKAN ????????????????????????????????????????????
	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Clock-page 
	MyWriteSPI(0b10111110);						// CLK/INT = 1, TD1 TD0 = 01, SROn = 1, EERE = 1, TAR = 1, TE = 1, WE = 0

	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCClock | 0x00);	// Skriv till Clock-page 
	for (nLoop = 0; nLoop < 7; nLoop++)
	{
		MyWriteSPI(szInitData[nLoop]);
	}

	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Clock-page 
	MyWriteSPI(0b10111111);						// CLK/INT = 1, TD1 TD0 = 01, SROn = 1, EERE = 1, TAR = 1, TE = 1, WE = 1
	
	Nop(); 
	Nop();

	//RTC_ENABLE = 0;

	FlagBits.bSPIbusy = 0;
}	

// -----------------------------------------------------------------------------
char DoChangeTickRV3049(unsigned char nT1)
{
	FlagBits.bSPIbusy = 1;

	OpenSPI(SPI_FOSC_4, MODE_01, SMPMID);

	//RTC_ENABLE = 1;

	Nop(); 
	
	MyWriteSPI(RTCWrite | RTCTimer | 0x00);	// Skriv till Timer-page 
	MyWriteSPI(nT1);						// Low	7+1=1Hz
	MyWriteSPI(0);							// High

	Nop(); 
	Nop();

	//RTC_ENABLE = 0;

	CloseSPI();
	
	FlagBits.bSPIbusy = 0;
}	

// -----------------------------------------------------------------------------
char DoStartRV3049(void)
{
	// CKP = 0	Idle state for clock is a low level
	// CKE = 0	Transmit occurs on transition from Idle to active clock state
	// MODE = 01

	FlagBits.bSPIbusy = 1;

	OpenSPI(SPI_FOSC_4, MODE_01, SMPMID);
//	OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);

	//RTC_ENABLE = 1;
    
	
	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control-Page
	MyWriteSPI(0b00100110);						// CONTROL_1, TimerSource=8Hz, Enables Self Recovery, CountdownAutoReload, 
//	MyWriteSPI(0b00100110);						// CONTROL_1, TimerSource=8Hz, Enables Self Recovery, CountdownAutoReload, 
												// Countdown Timer, WE = 0, börja med att ha WE avstängd (Control_1.0)
											
	MyWriteSPI(0b00000010);						// Control_INT, Timer Interrupt generated
	MyWriteSPI(0b00000000);						// Control_INT_FLAG

	MyWriteSPI(0b00000000);						// Control_Status
	MyWriteSPI(0b00000000);						// Control_Reset

	Nop(); 
	Nop();

	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCTimer | 0x00);	// Skriv till Timer-page 
	MyWriteSPI(7);							// Low	7+1=1Hz
	MyWriteSPI(0);							// High
	
	Nop(); 
	Nop();

	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control-Page
	MyWriteSPI(0b00100111);						// Control_1, TimerSource=8Hz, CountdownAutoReload, Countdown Timer, WE = 1
	
	//RTC_ENABLE = 0;

	CloseSPI();

	FlagBits.bSPIbusy = 0;
}	

// -----------------------------------------------------------------------------
/*
char DoInitRV3049(void)
{
	// CKP = 0	Idle state for clock is a low level
	// CKE = 0	Transmit occurs on transition from Idle to active clock state
	// MODE = 01

	FlagBits.bSPIbusy = 1;

	OpenSPI(SPI_FOSC_4, MODE_01, SMPMID);

	RTC_ENABLE = 1;

	MyWriteSPI(RTCWrite | RTCControl | 0x04);	// Skriv till Control-Page
	MyWriteSPI(0b00010000);						// CONTROL_RESET, SysR = 1 ger "System Reset"

	RTC_ENABLE = 0;
	
	Delay(10);

	RTC_ENABLE = 1;

	MyWriteSPI(RTCWrite | RTCControl | 0x03);	// Skriv till Control-Page
	MyWriteSPI(0b00000000);						// CONTROL_Status, återställ PON och SR
	
	RTC_ENABLE = 0;
	
	Delay(10);

	RTC_ENABLE = 1;
	
	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control-Page
	MyWriteSPI(0b00100100);						// CONTROL_1, TimerSource=8Hz, Enables Self Recovery, CountdownAutoReload, 
												// TAR = 1, TE = 0, WE = 0

// Nytt 2016-04-08 ------------------------------------------------------------------------------------------------------
	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control-Page
	MyWriteSPI(0b10011000);						// CONTROL_1, TimerSource=8Hz, Enables Self Recovery, CountdownAutoReload, 
												// TAR = 1, TE = 1, Countdown Timer, WE = 0
	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control-Page
	MyWriteSPI(0b10111100);						// CONTROL_1, TimerSource=8Hz, Enables Self Recovery, CountdownAutoReload, 
												// TAR = 1, TE = 1, Countdown Timer, WE = 0
	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control-Page
	MyWriteSPI(0b10111110);						// CONTROL_1, TimerSource=8Hz, Enables Self Recovery, CountdownAutoReload, 

// Nytt 2016-04-08 ------------------------------------------------------------------------------------------------------
											
	MyWriteSPI(0b00000010);						// Control_INT, Timer Interrupt generated
	MyWriteSPI(0b00000000);						// Control_INT_Flag
	MyWriteSPI(0b00000000);						// Control_Status
	MyWriteSPI(0b00000000);						// Control_Reset

	Nop(); 
	Nop();
	
	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCClock | 0x00);	// Skriv till Clock-page 
	MyWriteSPI(0x25);						// seconds
	MyWriteSPI(0x29);						// minutes
	MyWriteSPI(0x07);						// hour
	MyWriteSPI(0x14);						// day
	MyWriteSPI(0x02);						// weekday 1=Sön, 2=Mån, 3=Tis, 4=Ons, 5=Tors, 6=Fre, 7=Lör
	MyWriteSPI(0x03);						// month
	MyWriteSPI(0x16);						// year

	Nop(); 
	Nop();

	ToggleRTC();
	
	MyWriteSPI(RTCWrite | RTCAlarm | 0x00);	// Skriv till Alarm-page 
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);

	Nop(); 
	Nop();
	
	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCTimer | 0x00);	// Skriv till Timer-page 
	MyWriteSPI(7);							// Low	7+1=1Hz
	MyWriteSPI(0);							// High
	
	Nop(); 
	Nop();

	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x00);	// Skriv till Control-Page
	MyWriteSPI(0b00100111);						// Control_1, TimerSource=8Hz, CountdownAutoReload, Countdown Timer, WE = 1
	
	RTC_ENABLE = 0;

	CloseSPI();

	FlagBits.bSPIbusy = 0;
}	
*/
/*
// -----------------------------------------------------------------------------
void DoReadAllRTC_Regs(void)
{
	char nRegs, nLoop;

	FlagBits.bSPIbusy = 1;

	for (nLoop = 0x00; nLoop < 0x21; nLoop++)
	{
		szUSART_Out[nLoop] = 0xFF;
	}	
	
	OpenSPI(SPI_FOSC_4, MODE_01, SMPMID);

	RTC_ENABLE = 1;

	MyWriteSPI(RTCRead | 0b00000000);		// Control Page
	for (nLoop = 0; nLoop < 5; nLoop++)
	{
		szUSART_Out[nLoop] = MyReadSPI();
		Nop(); Nop();
	}	

	ToggleRTC();

	MyWriteSPI(RTCRead | 0b00001000);		// Clock Page
	for (nLoop = 0x08; nLoop < 0x0F; nLoop++)
	{
		szUSART_Out[nLoop] = MyReadSPI();
		Nop(); Nop();
	}	

	ToggleRTC();

	MyWriteSPI(RTCRead | 0b00010000);		// Alarm Page
	for (nLoop = 0x10; nLoop < 0x17; nLoop++)
	{
		szUSART_Out[nLoop] = MyReadSPI();
		Nop(); Nop();
	}	

	ToggleRTC();

	MyWriteSPI(RTCRead | 0b00011000);		// Timer Page
	for (nLoop = 0x18; nLoop < 0x1A; nLoop++)
	{
		szUSART_Out[nLoop] = MyReadSPI();
		Nop(); Nop();
	}	

	ToggleRTC();

	MyWriteSPI(RTCRead | 0b00100000);		// Temperature Page
	szUSART_Out[0x20] = MyReadSPI();

	RTC_ENABLE = 0;
	
	CloseSPI();
	
	Nop();
	Nop();
	Nop();

	FlagBits.bSPIbusy = 0;
}	

// const rom unsigned char RTCControl = 0b00000000, RTCClock = 0b00001000; RTCAlarm = 0b00010000; RTCTimer = 0b00011000;
// const rom unsigned char RTCTemp    = 0b00100000, RTCEE    = 0b00101000; RTCEECtrl = 0b00110000; RTCRAMM = 0b00111000;

// På Tellus2 (inklusive Rot(...) ) tar det 1840us att göra motsvarande som här tar 760us inklusive läsning av temperatur
// -----------------------------------------------------------------------------
char DoReadRTC_DEBUG(void)
{
	// global: short long lTid, lDatum;
	// global: unsigned char nWeekDay;

	static char nVarv = 0;

	char nCONTROL_INT_FLAG, nCONTROL_STATUS, nReturn;
	unsigned char nSec, nMin, nHour, nDay, nwDay, nWDay, nMon, nYear;
	signed char	nTemp;
	char szDay[6];

	FlagBits.bSPIbusy = 1;

	nReturn = 0;
	nTemp = 0;

//LED_0 = 1;	
	
	OpenSPI(SPI_FOSC_4, MODE_01, SMPMID);

	RTC_ENABLE = 1;		// Nu kan inga klock-register ändras innan RTC_ENABLE går låg
	
	// Läs diverse status-bitar
	// Vlow1: Temp-sensorn fungerar ner till att Vlow1 sätts	Kolla inte temperaturen
	// Vlow2: Klockan fungerar ner till att Vlow2 sätts			Klockan är inte korrekt längre
	// PON: PowerOn-flagga										Initiera klockan
	// SR: SystemReset-flagga									Initiera klockan
	// TF: TimerInterrupt-flagga								Klocktick, resetas i samband med interruptet
	
	MyWriteSPI(RTCRead | RTCControl | 0x02);	// Läs RTCControl
	nCONTROL_INT_FLAG = MyReadSPI();			// CONTROL_INT_FLAG
	nCONTROL_STATUS	  = MyReadSPI();			// CONTROL_STATUS

	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x02);	// Skriv till RTCControl
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);

	ToggleRTC();

	MyWriteSPI(RTCRead | RTCClock | 0x00);	// Läs från klock-registren
	nSec  = MyReadSPI();
	nMin  = MyReadSPI();
	nHour = MyReadSPI();
	nDay  = MyReadSPI();
	nwDay = MyReadSPI();	// weekday: 1=Sön, 2=Mån, 3=Tis, 4=Ons, 5=Tors, 6=Fre, 7=Lör
	nMon  = MyReadSPI();
	nYear = MyReadSPI();

	// Är Vlow1 ok?
	if ((nCONTROL_INT_FLAG & 0x04) == 0)
	{
		ToggleRTC();
		
		MyWriteSPI(RTCRead | RTCTemp | 0x00);	// Läs från temp-sensorn
		nTemp = MyReadSPI();
		nTemp -= 60;
	}

	// Har Vlow2, SRF eller PON triggat?
	if ( ( (nCONTROL_INT_FLAG & 0x18) != 0) || ( (nCONTROL_STATUS & 0x20) != 0) )
	{
		// Som tidigare, initiera om klockan
		nReturn = 1;
	}	
	
	RTC_ENABLE = 0;

	// Skifta fram rätt dag
	nWeekDay = 0x01;
	nWeekDay <<= (nwDay - 1);	// Ledtråd: "- 1" för att söndag ska inte skiftas
	
	lTid = 0L;
	lTid = (char)nHour;
	lTid <<= 8;
	lTid |= (char)nMin;
	lTid <<= 8;
	lTid |= (char)nSec;
	
	lDatum = 0L;
	lDatum = (char)nYear;
	lDatum <<= 8;
	lDatum |= (char)nMon;
	lDatum <<= 8;
	lDatum |= (char)nDay;
	
//	char nSec, nMin, nHour, nDay, nwDay, nWDay, nMon, nYear;
//	lDatum

//LED_0 = 0;

	CloseSPI();
	
/*	switch (nwDay)		// Mon Tue Wed Thu Fri Sat Sun
	{
		case 0x7:
			strcpypgm2ram(szDay, (const rom far char *)"Sat");
		break;
		case 0x6:
			strcpypgm2ram(szDay, (const rom far char *)"Fri");
		break;
		case 0x5:
			strcpypgm2ram(szDay, (const rom far char *)"Thu");
		break;
		case 0x04:
			strcpypgm2ram(szDay, (const rom far char *)"Wed");
		break;
		case 0x03:
			strcpypgm2ram(szDay, (const rom far char *)"Tue");
		break;
		case 0x02:
			strcpypgm2ram(szDay, (const rom far char *)"Mon");
		break;
		case 0x01:
			strcpypgm2ram(szDay, (const rom far char *)"Sun");
		break;
		default:
			strcpypgm2ram(szDay, (const rom far char *)"ERROR");
		break;
	}
*/
/*
	sprintf(szUSART_Out, (const rom far char *)"\t%02X-%02X-%02X %02X:%02X:%02X 0x%02X  %+d'C\t%d\t", 
		nYear, nMon, nDay, nHour, nMin, nSec, nWeekDay, nTemp, (char)(nVarv & 0x01) );
	
	SkrivBuffert(szUSART_Out, 1);
	
	nVarv++;
	
	Nop();
	Nop();

	FlagBits.bSPIbusy = 0;
	// Returnerar 	0 = OK
	//				1 = KLockan behöver initieras om 
	return nReturn;
}	

// -----------------------------------------------------------------------------
// På Tellus2 (inklusive Rot(...) ) tar det 1840us att göra motsvarande som här tar 800us inklusive läsning av temperatur
char DoReadRTC(void)
{
	// Tabell för snabbare omvandling av veckodag (från värde till bitvikt, söndag = 0x01, måndag = 0x02 osv)
	char szWD[8] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40};

	// global: short long lTid, lDatum;
	// global: unsigned char nWeekDay;

	static char nVarv = 0;

	char nCONTROL_INT_FLAG, nCONTROL_STATUS, nReturn;
	unsigned char nSec, nMin, nHour, nDay, nwDay, nMon, nYear;
	signed char	nTemp;

	FlagBits.bSPIbusy = 1;

	nReturn = 0;
	nTemp = 0;

	OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);

	RTC_ENABLE = 1;		// Nu kan inga klock-register ändras innan RTC_ENABLE går låg
	
	// Läs diverse status-bitar
	// Vlow1: Temp-sensorn fungerar ner till att Vlow1 sätts	Kolla inte temperaturen
	// Vlow2: Klockan fungerar ner till att Vlow2 sätts			Klockan är inte korrekt längre, initiera om klockan
	// PON: PowerOn-flagga										Initiera klockan
	// SR: SystemReset-flagga									Initiera klockan
	// TF: TimerInterrupt-flagga								Klocktick, resetas nedan
	
	MyWriteSPI(RTCRead | RTCControl | 0x02);	// Läs RTCControl
	nCONTROL_INT_FLAG = MyReadSPI();			// CONTROL_INT_FLAG
	nCONTROL_STATUS	  = MyReadSPI();			// CONTROL_STATUS

	ToggleRTC();

	// Det här gör att TimerFlag mflr. resetas
	MyWriteSPI(RTCWrite | RTCControl | 0x02);	// Skriv till RTCControl
	MyWriteSPI(0x00);
	MyWriteSPI(0x00);

	ToggleRTC();

	// Har Vlow2 (< 1.3V), SelfRecoveryFlag eller PowerONreset triggat?
	if ( ( (nCONTROL_INT_FLAG & 0x18) != 0) || ( (nCONTROL_STATUS & 0x20) != 0) )
	{
		// Som tidigare, initiering av klockan nödvändig
		nReturn = 1;
	}
	else
	{
		MyWriteSPI(RTCRead | RTCClock | 0x00);	// Läs från klock-registren
		nSec  = MyReadSPI();
		nMin  = MyReadSPI();
		nHour = MyReadSPI();
		nDay  = MyReadSPI();
		nwDay = MyReadSPI();	// weekday: 1=Sön, 2=Mån, 3=Tis, 4=Ons, 5=Tors, 6=Fre, 7=Lör
		nMon  = MyReadSPI();
		nYear = MyReadSPI();
	
		// Är Vlow1 (< 2.1V) ok?
		if ((nCONTROL_INT_FLAG & 0x04) == 0)
		{
			ToggleRTC();
			
			MyWriteSPI(RTCRead | RTCTemp | 0x00);	// Läs från temp-sensorn
			nTemp = MyReadSPI();
			nTemp -= 60;
		}
	}
	
	RTC_ENABLE = 0;

	CloseSPI();
	
	// Mycket snabbare jämfört med att skifta fram rätt värde
	nWeekDay = szWD[nwDay];
	
	// global: short long lTid, lDatum;
	// global: unsigned char nWeekDay;

	lTid = 0L;
	lTid = (char)nHour;
	lTid <<= 8;
	lTid |= (char)nMin;
	lTid <<= 8;
	lTid |= (char)nSec;
	
	lDatum = 0L;
	lDatum = (char)nYear;
	lDatum <<= 8;
	lDatum |= (char)nMon;
	lDatum <<= 8;
	lDatum |= (char)nDay;
	
//LED_0 = 0;
	
	// char nSec, nMin, nHour, nDay, nwDay, nWDay, nMon, nYear;

	sprintf(szUSART_Out, (const rom far char *)"\x0c\r\n %02X-%02X-%02X  %02X:%02X:%02X 0x%02X  %+d'C\r\n\r\n", 
		nYear, nMon, nDay, nHour, nMin, nSec, nWeekDay, nTemp);
	
	SkrivBuffert(szUSART_Out, 1);
	
	nVarv++;
	
	Nop();
	Nop();

	FlagBits.bSPIbusy = 0;

	// Returnerar 	0 = OK
	//				1 = KLockan behöver initieras om 
	return nReturn;
}	


// -----------------------------------------------------------------------------
char DoCheckRV3049tSart(void)
{
	char nReturn;

	FlagBits.bSPIbusy = 1;
	
	OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);

	RTC_ENABLE = 1;

	MyWriteSPI(RTCRead | RTCControl | 0x03);	// CONTROL_STATUS
	nReturn = MyReadSPI();						// Bl.a. PON (bit 5)

	CloseSPI();

	Nop();
	Nop();
	
	FlagBits.bSPIbusy = 0;
}

// -----------------------------------------------------------------------------
char DoResetRTC_INT(void)
{
	char nReturn;
	char nCONTROL_INT_FLAG, nCONTROL_STATUS;

	// IRQ från: 	(4)SRIE = 1, (3)V2IE = 1, (2)V1IE = 1, (1)TIE = 1 i register Control_INT (00000 001)
	// Cleras mha	(4) SRF = 0, (3)V2IF = 0, (2)V1IF = 0, (1) TF = 0 i register Control_INT FLAG (00000 010)
	// och			(4) SR = 0,  (3)V2F = 0,  (2)V1F = 0, i register Control_Status FLAG (00000 011)
	// Flaggan PON (bit 5 i Control Status indikerar Power-ON reset

	FlagBits.bSPIbusy = 1;
	
	OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);

	RTC_ENABLE = 1;

	// Eventuella IRQ sparas i nCONTROL_INT_FLAG och nCONTROL_STATUS
	MyWriteSPI(RTCRead | RTCControl | 0x02);	// Läs RTCControl
	nCONTROL_INT_FLAG = MyReadSPI();			// CONTROL_INT_FLAG
	nCONTROL_STATUS	  = MyReadSPI();			// CONTROL_STATUS
	
	Nop(); 
	Nop();
	
	ToggleRTC();

	MyWriteSPI(RTCWrite | RTCControl | 0x02);	// Skriv till Control-registret 
	MyWriteSPI(0x00);	// Control_INT FLAG
	MyWriteSPI(0x00);	// Control_Status FLAG

	Nop(); 
	Nop();
	
	RTC_ENABLE = 0;

	CloseSPI();

	Nop();
	Nop();
	
	FlagBits.bSPIbusy = 0;
}


*/
//	nTmp = DoReadRTC();
//	if (nTmp != 0)
//	{
//		Nop();
//
//		// Initiera om klockan
//		DoInitRV3049();
//	}
//
//	Nop();
//
//	// Sätt klockticket
//	nTICK = 8;
//	DoChangeTickRV3049(nTICK);
//
//	// Ställ in tid och datum enlig nedan
//	lGPSTid = 0x133030;
//	lGPSDatum = 0x180308;
//
//	Nop();
//	Nop();
//
//	DoSetTimeRV3049();
//
//	// Kontroll
//	nTmp = DoReadRTC();
//	Delay(1000);
//	nTmp = DoReadRTC();
//
//	// DoResetRTC_INT();
//	FlagBits.bTimerIRQ = 0;
//	nLoop = 0;
//	nTICK = 2;
//	//while (1)
//	{	
//		if (FlagBits.bTimerIRQ == 1)
//		{
//			// Interrupt-flaggorna blir lästa och clearade i DoReadRTC()
//			//LED_0 = 1;
////			DoReadRTC();
//			
//			// Interrupt-flaggorna clearas i DoResetRTC_INT()
//			// DoResetRTC_INT();
//			FlagBits.bTimerIRQ = 0;
//			
//			nLoop++;
//			if ( (nLoop % 5) == 0)
//			{
//				nLoop = 0;
//				DoChangeTickRV3049(nTICK);
//				nTICK += 2;
//				if (nTICK > 12)
//				{
//					nTICK = 2;
//				}	
//			} 
//
//			lData = DoReadInc();
//
//			nLo = (lData & 0x000000FF);		// X
//			lData >>= 8;
//			nMLo = (lData & 0x000000FF);	// Y
//			lData >>= 8;
//			nMHi = (lData & 0x000000FF);	// Z
//
//			sprintf(szUSART_Out, (const rom far char *)" %3d\t%3d\t%3d\t%d\r\n\r\n", (signed char)nLo, (signed char)nMLo, (signed char)nMHi, (char)FlagBits.bToggle);
//			SkrivBuffert(szUSART_Out, 1);
//
//			FlagBits.bToggle = !FlagBits.bToggle;
//		}	
//		
//		Nop();
//		Nop();
//	}
//
//	Delay(1000);
//    LATDbits.LATD4 = 1;
//  

