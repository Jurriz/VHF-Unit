#include <p18f46k22.h>
#include <stdlib.h>
#include <stdio.h>
#include <timers.h>
#include <usart.h>
#include <string.h>
#include <spi.h>
#include <delays.h>
#include <adc.h>

#include "Header_h.h"
#include "trx-4460_h.h"



// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// Enligt Microchip gäller följande:
// 			CKP	CKE
// MODE_00	0	1
//		01	0	0
//		10	1	1
//		11	1	0

// 4460		CKP = 0, CKE = 1 -> Mode 00
// 4460 	SMP = 
// SMP	
//	SPI Master mode:
//		1 = Input data sampled at end of data output time
//		0 = Input data sampled at middle of data output time
//	SPI Slave mode:
//		SMP must be cleared when SPI is used in Slave mode
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------

// Ansätter VHF eller UHF filter 
const char filter = 1;      // 0 = UHF, 1 = VHF

void InitTRXAndGotoSleep(void)
{
	// Se till att DoInit151Beacon() gått bra!
	if (DoInitBeacon() != 0xFF)     // 151
	{
		Delay(10);
		DoInitBeacon();         // 151
	}
	DoTurnBeaconPulseOn();      
	DoTurnBeaconPulseOff();     
}

// ------------------------------------------------------------------------------------
// DoInitVHFBeacon() ska direkt följas av DoTurnVHFBeaconPulseOn()
unsigned char DoInitBeacon(void)         // 151
{
	unsigned char nCts, nTmp, nReg, nReturn;
    //int filter = 0; // UHF = 0, VHF = 1;
    //unsigned char FREQ_CTRL_INTE, FREQ_CTRL_FRAC_H, FREQ_CTRL_FRAC_M, FREQ_CTRL_FRAC_L;
	nReturn = 0xFF;

	DoResetSi4460();

	TCXO_EN = 1;
	
	Delay(5);			// Räcker med 2ms 
	
	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);

	nTmp = DoCheckCTSManyTimes();

	nReturn &= nTmp;
	
    // Samma till både VHF och UHF
	DoSendSetupToSi4460(0);		// POWER_UP [0]	startar radion och ställer in den för TCXO

	Delay(15);		// 14ms

	nTmp = DoCheckCTSManyTimes();

	nReturn &= nTmp;

	//  0 = POWER_UP
	// 25 = Frekvensinställningarna
	// 26 = Start TX immediately on channel 0
	// 27 = CHANGE_STATE to SLEEP or STANDBY  (puls off?) 	(se GLOBAL_CLK_CFG:CLK_32K_SEL)
	// 28 = CHANGE_STATE to TX state (puls on?)
	// 29 = Reset av alla IRQ:er
	// 30 = Slår på switchen för VHF
	// 31 = Stänger antennswitchen för att minska strömmen

    
    // Kör de första 15st kommandonen, dessa är samma för både VHF och UHF!
	for (nReg = 1; nReg < 16; nReg++)
	{
		DoSendSetupToSi4460(nReg);
		nTmp = DoCheckCTSManyTimes();
		nReturn &= nTmp;
	}
    
    // Rad 16, Select PLL Synthesizer output divider ratio as a function of frequency band.
    //Vad som egentligen körs: DoSendSetupToSi4460(16);
    
    TRX_EN = 0;
	MyWriteSPI(0x11);   
	MyWriteSPI(0x20);   
	MyWriteSPI(0x02);   
    MyWriteSPI(0x50);
    MyWriteSPI(0x84);
    
    if (filter == 1) //  Beaconbits.UHF_filter = ?, Vhf = 1, UHF = 0;
    {
        MyWriteSPI(0x0D);   // 0x0D för VHF
    } else {
        MyWriteSPI(0x0A);   // 0x0A för UHF
    }
   
    nTmp = DoCheckCTSManyTimes();
    nReturn &= nTmp;
    
//  MyWriteSPI(0x1C);
//  MyWriteSPI(ReadEEByte(FREQ_CTRL_FRAC_H));
//	MyWriteSPI(ReadEEByte(FREQ_CTRL_FRAC_M));
//	MyWriteSPI(ReadEEByte(FREQ_CTRL_FRAC_L));

	// 151.000
//	MyWriteSPI(0x0D);
//	MyWriteSPI(0x89);
//	MyWriteSPI(0xD8);

	//MyWriteSPI(0x17);
	//MyWriteSPI(0xA1);
	//MyWriteSPI(0x20);
	//MyWriteSPI(0xFA);
    
    // Kör de sista, 17:e till 24:e kommandonen, dessa är samma för både VHF och UHF!
    for (nReg = 17; nReg < 25; nReg++)
	{
		DoSendSetupToSi4460(nReg);
		nTmp = DoCheckCTSManyTimes();
		nReturn &= nTmp;
	}
    
    
	// Rad 25 innehåller info som till viss del ligger i EEPROM
	// {0x0C, 0x11, 0x40, 0x08, 0x00, 0x44, 0x0D, 0x89, 0xD8, 0x17, 0xA1, 0x20, 0xFA},		// 25	FREQ_CONTROL_INTE (1) och FREQ_CONTROL_FRAC (3)
	//                                ¯¯¯¯  ¯¯¯¯  ¯¯¯¯  ¯¯¯¯
	// ----------------------------------------
    
	TRX_EN = 0;

    // SET_PROPERTY
	MyWriteSPI(0x11);
	MyWriteSPI(0x40);   //GROUP
	MyWriteSPI(0x08);   // NUM_PROPS
	MyWriteSPI(0x00);   // START_PROP

    // Beaconbits.UHF_filter = ?, Kollar vilken hastighet som skall sättas
    if (filter == 1)
    {
//151.123 Mhz
//   MyWriteSPI(0x44);     
//	 MyWriteSPI(0x0D);   
//   MyWriteSPI(0xFE);
//   MyWriteSPI(0x1C);
    
	MyWriteSPI(ReadEEByte(0));
    MyWriteSPI(ReadEEByte(1));
	MyWriteSPI(ReadEEByte(2));
	MyWriteSPI(ReadEEByte(3));  
    
	// 151.000 Mhz
//  MyWriteSPI(0x44);    
//	MyWriteSPI(0x0D);
//	MyWriteSPI(0x89);
//	MyWriteSPI(0xD8);

    } else {
    MyWriteSPI(ReadEEByte(4));
    MyWriteSPI(ReadEEByte(5));
	MyWriteSPI(ReadEEByte(6));
	MyWriteSPI(ReadEEByte(7)); 
    }
    
    // Resten av värdena är samma för både VHF och UHF
	MyWriteSPI(0x17);
	MyWriteSPI(0xA1);
	MyWriteSPI(0x20);
	MyWriteSPI(0xFA);

	TRX_EN = 1;

	nTmp = DoCheckCTSManyTimes();
	nReturn &= nTmp;

	// ----------------------------------------

	CloseSPI1();
	
	return nReturn;
}	

// ------------------------------------------------------------------------------------
// Startar beacon, en VHF/UHF-puls
unsigned char  DoTurnBeaconPulseOn(void)
{
	unsigned char nTmp, nReturn;
    //int filter = 0;

	nReturn = 0xFF;

	TCXO_EN = 1;		// OBS - Först

	Delay100TCYx(2);	// Ska vara 100us enligt databladet
	
	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);

	//Starta beacon, if(BeaconBits.UHF_FILTER = ?)
	if (filter == 1)		// Beacon mellan 142 och 175 MHz. Filtret ska inte ställas in för UHF (420-525 MHz)
	{
        DoSendSetupToSi4460(30);		// Slår på switchen för VHF
	}
	else								// Beacon mellan 420 och 525 MHz. Filtret ska inte ställas in för VHF (142-175 MHz)
	{
		DoSendSetupToSi4460(31);		// Slår på switchen för UHF
	}
    
    nTmp = DoCheckCTSManyTimes();
    nReturn &= nTmp;
    
	DoSendSetupToSi4460(28);		// CHANGE_STATE to TX state	

	nTmp = DoCheckCTSManyTimes();
	nReturn &= nTmp;

	CloseSPI1();
	
	return nReturn;
}	

