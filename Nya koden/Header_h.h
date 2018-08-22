#ifndef __AMCE_H
#define __AMCE_H

// -----------------------------------------------------------------------------
// **********************************************************************
// TRISA = 0b.xx11.1111; PORTA = 0b.xx00.0000;
// **********************************************************************
// #define 				PORTAbits.RA7	// Används för oscillatorn på lab-kortet
// #define 				PORTAbits.RA6	// Används för oscillatorn på lab-kortet
#define GREEN_LED       LATAbits.LATA5  // D,O 
// #define FREE         LATAbits.LATA4	// D,O
// #define FREE         LATAbits.LATA3	// D,O	
#define VSEL3   		LATAbits.LATA2	// VSEL 3 	
#define VSEL2           LATAbits.LATA1	// VSEL 2, inverterad 
#define VSEL1           LATAbits.LATA0	// VSEL 1


// **********************************************************************
// TRISB = 0b.1110.0111; PORTB = 0b.0000.0000;		Ok
// **********************************************************************
//          			@ PORTB.7       // PGD	
//              		@ PORTB.6   	// PGC
#define Hall_Out		PORTBbits.RB5	// Hall sensor output		
#define NIRQ_Radio      PORTBbits.RB4	// NIRQ_Radio
//#define FREE			PORTBbits.RB3	// D I
#define TCXO_EN			LATBbits.LATB2	// TCXO Enable for Radio
#define INT_Acc 		PORTBbits.RB1	// INT_ACC	
#define nINT_RTC		PORTBbits.RB0	// nINT_RTC


// **********************************************************************
// TRISC = 0b.1000.0000; PORTC = 0b.0000.0000;		Ok
// **********************************************************************
//               		@ PORTC.7   	// RX USART
// 						@ PORTC.6 		// TX USART
// 						@ PORTC.5 		// SDO
//						@ PORTC.4		// SDI
//						@ PORTC.3		// SCK
#define RADIO_SDN		LATCbits.LATC2	// Används för att starta upp kretsen på rätt sätt (VDD)
//                   	@ PORTC.1       // XTAL kristall till MCU 
//              		@ PORTC.0       // XTAL kristall till MCU



// **********************************************************************
// TRISD = 0b.0000.0000, 
// **********************************************************************
// #define FREE			LATDbits.LATD7	// D,O
// #define FREE 		LATDbits.LATD6	// D,O
// #define FREE 		LATDbits.LATD5	// D,O
#define TRX_EN          LATDbits.LATD4	// D,O Radio Chip Select
//#define FREE			LATDbits.LATD3	// D,O
#define ACC_ENABLE		LATDbits.LATD2	// ACC Chip Select
//          		 	@ PORTD.1       // SDA_RTC
//          		 	@ PORTD.0       // SCL_RTC

// **********************************************************************
// Hela PORTE används till LCD:n
// TRISE = 0b.0000.0000; PORTE = 0b.0000.0000;		Ok
// **********************************************************************
//              			@PORTE.3    	// MCLR
// #define FREE             LATEbits.LATE2	// D,O
// #define FREE             LATEbits.LATE1	// D,O
#define RED_LED             LATEbits.LATE0	// D,O

// -----------------------------------------------------------------------------
// Main.c
void SkrivBuffert(char *szUt, char nVal);
void HandleIRQ(void);

// -----------------------------------------------------------------------------
// RTC-RV8803C7
void DoSetTimeRV3049(void);
void ToggleRTC(void);
char DoStartRV3049(void);
char DoInitRV3049(void);
void DoReadAllRTC_Regs(void);
char DoReadRTC(void);
char DoResetRTC_INT(void);
char DoChangeTickRV3049(unsigned char nT1);
char DoCheckRV3049Start(void);

// -----------------------------------------------------------------------------
// ACC-LIS2DW12
void ToggleACC(void);
long int DoReadInc(void);
char DoStartST_ACC(void);

// -----------------------------------------------------------------------------
// Radio-SI4463
void DoResetRadio(void);
void ToggleRadio(void);
void DoStartRadio(void);
unsigned char ReadFromRadio(unsigned char nProp, unsigned char nLen);
unsigned char DoCheckCTSManyTimes1 (void);

// Beacon_21
void GreenLedPulse(int);
void RedLedPulse(int);
void NoLedPulse(int);

// TRX-SI4460
void InitTRXAndGotoSleep(void);
unsigned char DoInitBeacon(void);
unsigned char  DoTurnBeaconPulseOn(void);
unsigned char DoTurnBeaconPulseOff(void);
void DoResetSi4460(void);
char DoSendBB_TX_SetupToSi4460(unsigned char nIndex);