// ------------------------------------------------------------------------------------
// Stänger av
unsigned char DoTurnBeaconPulseOff(void)
{
	unsigned char nTmp, nReturn;

	nReturn = 0xFF;

	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);

	DoSendSetupToSi4460(33);		// READY, stänger av TX

	nTmp = DoCheckCTSManyTimes();
	nReturn &= nTmp;

	DoSendSetupToSi4460(32);		// Stänger antennswitchen för att minska strömmen

	nTmp = DoCheckCTSManyTimes();
	nReturn &= nTmp;

	DoClearIRQSi4460();				// Fordras för att gå till sleep/standby

	nTmp = DoCheckCTSManyTimes();
	nReturn &= nTmp;

	DoSendSetupToSi4460(27);		// CHANGE_STATE to SLEEP/STANDBY

	CloseSPI1();

	TCXO_EN = 0;		// OBS - Sist

	return nReturn;
}

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------
// DoInit433Beacon() ska direkt följas av DoTurn433BeaconPulseOn()
// Slängas ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
//void DoInit433Beacon(void)
//{
//	unsigned char nTmp, nReg;
//
//	TCXO_EN = 1;		// OBS - Först
//
//	Delay(5);			// Räcker med 2ms 
//
//	// Reset mha SDN, alla register clearas
//	DoResetSi4460();
//
//	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);
//
//	nTmp = DoCheckCTSManyTimes();
//
//	DoSendUHFSetupToSi4460(0);	// POWER_UP [0]	startar radion och ställer in den för TCXO
//
//	Delay(15);		// 14ms
//
//	nTmp = DoCheckCTSManyTimes();
//
//	// 0-30 totalt
//	//  0 = POWER_UP
//	// 26 = Start TX immediately on channel 0
//	// 27 = CHANGE_STATE to SLEEP or STANDBY  (puls off?) 	(se GLOBAL_CLK_CFG:CLK_32K_SEL)
//	// 28 = CHANGE_STATE to TX state (puls on?)
//	// 29 = Slår på switchen för VHF
//	// 30 = Slår av matningsspänningen till antnn-switchen
//
//	for (nReg = 1; nReg < 26; nReg++)
//	{
//		DoSendUHFSetupToSi4460(nReg);
//		nTmp = DoCheckCTSManyTimes();
//	}
//
//	CloseSPI1();
//}
//
//// ------------------------------------------------------------------------------------
//// Startar beacon en UHF-puls
//void DoTurn433BeaconPulseOn(void)
//{
//	unsigned char nTmp, nReg;
//
//	TCXO_EN = 1;		// OBS - Först
//
//	Delay100TCYx(2);	// Ska vara 100us enligt databladet
//
//	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);
//
//	nTmp = DoCheckCTSManyTimes();	// Väcker upp kretsen mha SPI-kommunikation
//
//	DoSendUHFSetupToSi4460(29);		// Öppnar antennswitchen
//
//	nTmp = DoCheckCTSManyTimes();
//	
//	DoSendUHFSetupToSi4460(28);		// CHANGE_STATE to TX state	
//
//	nTmp = DoCheckCTSManyTimes();
//
//	CloseSPI1();
//}
//
//// ------------------------------------------------------------------------------------
//// Stänger av
//void DoTurn433BeaconPulseOff(void)
//{
//	unsigned char nTmp;
//
////	while (Flaggs.bSPIbusy == 1);
//
//	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);
//
//	DoSendUHFSetupToSi4460(31);		// READY, stänger TX
//
//	nTmp = DoCheckCTSManyTimes();
//
//	DoSendUHFSetupToSi4460(30);		// Stänger antennswitchen
//
//	nTmp = DoCheckCTSManyTimes();
//
//	DoClearIRQSi4460();				// Fordras för att gå till sleep/standby
//	
//	nTmp = DoCheckCTSManyTimes();
//
//	DoSendUHFSetupToSi4460(27);		// CHANGE_STATE to SLEEP or STANDBY
//
//	CloseSPI1();
//		
//	TCXO_EN = 0;		// OBS - Sist
//}
// ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤


// ------------------------------------------------------------------------------------
void DoResetSi4460(void)
{
	// Reset mha SDN, alla register clearas
	RADIO_SDN = 1;

	//Delay10TCYx(35);	// 35x10us=350us, ska vara 300us
	Delay(1);			// 300us

	RADIO_SDN = 0;

	Delay(10);		// 5ms
}	

// ------------------------------------------------------------------------------------
// GET_MODEM_STATUS
// Number: 0x22
/* unsigned char DoGetRSSI(void)
{
	static char nToggle = 1;
	unsigned char nTmp, nCTS, nMODEM_PEND, nMODEM_STATUS, nCURR_RSSI, nLATCH_RSSI, nANT1_RSSI, nANT2_RSSI;
	unsigned char nAFC_FREQ_OFFSET_H, nAFC_FREQ_OFFSET_L;
	int nAFC_Freq_Offset;

//	while (Flaggs.bSPIbusy == 1);

//	MyOpenSPI(SPI_FOSC_4, MODE_00, SMPMID);

	TRX_EN = 0;
	
	MyWriteSPI(0x01);		// PART_INFO
//	MyWriteSPI(0x22);		// GET_MODEM_STATUS
//	MyWriteSPI(0xFF);		// GET_MODEM_STATUS

	TRX_EN = 1;

	nCTS = DoCheckCTSManyTimes();	// 0

	TRX_EN = 0;

	nMODEM_PEND = MyReadSPI();			// 1
	nMODEM_STATUS = MyReadSPI();		// 2
	nCURR_RSSI = MyReadSPI();			// 3
	nLATCH_RSSI = MyReadSPI();			// 4
	nANT1_RSSI = MyReadSPI();			// 5
	nANT2_RSSI = MyReadSPI();			// 6
	nAFC_FREQ_OFFSET_H = MyReadSPI();	// 7
	nAFC_FREQ_OFFSET_L = MyReadSPI();	// 8
	nTmp = MyReadSPI();					// 9

	TRX_EN = 1;

//	MyCloseSPI();	

	nAFC_Freq_Offset = nAFC_FREQ_OFFSET_H;
	nAFC_Freq_Offset <<= 8;
	nAFC_Freq_Offset += nAFC_FREQ_OFFSET_H;

//	GREEN_LED = TRX_0_IN;	// LEDIGT
//	RED_LED = !TRX_0_IN;	// UPPTAGET

//	sprintf(szInfo, (const rom far char *)"\x0C%d", nToggle);
	sprintf(szInfo, (const rom far char *)"\x0C%02X %02X %02X - %03d %03d %03d %03d - %5d - %d\r\n", 
		nCTS, nMODEM_PEND, nMODEM_STATUS, nCURR_RSSI, nLATCH_RSSI, nANT1_RSSI, nANT2_RSSI,
		nAFC_Freq_Offset, nToggle);

	SkrivBuffert(szInfo, 1);

	nToggle = !nToggle;
}	
*/
// ------------------------------------------------------------------------------------
// nIndex anger vilken rad som ska skickas 
// Komplett initiering: 20.7ms
char DoSendSetupToSi4460(unsigned char nIndex)
{
	char nLoop, nLen;
	far rom char *szRadPek;

	szRadPek = (far rom char *)BeaconRad[nIndex].szRad;

	nLen = *szRadPek++;

	TRX_EN = 0;

	for (nLoop = 0; nLoop < nLen; nLoop++)
	{
		Nop();
		Nop();

		SSPBUF = *szRadPek++;
		TMR0L = 0;
		while ( (!SSPSTATbits.BF) && (TMR0L < 5) );	// wait until bus cycle complete 
	}	

	TRX_EN = 1;
}

// ------------------------------------------------------------------------------------
// nIndex anger vilken rad som ska skickas 
// OBS TX
char DoSendBB_TX_SetupToSi4460(unsigned char nIndex)
{
	char nLoop, nTmp, nLen;
	far rom char *szRadPek;

	szRadPek = (far rom char *)BBTXRad[nIndex].szRad;

	nLen = *szRadPek++;

	TRX_EN = 0;

	for (nLoop = 0; nLoop < nLen; nLoop++)
	{
		Nop();
		Nop();

		SSPBUF = *szRadPek++;
		TMR0L = 0;
		while ( (!SSPSTATbits.BF) && (TMR0L < 5) );	// wait until bus cycle complete 
	}	

	TRX_EN = 1;
}


// ------------------------------------------------------------------------------------
// nIndex anger vilken rad som ska skickas 
char DoSendUHFSetupToSi4460(unsigned char nIndex)
{
	char nLoop, nTmp, nLen;
	far rom char *szRadPek;

	szRadPek = (far rom char *)Beacon433Rad[nIndex].szRad;

	nLen = *szRadPek++;

	TRX_EN = 0;

	for (nLoop = 0; nLoop < nLen; nLoop++)
	{
		Nop();
		Nop();

		SSPBUF = *szRadPek++;
		TMR0L = 0;
		while ( (!SSPSTATbits.BF) && (TMR0L < 5) );	// wait until bus cycle complete 
	}	

	TRX_EN = 1;
}	

// ------------------------------------------------------------------------------------
// nIndex anger vilken rad som ska skickas 
char DoSendTRXSetupToSi4460(unsigned char nIndex)
{
	char nLoop, nTmp, nLen;
	far rom char *szRadPek;

	szRadPek = (far rom char *)RXTXRad[nIndex].szRad;

	nLen = *szRadPek++;

	TRX_EN = 0;

	for (nLoop = 0; nLoop < nLen; nLoop++)
	{
		Nop();
		Nop();

		SSPBUF = *szRadPek++;
		TMR0L = 0;
		while ( (!SSPSTATbits.BF) && (TMR0L < 5) );	// wait until bus cycle complete 
	}	

	TRX_EN = 1;
}	

// ------------------------------------------------------------------------------------
//		SSPBUF = Beacon151Rad[nIndex].szRad[nLoop];
//		while (!SSPSTATbits.BF);
//		MyWriteSPI(szRadPek[nLoop]);	// 151 MHz Beacon
//		MyWriteSPI(*szRadPek++);	// 151 MHz Beacon
//		MyWriteSPI(nTmp);
//		MyWriteSPI((unsigned char)Beacon151Rad[nIndex].szRad[nLoop]);	// 151 MHz Beacon
//		MyWriteSPI(*szRadPek++);	// 151 MHz Beacon
//		nTmp = *szRadPek;
// ------------------------------------------------------------------------------------
// nIndex anger vilken rad som ska skickas 
char DoSendUtilSetupToSi4460(unsigned char nIndex)
{
	char nLoop, nTmp, nLen;
	far rom char *szRadPek;

	szRadPek = (far rom char *)UtilRad[nIndex].szRad;

	nLen = *szRadPek++;

	TRX_EN = 0;

	for (nLoop = 0; nLoop < nLen; nLoop++)
	{
		Nop();
		Nop();

		SSPBUF = *szRadPek++;
		TMR0L = 0;
		while ( (!SSPSTATbits.BF) && (TMR0L < 5) );	// wait until bus cycle complete 
	}	

	TRX_EN = 1;
}

// ------------------------------------------------------------------------------------
//		SSPBUF = Beacon151Rad[nIndex].szRad[nLoop];
//		while (!SSPSTATbits.BF);
//		MyWriteSPI(szRadPek[nLoop]);	// 151 MHz Beacon
//		MyWriteSPI(*szRadPek++);	// 151 MHz Beacon
//		MyWriteSPI(nTmp);
//		MyWriteSPI((unsigned char)Beacon151Rad[nIndex].szRad[nLoop]);	// 151 MHz Beacon
//		MyWriteSPI(*szRadPek++);	// 151 MHz Beacon
//		nTmp = *szRadPek;
// ------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------
unsigned char DoSendCommandToSi4460(char *szPek, char nLen)
{
	char nReturn, nLoop;

	TRX_EN = 0;

	for (nLoop = 0; nLoop < nLen; nLoop++)
	{
		MyWriteSPI(szPek[nLoop]);
	}

	TRX_EN = 1;
	
	return nLoop;
}	

// ------------------------------------------------------------------------------------
unsigned char DoCheckCTS(void)
{
	char nReturn, nLoop;

	TRX_EN = 0;
	
	MyWriteSPI(0x44);		
	nReturn = MyReadSPI();	// Läs CTS
	
	TRX_EN = 1;
	
	return nReturn;
}

// ------------------------------------------------------------------------------------
unsigned char DoCheckCTSManyTimes(void)
{
	char nReturn, nLoop;

	nLoop = 0;

	TRX_EN = 0;
	MyWriteSPI(0x44);		// Läs CTS
	Nop();
	nReturn = MyReadSPI();
	TRX_EN = 1;

	while ( (nReturn != 0xFF) && (nLoop < 20) )
	{
		TRX_EN = 0;
		MyWriteSPI(0x44);		// Läs CTS
		Nop();
		nReturn = MyReadSPI();
		TRX_EN = 1;
		nLoop++;
	}

	if (nReturn != 0xFF)
	{
		RED_LED = 1;
		Delay(20);
		RED_LED = 0;
	}	
    TRX_EN = 0;
	return nReturn;
}


// ------------------------------------------------------------------------------------
unsigned char DoSpecialCheckCTS(void)
{
	char nReturn, nLoop;

	// Det är förutsatt att TRX_EN dragits låg före anropet

	MyWriteSPI(0x44);		// Läs CTS
	Nop();
	nReturn = MyReadSPI();

	nLoop = 0;
	// Max 10 försök annars returneras det som finns!
	while ( (nReturn != 0xFF) && (nLoop < 10) )
	{
		MyWriteSPI(0x44);		// Läs CTS
		Nop(); 
		nReturn = MyReadSPI();
		Nop(); 
		Nop();
	}
	
	return nReturn;
}

// ------------------------------------------------------------------------------------
void DoReadFromSi4460(unsigned char nLen)
{
	unsigned char nLoop, n1, n2, n3;
	
	TRX_EN = 0;
	
	Nop(); Nop(); 

	n1 = MyReadSPI();
	Nop();
	n2 = MyReadSPI();

	Nop();

	TRX_EN = 1;
}	

// ------------------------------------------------------------------------------------
void DoReadFromRX_FIFOSi4460(unsigned char nLen)		// 0x2A = 42byte
{
	unsigned char nLoop;
	char szRadioBuf[];
    
	TRX_EN = 0;

	Nop(); Nop(); 

	MyWriteSPI(0x77);

	for (nLoop = 0; nLoop < nLen; nLoop++)
	{
		szRadioBuf[nLoop] = MyReadSPI();
		Nop();
	}

	Nop();

	TRX_EN = 1;
}	


// ----------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                                     |-----------------| Ligger till grund för CRC
//                                                                                                                    |--| Command ligger sist före CRC
// Korrekt paket: 0x55 0x55 0x55 0xDA 0x91 0xD3 0x1B 0x1B 0x1B 0x1B 0x1B 0x00 0x00 0x01 0xFF 0x00 0x04 0x03 0xDA 0xDF 0x12 0xF9 0x82 0x55 0x55 
//                | Preamble   | |Sync        | | Ingår alltid exakt lika                  | LenH Len  |------------------- CRC  CRC | Preamble...
// ----------------------------------------------------------------------------------------------------------------------------------------------------
// Command: 0x12 = Remote Data DownLoad, 0x34 = DropOff, 0x56 = Test av ID, 0x78 = Reset av SMS-pekare
// ----------------------------------------------------------------------------------------------------------------------------------------------------

// Letar först fram "0x1B 0x1B 0x00 0x00 0x01 0xFF 0x00 0x04" och sedan ID, Command och CRC