// Behövs ej, samma funktioner kan användas
//void DoInit433Beacon(void);
//void DoTurn433BeaconPulseOn(void);
//void DoTurn433BeaconPulseOff(void);

char DoSendSetupToSi4460(unsigned char nIndex);

//Behövs ej, använder den övre till alla funktioner
//char DoSendUHFSetupToSi4460(unsigned char nIndex);

char DoSendTRXSetupToSi4460(unsigned char nIndex);

char DoSendUtilSetupToSi4460(unsigned char nIndex);
unsigned char DoSendCommandToSi4460(char *szPek, char nLen);
unsigned char DoCheckCTS(void);
unsigned char DoCheckCTSManyTimes(void);
unsigned char DoSpecialCheckCTS(void);
void DoReadFromSi4460(unsigned char nLen);
void DoReadFromRX_FIFOSi4460(unsigned char nLen);
void DoClearIRQSi4460(void);
char DoReadFIFOnumsSi4460(unsigned char nVal);
unsigned char DoReadIRQSi4460(unsigned char nCom, unsigned char nLen);
void DoPutRadioToSleep(void);
unsigned char ReadFrom4460(unsigned char nProp, unsigned char nLen, unsigned char *szData);
unsigned char ReadFRR4460(unsigned char nFRR);
unsigned char CalcRSSI(unsigned char nLastValue);
void DoInitTRX(void);
void DoInitBB_TX(unsigned char nMode);
void DoCloseBB_TX_Session(void);

// -----------------------------------------------------------------------------
// Util.c
unsigned char MyReadSPI(void);
unsigned char SPI1_Exchange8bit(unsigned char data);
unsigned char MyWriteSPI(unsigned char data_out);
int dow(int y, int m, int d);
unsigned char BCD2Byte(unsigned char nIn);
signed int AccDataCalc(unsigned char val_L, unsigned char val_H);
unsigned char ReadEEByte(int nEEAdr);
void Write2EE(const unsigned char nData, const int nAdress);
void Blink(void);
void OSCILLATOR_Initialize(void);
void TestaVSEL(void);
void Delay(unsigned int nDelay);


// AmCe_NReader.c --------------------------------------------------------------
void DoEraseAllNotes(void);
void DoClearEEPROMandRAM(void);
void DoClearEEPROMandRAM_Admin(void);
char DoReadNoteFromEEPROM_Admin(unsigned int nIndex, const char bPrint);
char DoReadNoteFromEEPROM(const unsigned int nIndex, const char bPrint);
void UseReader(void);
void DoCheckAndPay(void);
void DoCashInBox(void);
int DoSaveNote(const char *Note);
int DoSaveNote_Admin(const char *Note);
void DoSaveNoteToEEPROM(unsigned int nIndex);
void DoSaveNoteToEEPROM_Admin(unsigned int nIndex);

// AmCe_util.c -----------------------------------------------------------------
void InitCPU(void);
void UseUtilMenu(void);
unsigned char GetKeyPressed(void);
void Delay19200(void);
void Delay(unsigned int nDelay);
void Blink(void);
unsigned char DoEnterPin(unsigned char nVal, unsigned char nShowResult);
void SetMainMenu(void);
void DoTXcharSW(char nTecken);
void DoTXstringSW(char *szUt);
void SaveCompanyInfoToEEPROM(char *szInfo);
void ReadCompanyInfoFromEEPROM(void);
void GetCompanyInfoFromEEPROM(char nRad, char *szInfo);
void SavePINtoEEPROM(char *szInfo);
void GetPINfromEEPROM(void);
unsigned int DoGetPrintNumber(char nMode);
void DoCommunicate(void);

// AmCe_LCD.c ------------------------------------------------------------------

// AmCe_EEPROM.c ---------------------------------------------------------------
void EE_WriteEnable(void);
void EE_WriteDisable(void);
void EE_ReadFromAdress(unsigned int nAdress);
void EE_DoSaveAbout(void);
void EE_DoWriteWait(void);
char EE_DoSaveNote(const unsigned int nIndex, const char *EEData);
char EE_DoReadNote(const unsigned int nIndex, char *EEData);
char EE_Write16Byte(void);
char HDReadOneByteSPI(char HighAdd, char LowAdd);
char HDByteWriteSPI(char HighAdd, char LowAdd, char Data);
char HDByteReadSPI(char HighAdd, char LowAdd, char *rdptr, char length);
char HDPageWriteSPI(char HighAdd, char LowAdd, char *wrptr, char length);
char HDSequentialReadSPI(char HighAdd, char LowAdd, char *rdptr, char length);
char PutStringSPI(char *wrptr, char length);
void SPIWIPPolling(void);
void WriteEnable(void);
void ReadStatus(void);