//char DoUncodeMessage(unsigned char nLen)
//{
//	unsigned short long lID;
//    
//	char nReturn, nTmp, nCommand, nLoop, nStart, nCRC, nPek;
//	int lCRC;
//
//	char data;
//	char nB;
//
////	PrintSeparator();
////	PrintSingleByte(nLen);
//	
//	nCommand = 0;
//	nLoop = 0;
//	nStart = 0;
//	nCRC = 0;
//	nPek = 0;
//
//	while ( (nLoop < nLen) && (nStart != 0x3F) && (nCRC != 0xFF) )
//	{
//		while ( (nLoop < nLen) && (nStart != 0x3F) )
//		{
//			// Leta fram den sista (av fem) 0x1B
//			while ( (szRadioBuf[nLoop] == 0x1B) && (nLoop < nLen) )
//			{
//				nLoop++;
//			}
//			//  0x1B 0x00 0x00 0x01 0xFF 0x00 0x04 0x03 0xDA 0xDF 0x12 0xF9 0x82 0x55 0x55 0x55 0x55 0x55 0xDA 0x91 0xD3 0x1B 0x1B 0x1B 0x1B 0x1B 0x00 0x00 0x01 0xFF 0x00 0x04 0x03 0xDA 0xDF 0x12 0xF9 0x82 0x55 0x55 0x55 0x55 0x55 0xDA 0x91 0xD3 0x1B 0x1B 0x1B 0x1B 
//
////			if ( (szRadioBuf[nLoop] == 0x1B) && (nStart == 0x00) )
////			{
////				nStart |= 0x01;
////			}
////			else if ( (szRadioBuf[nLoop] == 0x1B) && (nStart == 0x01) )
////			{
////				nStart |= 0x02;
////			}
////			else if 
//			if ( (szRadioBuf[nLoop] == 0x00) && (nStart == 0x00) )
//			{
//				nStart |= 0x01;
//			}
//			else if ( (szRadioBuf[nLoop] == 0x00) && (nStart == 0x01) )
//			{
//				nStart |= 0x02;
//			}
//			else if ( (szRadioBuf[nLoop] == 0x01) && (nStart == 0x03) )
//			{
//				nStart |= 0x04;
//			}
//			else if ( (szRadioBuf[nLoop] == 0xFF) && (nStart == 0x07) )
//			{
//				nStart |= 0x08;
//			}
//			else if ( (szRadioBuf[nLoop] == 0x00) && (nStart == 0x0F) )
//			{
//				nStart |= 0x10;
//			}
//			else if ( (szRadioBuf[nLoop] == 0x04) && (nStart == 0x1F) )
//			{
//				nStart |= 0x20;
//			}
//			else
//			{
//				nStart = 0x00;
//			}	
//	
//			nLoop++;
//		}
//
//		// PrintSingleByte(nStart);
//
//		if (nStart == 0x3F)
//		{
//			// PrintSingleByte(nLoop);
//
//			lCRC = INITIAL_VALUE;
//			// szRadioBuf[nLoop] innehåller första tecknet, szRadioBuf[nLoop + 5] det sista
//			nPek = nLoop;
//			nTmp = nLoop + 5;
//			while (nLoop <= nTmp) 
//			{
//				data = szRadioBuf[nLoop];
//		
//				Nop();
//				for(nB = 0; nB < 8; nB++)
//				{
//					if( (lCRC ^ data) & 1)
//					{
//						lCRC = (lCRC >> 1) ^ POLY_REVERSE;
//					}
//					else
//					{
//						lCRC >>= 1;
//					}
//		 			data >>= 1;
//		 		}
//		 		nLoop++;
//			}
//
//			if (lCRC == 0x0000)
//			{
//				nCRC = 0xFF;
//			}
//			else
//			{
//				// Fortsätt leta!
//				nStart = 0x00;
//				nCRC = 0x00;
//			}	
//		}		
//
//		// PrintSingleByte(nCRC);
//		// PrintSeparator();
//	}
//
//	nReturn = 0;
//
//	if ( (nCRC == 0xFF) && (nStart == 0x3F) )
//	{
//		lID = (unsigned short long)(szRadioBuf[nPek] & 0x0000ff);
//		lID <<= 8;
//		lID |= (unsigned short long)(szRadioBuf[nPek + 1] & 0x0000ff);
//		lID <<= 8;
//		lID |= (unsigned short long)(szRadioBuf[nPek + 2] & 0x0000ff);
//
//		nCommand = szRadioBuf[nPek + 3];
//
//		nReturn = DecodeData(lID, nCommand);
//
//		// PrintSeparator();
//		// PrintSingleByteHex(nReturn);
//
//		//	1: Högt ID, Remote Data			0x12	Skarpt ID
//		//	2: Lågt ID, Drop Off			0x34	Skarpt ID
//		//	3: Högt eller lågt ID, test		0x56	Skarpt ID
//		//	4: ID 252639, Remote Data		0x12	ID: 200001 för test
//		//	5: ID 147363, Drop Off			0x34	ID: 200001 för test
//		//	6: Fel ID men annars korrekt CRC
//		//	8: Högt ID, Remote Data			0x78	Skarpt ID, återställer pekaren som talar om vad som ska skickas via SMS
//		//	9: Högt ID, Remote Data			0x9A	Skarpt ID, skicka via UHF
//	}
//
//	//	PrintSeparator();
//	//	PrintSingleByte(nStart);
//	//	PrintSingleByte(nCRC);
//	
//	return nReturn;
//}
//
//
//// ------------------------------------------------------------------------------------
//char DoFindTestMessage(unsigned char nLen)
//{
//	char nLoop, nSync;    
//    
//	// Leta fram "0x04 0x03 0xDA 0xDF 0x12 0xF9 0x82"
//	nLoop = 0;
//	nSync = 0x03;
//	while ( (nLoop < nLen) && (nSync != 0xFF) )
//	{
//		if ( (szRadioBuf[nLoop] == 0x03) && (nSync == 0x01) )
//		{
//			nSync |= 0x04;
//		}
//		else if ( (szRadioBuf[nLoop] == 0xDA) && (nSync == 0x03) )
//		{
//			nSync |= 0x08;
//		}
//		else if ( (szRadioBuf[nLoop] == 0xDF) && (nSync == 0x07) )
//		{
//			nSync |= 0x10;
//		}
//		else if ( (szRadioBuf[nLoop] == 0x12) && (nSync == 0x0F) )
//		{
//			nSync |= 0x20;
//		}
//		else if ( (szRadioBuf[nLoop] == 0xF9) && (nSync == 0x1F) )
//		{
//			nSync |= 0x40;
//		}
//		else if ( (szRadioBuf[nLoop] == 0x82) && (nSync == 0x3F) )
//		{
//			nSync |= 0x80;
//		}
//		else
//		{
//			nSync = 0x03;
//		}	
//			
//		nLoop++;
//	}
//	
//	return nSync;
//}









// ----------------------------------------------------------------------------------------------------------------------------------------------------
//                                                                                                     |-----------------| Ligger till grund för CRC
//                                                                                                                    |--| Command ligger sist före CRC
// Korrekt paket: 0x55 0x55 0x55 0xDA 0x91 0xD3 0x1B 0x1B 0x1B 0x1B 0x1B 0x00 0x00 0x01 0xFF 0x00 0x04 0x03 0xDA 0xDF 0x12 0xF9 0x82 0x55 0x55 
//                | Preamble   | |Sync        | | Ingår alltid exakt lika                  | LenH Len  |------------------- CRC  CRC | Preamble...
// ----------------------------------------------------------------------------------------------------------------------------------------------------
// Command: 0x12 = Remote Data DownLoad, 0x34 = DropOff, 0x56 = Test av ID, 0x78 = Reset av SMS-pekare
// ----------------------------------------------------------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------
/* void DoReadIRQSi4460(unsigned char nCom, unsigned char nLen)
{
	unsigned char nLoop, nPek, nTmp;
	
	TRX_EN = 0;
	Nop(); Nop(); 

	nPek = 50;
	MyWriteSPI(nCom);

	nTmp = 0;
	while (nTmp != 0xFF)
	{
		nTmp = DoCheckCTS(1);	// (1) betyder: läs en enskild CTS, TRX_EN togglas i funktionen, TRX_EN == 1 vid avslut
		Nop();
		Nop();
	}

	TRX_EN = 0;
	
	for (nLoop = 0; nLoop < nLen; nLoop++, nPek++)
	{
		szRadioBuf[nPek] = MyReadSPI();

		Nop();
	}

	Nop();

	TRX_EN = 1;
} */


// ------------------------------------------------------------------------------------
void DoClearIRQSi4460(void)
{
	//char nLoop, nTmp;

	TRX_EN = 0;

	Nop(); Nop(); 

	MyWriteSPI(0x20);	// Send the interrupt status command
	MyWriteSPI(0x00);	// Clear PH_CLR_PEND
	MyWriteSPI(0x00);	// Clear MODEM_CLR_PEND
	MyWriteSPI(0x00);	// Clear CHIP_CLR_PEND
	Nop();

	TRX_EN = 1;
}

// --------------------------------------------------------------------------------------------------
char DoReadFIFOnumsSi4460(unsigned char nVal)
{
	unsigned char nLoop, nReturn, nRX, nTX;
	
	nRX = 0;
	
	TRX_EN = 0;
	
	MyWriteSPI(0x15);
	MyWriteSPI(nVal);
	Nop();	
	TRX_EN = 1;
	
	Nop();	
	
	TRX_EN = 0;

	MyWriteSPI(0x44);		// Läs CTS
	
	Nop();
	nReturn = MyReadSPI();
	while (nReturn != 0xFF)
	{
		TRX_EN = 1;
		Nop();
		TRX_EN = 0;
		MyWriteSPI(0x44);		// Läs CTS
		Nop();
		nReturn = MyReadSPI();
		Nop();
	}

	if (nReturn == 0xFF)
	{
		nRX = MyReadSPI();
	
		nTX = MyReadSPI();
	}
	
	TRX_EN = 1;

	return nRX;
}

// --------------------------------------------------------------------------------------------------
unsigned char DoReadIRQSi4460(unsigned char nCom, unsigned char nLen)
{
	unsigned char nReturn, nPek, nLoop;
    char szRadioBuf[]; //?

	nReturn = 0;
	
	TRX_EN = 0;

	Nop(); Nop(); 

	MyWriteSPI(nCom);
	MyWriteSPI(0xFF);
	MyWriteSPI(0xFF);
	MyWriteSPI(0xFF);

	Nop();
	TRX_EN = 1;
	Nop();
	TRX_EN = 0;
	Nop();

	if (DoSpecialCheckCTS() == 0xFF)
	{
		nReturn = 0xFF;
		nPek = 0x20;
		for (nLoop = 0; nLoop < nLen; nLoop++, nPek++)
		{
			szRadioBuf[nPek] = MyReadSPI();
	
			Nop();
		}
	}

	Nop();

	TRX_EN = 1;

	return nReturn;
}	

// ------------------------------------------------------------------------------------
// RÖR FÖR FAN INTE DET HÄR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// I = 26uA
// ------------------------------------------------------------------------------------
void DoPutRadioToSleep(void)
{	
	unsigned char nTmp;

	TCXO_EN = 1;

	// Reset mha SDN, alla register clearas
	DoResetSi4460();

	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);

	nTmp = DoCheckCTSManyTimes();

	DoSendUtilSetupToSi4460(0);			// POWER_UP [0]	startar radion och ställer in den för TCXO

	Delay(20);							// ca. 14ms

	nTmp = DoCheckCTSManyTimes();

	DoSendUtilSetupToSi4460(1);			// Sätter alla I/O:s för sleep/standby ("Input and output drivers disabled")

	nTmp = DoCheckCTSManyTimes();

	DoSendUtilSetupToSi4460(2);			// Reset av alla IRQ

	nTmp = DoCheckCTSManyTimes();

	DoSendUtilSetupToSi4460(3);			// CHANGE_STATE to SLEEP or STANDBY

	// nTmp = DoCheckCTSManyTimes();	// Utan bortkommentering drar kortet 1.54mA

	TCXO_EN = 0;

	CloseSPI1();	
}
// ------------------------------------------------------------------------------------
// I = 26uA
// RÖR FÖR FAN INTE DET HÄR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------
// char DoSendVHFSetupToSi4460(unsigned char nIndex);
// char DoSendUHFSetupToSi4460(unsigned char nIndex);
// char DoSendTXRXSetupToSi4460(unsigned char nIndex);
// ------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
// SPI
// If the CTS is polled over the SPI bus, the host MCU should pull the NSEL pin low. This should be followed by
// sending out the 0x44 Read command ID and providing an additional eight clock pulses on the SCLK pin. The radio
// will provide the CTS byte on its SDO pin during the additional clock pulses. If the CTS byte is 0x00, then the
// response is not yet ready and the host MCU should pull up the NSEL pin and repeat the procedure from the
// beginning as long as the CTS becomes 0xFF. If CTS is 0xFF, then the host MCU should keep the NSEL pin low
// and provide as many clock cycles on the SCLK pin as the data to be read out requires. The radio will clock out the
// requested data on its SDO pin during the additional clock pulses.
// -------------------------------------------------------------------------------------------------------------------
// GPIO1
// Any of the GPIOs can be configured for monitoring the CTS. GPIOs can be configured to go high or low if the chip
// completes the command. The function of the GPIOs can be changed by the GPIO_PIN_CFG command. By
// default, GPIO1 is set as “High when command completed, low otherwise” after Power On Reset. Therefore, this pin
// can be used for monitoring the CTS right after Power On Reset to know when the chip is ready to boot up.

// ------------------------------------------------------------------------------------
unsigned char ReadFrom4460(unsigned char nProp, unsigned char nLen, unsigned char *szData)
{
	unsigned char nLoop, nReturn, nTmp, nString[10];

	nReturn = 0;

	TCXO_EN = 1;			// Bör redan vara igång

	TRX_EN = 0;
	MyWriteSPI(nProp);		// Vad ska läsas
	TRX_EN = 1;
	
	Nop();
	
	TRX_EN = 0;
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

	if (nTmp == 0xFF)
	{
		nReturn = 1;
		
		nLoop = 0;
		while (nLoop < nLen)
		{
			szData[nLoop] = MyReadSPI();
			nLoop++;
		}	
	}		

	TRX_EN = 1;

//	MyCloseSPI();

	return nReturn;
}	

// ------------------------------------------------------------------------------------
// Läs FastResponsReg nFRR
unsigned char ReadFRR4460(unsigned char nFRR)
{
	unsigned char nReturn;

	nReturn = 0;

	TCXO_EN = 1;			// Ska redan vara igång

	TRX_EN = 0;

	MyWriteSPI(nFRR);		// Vad ska läsas
	nReturn = MyReadSPI();

	TRX_EN = 1;

	return nReturn;
}	

// ------------------------------------------------------------------------------------
// Den lägsta gränsen för mottagning ligger runt 15-16
unsigned char CalcRSSI(unsigned char nLastValue)
{
	char nLoop;
	int nRssi;
    int nRssiArray[];

	// TEST3 = 1;

//	PrintCharacter('R');

	if (nLastValue < 5)
	{
		nLastValue = 5;
	}	

	if (nLastValue > 50)
	{
		nLastValue = 50;
	}	

	// index 0=1, index 1=2, index 2=3
	nRssi = (int)0;
	for (nLoop = 0; nLoop < 3; nLoop++)
	{
		nRssiArray[nLoop] = nRssiArray[nLoop + 1];
		nRssi += nRssiArray[nLoop];
	}
	// Ger nRssiArray[3] = nLastValue;
	nRssiArray[nLoop] = nLastValue;
	nRssi += nRssiArray[nLoop];

	nRssi = (int)0;
	for (nLoop = 0; nLoop < 4; nLoop++)
	{
		nRssi += nRssiArray[nLoop];
		// PrintSingleByte(nRssiArray[nLoop]);
	}

	// Meddelvärde
	nRssi >>= 2;	// Dela med 4
	
	nRssi &= 0x3F;

	// Lite över medelvärdet
	nRssi += 1;

	// TEST3 = 0;

	return (unsigned char)nRssi;
}