// AmCe_CashInBox.c ---------------------------------------------------------------
void UseCashInBox(void);
void DoCashInBox(void);
int DoSaveNote(const char *Note);
int DoSaveNote_Admin(const char *Note);
void DoSaveNoteToEEPROM(unsigned int nIndex);
void DoSaveNoteToEEPROM_Admin(unsigned int nIndex);
char DoReadNoteFromEEPROM_Admin(unsigned int nIndex, const char bPrint);
char DoReadNoteFromEEPROM(const unsigned int nIndex, const char bPrint);
void DoClearEEPROMandRAM_Admin(void);
void DoClearEEPROMandRAM(void);
void DoEraseAllNotes(void);
void DoReadAllDataFromEE(void);
void DoReadNotesFromEE(void);
char DoFindCurrency(void);
char DoFindCurrency_Admin(void);
char DoReadNoteFromEEPROM_Currency(const char *ISO, const char bPrint);
char DoReadNoteFromEEPROM_Currency_Admin(const char *ISO, const char bPrint);
void DoShowCurLists(void);
void DoEraseAllDataFromEE(void);
void DoDisplayNote(char nTecken, char *DataUt, unsigned int nTal);

// AmCe_RTC.c ------------------------------------------------------------------
char DoGetRegisterFromRTC(char nAdress);
char DoSetTimeRTC(char *szTid);
char DoSetDateRTC(char *szDatum);
int DoGetTempRTC(void);
void DoShowTimeAndDate(void);
void DoSetTimeAndDate(void);
void DoGetTimeAndDate(char *szText);
char BCDtoDecimal(char nTal);
void DoGetAndSaveTimeAndDate(void);
void DoGetTimeAndDate4Log(char *szText);

// AmCe_CiB_Print.c ------------------------------------------------------------
void DoPrintTopOfReceipt(char mode_state, char nCompanyInfo);
void DoPrintBottomOfReceipt(char nFoundData);
void DoPrintEraseMessage(char mode_state);
void DoSendTextToUSART2(void);

// AmCe_Flash.c ----------------------------------------------------------------
void ScanMemory(void);
void DoReadNotePointer(void);
void DoSaveNotePointer(void);
void DoSaveNoteToFlash(const char *szSaveNote);
void DoReadAndSendNotes(void);
void DoReadAndSendEvent(void);
void MakeDelay(void);
unsigned char ReadOneByte(unsigned short long lFlashAdress);
unsigned char ReadManyBytes(unsigned char nStringLen, unsigned short long lFlashAdress, unsigned char *DataString);
unsigned char ReadDeviceID(void);
void ReadSector(int nSektor);
void SectorErase(int nSektor);
void ChipErase(void);
void WriteOneByte(unsigned short long lByteAdress, unsigned char nUt);
void Write64Bytes2Flash(unsigned short long lByteAdress);
void WriteStatusReg(unsigned char nUt);
void WriteFlashEnable(void);
void WriteFlashDisable(void);
void MemoryEnable(void);
void MemoryDisable(void);

// -----------------------------------------------------------------------------

  
extern volatile union 
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

//extern const rom unsigned char AccRead, AccWrite;

// -----------------------------------------------------------------------------
extern char nByte_1, nByte_2;
extern char nTimeOutUSART_1, nTimeOutUSART_2;
extern char filter;

// -----------------------------------------------------------------------------
//#pragma udata USART_Buffer0
	extern char szUSART_1[32];
//#pragma udata

//#pragma udata USART_Buffer1
	extern char szUSART_Out[128];
//#pragma udata

//#pragma udata USART_Buffer2
	extern char szUSART_2[32];
//#pragma udata

// extern unsigned char nTICK;

extern const rom unsigned char AccWrite, AccRead, Fifo;
extern const rom unsigned char RTCWrite, RTCRead;

extern const rom unsigned char RTCControl, RTCClock, RTCAlarm, RTCTimer;
extern const rom unsigned char RTCTemp, RTCEE, RTCEECtrl, RTCRAM;

extern short long lTid, lDatum;
extern unsigned char nWeekDay;
extern short long lGPSTid, lGPSDatum;


extern typedef const rom struct MyNewRad
{
	char szRad[17];
};

//extern const rom struct MyNewRad Beacon433Rad[];
//extern const rom struct MyNewRad Beacon151Rad[];
//extern const rom struct MyNewRad RXTXRad[];
//extern const rom struct MyNewRad BitBangRad[];
//extern const rom struct MyNewRad UtilRad[];
//extern const rom struct MyNewRad BBTXRad[];
//extern const rom struct MyNewRad BBRXRad[];
//extern const rom struct MyNewRad BB_RXTX_Rad[];


#endif