//
//// ------------------------------------------------------------------------------------
//// Kod från main-loopen från RXTX-projektet
//char DoListenTRX(unsigned char nShowLED)
//{
//	static unsigned char nKaos = 0;
//	int nAFC;
//	unsigned char szData[12];
//	unsigned char nTmp, nLen, nLoop, nRSSI_In, nFRR, nError, nResult;
//
//	// TEST2 = 1;
//
//	TCXO_EN = 1;
//
//	Delay100TCYx(2);	// Ska vara 100us enligt databladet
//
//	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);
//
//	DoClearIRQSi4460();
//
//	DoSendTRXSetupToSi4460(36);
//	nTmp = DoCheckCTSManyTimes();
//
//	DoSendTRXSetupToSi4460(37);
//	nTmp = DoCheckCTSManyTimes();
//
//	// Nu är radion i RX-state!
//	Delay(5);
//	
//	// Kolla RSSI
//	ReadFrom4460(0x22, 8, (unsigned char *)szData);
//
//	nRSSI_In = szData[2];
//
//	// nAFC = (int)szData[6];	// AFC_FREQ_OFFSET[15:8]
//	// nAFC <<= 8;
//	// nAFC += (int)szData[7];	// AFC_FREQ_OFFSET[7:0]
//	// PrintSingleInt(nAFC);
//
//	if (nShowLED != 0)
//	{
//		PrintCR_LF();
//		PrintSingleByte(nRSSI_In);		// Uppmätt RSSI-värde	// Mycket bättre
//		PrintSingleByte(nMedelRSSI);	// Beräknat medelvärde för RSSI
//		PrintSeparator();
//	}
//
//	// --------------------------------------------------------------------------------------------------------------------------------------------------------------
//	// 0A				0	0						0					0				1			0						1						0
//	// MODEM_PEND:		X	POSTAMBLE_DETECT_PEND	INVALID_SYNC_PEND	RSSI_JUMP_PEND	RSSI_PEND	INVALID_PREAMBLE_PEND	PREAMBLE_DETECT_PEND	SYNC_DETECT_PEND
//	//
//	// 13				0	0						0					1				0			0						1						1				
//	// MODEM_STATUS:	X	POSTAMBLE_DETECT		INVALID_SYNC		RSSI_JUMP		RSSI		INVALID_PREAMBLE		PREAMBLE_DETECT			SYNC_DETECT
//	// --------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//	WDI = 1; 
//	Nop(); 
//	Nop(); 
//	WDI = 0;
//
//	// ----------------------------------------------------------------------------------------------------------------------------------------------------
//	//                                                                                                     |-----------------| Ligger till grund för CRC
//	//                                                                                                                    |--| Command ligger sist före CRC
//	// Korrekt paket: 0x55 0x55 0x55 0xDA 0x91 0xD3 0x1B 0x1B 0x1B 0x1B 0x1B 0x00 0x00 0x01 0xFF 0x00 0x04 0x03 0xDA 0xDF 0x12 0xF9 0x82 0x55 0x55 
//	//                | Preamble   | |Sync        | | Ingår alltid exakt lika                  | LenH Len  |------------------- CRC  CRC | Preamble...
//	// ----------------------------------------------------------------------------------------------------------------------------------------------------
//	// Command: 0x12 = Remote Data DownLoad, 0x34 = DropOff, 0x56 = Test av ID, 0x78 = Reset av SMS-pekare
//	// ----------------------------------------------------------------------------------------------------------------------------------------------------
//	
//	nError = 0;
//	nResult	= 0;
//	
//	// Är det värt att lyssna?
////	if (nRSSI_In != 0)
//	if (nRSSI_In >= nMedelRSSI)
//	{
//		nKaos = 0;
//		
//		if (nShowLED != 0)
//		{
//			GUL_LED = LED_ON;
//		}
//			
//		Nop();
//		Nop();
//
//		// I gamla Tellus letades synk under max 133ms (320 bitar = 40 byte = rimligt)
//		// 2400bps, preamble och synk bör hittas inom 20 byte, 20x10x1/2400=83ms
//		nLoop = 0;
//		nLen = DoReadFIFOnumsSi4460(NO_FIFO_ERASE);
//		while ( (nLen < 3) && (nLoop < 40) )
//		{
//			nLen = DoReadFIFOnumsSi4460(NO_FIFO_ERASE);
//			Delay(3);
//			nLoop++;
//		}
//
//		if (nShowLED != 0)
//		{
//			PrintSingleByte(nLen);
//		}	
//
//		if (nLen > 2)
//		{
//			nLoop = 0;
//			nLen = DoReadFIFOnumsSi4460(NO_FIFO_ERASE);
//			while ( (nLen < 45) && (nLoop < 100) )
//			{
//				nLen = DoReadFIFOnumsSi4460(NO_FIFO_ERASE);
//				Delay(3);
//				nLoop++;
//			}
//		}	
//
//		if (nShowLED != 0)
//		{
//			PrintSingleByte(nLen);
//		}	
//
//		// Här kan RX stängas av !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//			
//		DoSendTRXSetupToSi4460(39);		// READY, stänger RX
//		nTmp = DoCheckCTSManyTimes();
//
//		// PrintSingleByte(nLen);
//
//		// 2400bps
//		// Den del av meddelande som ligger först i FIFO:n (17 byte)
//		//                                         0x1B 0x1B 0x1B 0x1B 0x1B 0x00 0x00 0x01 0xFF 0x00 0x04 0x03 0xDA 0xDF 0x12 0xF9 0x86 
//		// Resten, helt meddelande
//		// 0x55 0x55 0x55 0x55 0x55 0xDA 0x91 0xD3 0x1B 0x1B 0x1B 0x1B 0x1B 0x00 0x00 0x01 0xFF 0x00 0x04 0x03 0xDA 0xDF 0x12 0xF9 0x82 
//		// Slutet
//		// 0x55 0x55 0x15 0x55 0x55 0xDA 0x91 0xD3 0x1B 0x1B 0x1F 0x1B 0x1B 0x00 0x00 0x01 0xFF 0x00 0x04 0x03 0xDA 0xDF ..............  
//		// I det här fallet ska sista byten i meddelandet vara 0x82
//		// Eftersom paketformatet inte stöds av Silabs läser radion tills FIFO:n är full
//
//		// Om (nLen > 0) OCH (nLen <= 64) finns i alla fall preamble och synk-ord
//		// Ett komplett meddelande utan preamble och synk (redan läst av radion men utanför FIFO:n) är 17 byte 
//		if (nLen >= 18)
//		{
//			// Följande bör vara omöjligt men ändå...
//			if (nLen > 64)
//			{
//				nLen = 64;
//			}	
//
//			// Läs nLen tecken från radions FIFO
//			DoReadFromRX_FIFOSi4460(nLen);		// Läs så många tecken det finns i bufferten
//
//			nResult = DoUncodeMessage(nLen);
//
//			if (nShowLED != 0)
//			{
//				if (nResult != 0x00)
//				{
//					PrintCharacter('Y');
//					GREEN_LED = LED_ON;
//				}	
//				else
//				{
//					PrintCharacter('N');
//					RED_LED = LED_ON;
//				}
//	
//				PrintSeparator();
//				PrintSingleByteHex(nResult);
// 			}	
//		}
//		// Tröskelvärdet ska ändras om det inte finns någonting i meddelandet
//		else if (nLen == 0)
//		{
//			nError = 1;
//			
//			if (nShowLED != 0)
//			{
//				RED_LED = LED_ON;
//			}	
//		}	
//	}
//	else
//	{
//		nKaos++;
//
//		nError = 1;
//				
//		if (nShowLED != 0)
//		{
//			RED_LED = LED_ON;
//		}	
//	}
//
//	//GUL_LED = LED_OFF;
//
//	// I princip onödig eftersom DoInit151Beacon() innehåller en Reset mha SDN, alla register clearas
//	DoReadFIFOnumsSi4460(RX_FIFO_ERASE);	// Töm RX-bufferten
//
//	DoSendTRXSetupToSi4460(39);		// READY, stänger RX
//	nTmp = DoCheckCTSManyTimes();
//
//	DoClearIRQSi4460();				// Fordras för att gå till sleep/standby
//
//	nTmp = DoCheckCTSManyTimes();
//	DoSendTRXSetupToSi4460(40);		// READY, CHANGE_STATE to SLEEP or STANDBY
//
//	CloseSPI1();
//
//	TCXO_EN = 0;
//
//	// Beräkna medelvärde vid misslyckad körning (ingenting nyttigt har hittats)
//	if (nError == 1)
//	{
//		if (nKaos >= 10)	// Motsvarar 5 minuter
//		{
//			nKaos = 0;
//			if (nRSSI_In > 5)
//			{
//				nRSSI_In -= 3;
//			}
//		}
//		
//		nMedelRSSI = CalcRSSI(nRSSI_In);
//	}
//
//	// TEST2 = 0;
//		
//	return nResult;
//}			













// ------------------------------------------------------------------------------------
void DoInitTRX(void)
{
	unsigned char nTmp, nReg;

	// TEST1 = 1;

	DoResetSi4460();

	TCXO_EN = 1;
	
	Delay(5);			// Räcker med 2ms 
	
	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);

	nTmp = DoCheckCTSManyTimes();

	DoSendTRXSetupToSi4460(0);	// POWER_UP [0]	startar radion och ställer in den för TCXO

	Delay(15);

	nTmp = DoCheckCTSManyTimes();

	DoClearIRQSi4460();

	for (nReg = 1; nReg < 36; nReg++)
	{
		DoSendTRXSetupToSi4460(nReg);
		nTmp = DoCheckCTSManyTimes();
	}

	// Fungerar tydligen inte om det inte är ett DIRECT_RX-projekt
	// DoSendTRXSetupToSi4460(41);		// Ställ in GPIO_0 och 1 för TX_CLK respektive TX_DATA
	// nTmp = DoCheckCTSManyTimes();

	CloseSPI1();

	// TEST1 = 0;

	Nop();
}

// ------------------------------------------------------------------------------------

/*
3.2.1. Sending Commands
The behavior of the radio can be changed by sending commands to the radio (e.g. changing the power states, start
packet transmission, etc.). The radio can be configured through several so called “properties”. The properties hold
radio configuration settings, such as interrupt settings, modem parameters, packet handler settings, etc. The
properties can be set and read via API commands.
Most of the commands are sent from the host MCU to the radio, and the host MCU does not expect any response
from the chip. There are other commands that are used to read back a property from the chip, such as checking
the interrupt status flags, reading the transmit/receive FIFOs, etc. This paragraph gives a detailed overview about
the simple commands (commands with no response); the Get response to a command paragraph describes the
SPI transactions of getting information from the radio (commands with response).
All the commands that are sent to the radio should have the same structure. After pulling down the NSEL pin of the
radio, the command ID should be sent first. The commands may have up to 15 input parameters with the exception
of START_RX and START_TX, which may have 64 bytes input. The number of input parameters varies depending
on the actual command. The NSEL pin must be pulled high when the transaction finishes.

The command IDs are listed in Table 2.
Table 2. Commands
Command ID Name Input Parameters
0x02 POWER_UP Power-up device and mode selection. Modes include operational
function.
0x04 PATCH_IMAGE Loads image from NVM/ROM into RAM
0x00 NOP No operation command
0x01 PART_INFO Reports basic information about the device
0x10 FUNC_INFO Returns the Function revision information of the device
0x11 SET_PROPERTY Sets the value of a property
0x12 GET_PROPERTY Retrieve a property's value
0x13 GPIO_PIN_CFG Configures the GPIO pins
0x15 FIFO_INFO Provides access to transmit and receive FIFO counts and reset
0x17 IRCAL Calibrate IR
0x20 GET_INT_STATUS Returns the interrupt status byte
0x33 REQUEST_DEVICE_STATE Request current device state
0x34 CHANGE_STATE Update state machine entries
0x14 GET_ADC_READING Retrieve the results of possible ADC conversions
0x16 GET_PACKET_INFO Returns information about the last packet received
0x18 PROTOCOL_CFG Sets the chip up for specified protocol
0x21 GET_PH_STATUS Returns the packet handler status
0x22 GET_MODEM_STATUS Returns the modem status byte
0x23 GET_CHIP_STATUS Returns the chip status
0x36 RX_HOP Fast RX to RX transitions for use in frequency hopping systems
0x50 FRR_A Returns Fast response register A
0x51 FRR_B Returns Fast response register B
0x53 FRR_C Returns Fast response register C
0x57 FRR_D Returns Fast response register D
0x44 READ Command buffer Returns Clear to send (CTS) value and the result of the previouscommand
0x66 TX_FIFO_WRITE Writes TX data buffer (max. 64 bytes)
0x77 RX_FIFO_READ Reads RX data buffer (max. 64 bytes)
0x31 START_TX Switches to TX state and starts packet transmission.
0x32 START_RX Switches to RX state.

After the radio receives a command, it processes the request. During this time, the radio is not capable of receiving
a new command. The host MCU has to poll the radio and identify when the next command can be sent. The CTS
(Clear to Send) signal shows the actual status of the command buffer of the radio. It can be monitored over the SPI
port or on GPIOs, or the chip may generate an interrupt if it is ready to receive the next command. These three
options are detailed below.
*/

/*
TEST1
DoInitTRX	Varje gång eller en gång fram till beacon att används
60ms

TEST2
DoListenTRX
9.00ms / 141ms / 405ms

TEST3
CalcRSSI
0.31ms / 0
*/












//// ------------------------------------------------------------------------------------
//// Funktion som avkodar data (szUSART), korrigerar om inverterad, skiftar mm.
//// En CRC-check görs. Om allt är OK returneras det kommando som sänts från handenheten.
//// char DecodeData(char nLen, unsigned short long nMyID)
//char DecodeData(unsigned short long nReceivedID, char nCommand)
//{
//	unsigned char nDecodeLoop, nPek, nNoll, nReturn;
//	unsigned char nLika, nTest, nSkifta, nInvertera, nVarv;
//	unsigned short long nReadID, lHighID, lLowID;
//	unsigned short nMyCRC;
//	unsigned short nRotate;
//	char *nAdress;
//
//	nReturn = 0;
//
//	// nMyCRC = CalcCrc16_ID((unsigned short long) nMyID);
//	// lHighID = (nMyID + (unsigned short long)nMyCRC);
//	// lLowID = (nMyID - (unsigned short long)nMyCRC);
//	// nMyCRC = CalcCrc16_ID((unsigned short long) nMyID);
//
//	// Hämta högt ID från EEPROMet
//	//lHighID = (nMyID + (unsigned short long)nMyCRC);
//	lHighID	= (ReadEEByte(HIGH_ID_H) & 0x0000ff);
//	lHighID <<= 8;
//	lHighID |= (ReadEEByte(HIGH_ID_M) & 0x0000ff);
//	lHighID <<= 8;
//	lHighID |= (ReadEEByte(HIGH_ID_L) & 0x0000ff);
//
//	// Hämta lågt ID från EEPROMet
//	//lLowID = (nMyID - (unsigned short long)nMyCRC);
//	lLowID	= (ReadEEByte(LOW_ID_H) & 0x0000ff);
//	lLowID <<= 8;
//	lLowID |= (ReadEEByte(LOW_ID_M) & 0x0000ff);
//	lLowID <<= 8;
//	lLowID |= (ReadEEByte(LOW_ID_L) & 0x0000ff);
//
//	Nop();
//
//	/*
//	0: Oläsbart
//	1: Högt ID, Remote Data			0x12	Skarpt ID
//	2: Lågt ID, Drop Off			0x34	Skarpt ID
//	3: Högt eller lågt ID, test		0x56	Skarpt ID
//	4: ID 252639, Remote Data		0x12	ID: 200001 för test
//	5: ID 147363, Drop Off			0x34	ID: 200001 för test
//	6: Fel ID men annars korrekt CRC
//	8: Högt ID, Remote Data			0x78	Skarpt ID, återställer pekaren som talar om vad som ska skickas via SMS
//	9: Högt ID, Remote Data			0x9A	Skarpt ID, skicka via UHF
//	*/
//
//	// T2H-5880: 206902 - 224836 - 188968
//
//	// ID 200001 har 252639 och 147363
//
//	// Defaultvärde
//	nReturn = 6;
//	if (nCommand == 0x12)					// Remote Data
//	{
//		if (nReceivedID == lHighID)
//		{
//			nReturn = 1;
//		}
//		else if (nReceivedID == 0x03DADF)	// 252639 (200001)
//		{
//			nReturn = 4;
//		}
//	}
//	else if (nCommand == 0x34)				// Drop Off
//	{
//		if (nReceivedID == lLowID)
//		{
//			nReturn = 2;
//		}
//		else if (nReceivedID == 0x023FA3)	// 147363 (200001)
//		{
//			nReturn = 5;
//		}
//	}
//	else if (nCommand == 0x56)				// Test
//	{
//		if ( (nReceivedID == lLowID) || (nReceivedID == lHighID) )
//		{
//			nReturn = 3;
//		}
//	}
//	else if (nCommand == 0x78)				// Reset av SMS-pekare
//	{
//		if (nReceivedID == lHighID)
//		{
//			nReturn = 8;
//		}
//	}
//	else if (nCommand == 0x9A)				// Skicka data via UHF, samma id som VHF-nerladdning
//	{
//		if (nReceivedID == lHighID)
//		{
//			nReturn = 9;
//		}
//	}
//	else
//	{
//		nReturn = 6;						// Korrekt meddelande men fel ID
//	}
//
//	Nop();
//
//	if (nReturn == 6)
//	{
//		Nop();
//		Nop();
//	}
//
//	return nReturn;
//}
//













// ------------------------------------------------------------------------------------
// TX 2400/1200 Manchester
void DoInitBB_TX(unsigned char nMode)
{
	unsigned char nTmp, nReg;

	//TRIS_TRX_0 = 1;	// Ingång
	//TRIS_TRX_1 = 1;	// Utgång

	// Reset mha SDN, alla register clearas
	DoResetSi4460();

	TCXO_EN = 1;		// OBS - Först

	Delay(5);			// Räcker med 2ms 

	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);

	nTmp = DoCheckCTSManyTimes();

	DoSendBB_TX_SetupToSi4460(0);	// POWER_UP [0]	startar radion och ställer in den för TCXO

	Delay(15);		// 14ms

	nTmp = DoCheckCTSManyTimes();

	for (nReg = 1; nReg < 26; nReg++)
	{
		DoSendBB_TX_SetupToSi4460(nReg);
		nTmp = DoCheckCTSManyTimes();
	}

	CloseSPI1();

	//TRIS_TRX_0 = 1;	// Ingång
	//TRIS_TRX_1 = 0;	// Utgång

}

// ------------------------------------------------------------------------------------
void DoCloseBB_TX_Session(void)
{
	unsigned char nTmp;

	//TRIS_TRX_0 = 1;	// Ingång
	//TRIS_TRX_1 = 1;	// Utgång

	OpenSPI1(SPI_FOSC_4, MODE_00, SMPMID);

	DoSendBB_TX_SetupToSi4460(31);	// Slår av matningsspänningen till antenn-switchen, samtliga "Input and output drivers disabled."
	nTmp = DoCheckCTSManyTimes();

	DoSendTRXSetupToSi4460(39);		// READY, stänger RX
	nTmp = DoCheckCTSManyTimes();

	DoClearIRQSi4460();				// Fordras för att gå till sleep/standby

	nTmp = DoCheckCTSManyTimes();
	DoSendTRXSetupToSi4460(40);		// READY, CHANGE_STATE to SLEEP or STANDBY

	CloseSPI1();

	TCXO_EN = 0;		// OBS - Sist
}	





















//// ------------------------------------------------------------------------------------
//void SendManchester(unsigned char szUHF)
//{
//	unsigned char nBit, nSendData;
//	
//	nSendData = szUHF;
//	for (nBit = 0; nBit < 8; nBit++)
//	{
//		if ( (nSendData & 0x01) == 1)
//		{
//			while (TRX_CLK == 0);
//	
//			TRX_DATA = 1;
//	
//			while (TRX_CLK == 1);
//	
//			while (TRX_CLK == 0);
//	
//			TRX_DATA = 0;
//	
//			while (TRX_CLK == 1);
//		}
//		else
//		{
//			while (TRX_CLK == 0);
//	
//			TRX_DATA = 0;
//	
//			while (TRX_CLK == 1);
//	
//			while (TRX_CLK == 0);
//	
//			TRX_DATA = 1;
//	
//			while (TRX_CLK == 1);
//		}
//		// Skifta ett steg höger
//		nSendData >>= 1;
//	}
//}
//
//// ------------------------------------------------------------------------------------
//void SendManchesterInv(unsigned char szUHF)
//{
//	unsigned char nBit, nSendData;
//	
//	nSendData = szUHF;
//	for (nBit = 0; nBit < 8; nBit++)
//	{
//		if ( (nSendData & 0x01) == 0)
//		{
//			while (TRX_CLK == 0);
//	
//			TRX_DATA = 1;
//	
//			while (TRX_CLK == 1);
//	
//			while (TRX_CLK == 0);
//	
//			TRX_DATA = 0;
//	
//			while (TRX_CLK == 1);
//		}
//		else
//		{
//			while (TRX_CLK == 0);
//	
//			TRX_DATA = 0;
//	
//			while (TRX_CLK == 1);
//	
//			while (TRX_CLK == 0);
//	
//			TRX_DATA = 1;
//	
//			while (TRX_CLK == 1);
//		}
//		// Skifta ett steg höger
//		nSendData >>= 1;
//	}
//}
//
//// ------------------------------------------------------------------------------------
//unsigned int NewCrc16(unsigned char Data_buffer[], unsigned int nCrcLen, char nInOut)		// In=1, Out=0
//{
//	unsigned int nCRCStart, nCRCStop, nCRCI;
//	unsigned int crc = INITIAL_VALUE;
//	unsigned char data;
//	unsigned char b;
//
//	if (nCrcLen < 290)
//	{
//		if (nInOut == 1)				// RX, räkna CRC på hela meddelandet INKLUSIVE CRC
//		{
//			nCRCStart = 6;					// beräkningsunderlaget börjar på sz...[6]
//			nCRCStop = (nCrcLen + 6);		// Payload till sz...[6 + nCrcLen]
//			nCRCStop += 2;					// + 2 byte CRC
//		}
//		else							// TX, räkna CRC på meddelandet EXKLUSIVE CRC
//		{
//			nCRCStart = 13;
//			nCRCStop = (13 + nCrcLen);		// beräkningsunderlaget börjar på sz...[13] och slutar [13 + nCrcLen]
//		}
//	
//		for(nCRCI = nCRCStart; nCRCI < nCRCStop; nCRCI++)	// 
//		{
//			data = Data_buffer[nCRCI];
//				
//			for(b = 0; b < 8; b++)
//			{
//				if( (crc ^ data) & 1)
//				{
//					crc = (crc >> 1) ^ POLY_REVERSE;
//				}
//				else
//				{
//					crc >>= 1;
//				}
//	 			data >>= 1;
//	 		}
//	 		
//		}
//		return crc;
//	}
//	else
//	{
//		return 0xffff;
//	}
//}























// -----------------------------------------------------------------------------------------------------------------------------------------
// The Manchester code is based on transitions; a “0” is encoded as a low-to-high transition, a “1” is encoded as a high-to-low transition
// Klockan går dubbelt så snabbt som data-hastigheten
// -----------------------------------------------------------------------------------------------------------------------------------------
/* void DoSendOne(void)
{
	while (TRX_CLK == 0);

	TRX_DATA = 0;

	while (TRX_CLK == 1);

	while (TRX_CLK == 0);

	TRX_DATA = 1;

	while (TRX_CLK == 1);
}
*/
// -----------------------------------------------------------------------------------------------------------------------------------------
// The Manchester code is based on transitions; a “0” is encoded as a low-to-high transition, a “1” is encoded as a high-to-low transition
// -----------------------------------------------------------------------------------------------------------------------------------------
/* void DoSendZero(void)
{
	while (TRX_CLK == 0);

	TRX_DATA = 1;

	while (TRX_CLK == 1);

	while (TRX_CLK == 0);

	TRX_DATA = 0;

	while (TRX_CLK == 1);
}
*/

