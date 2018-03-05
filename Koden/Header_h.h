#ifndef __AMCE_H
#define __AMCE_H


// -----------------------------------------------------------------------------
// **********************************************************************
// TRISA = 0b.xx11.1111; PORTA = 0b.xx00.0000;
// **********************************************************************
// #define 				PORTAbits.RA7	// Används för oscillatorn på lab-kortet
// #define 				PORTAbits.RA6	// Används för oscillatorn på lab-kortet
// #define 				PORTAbits.RA5	// Används till switch på lab-kortet
// #define RTC_EN		LATAbits.LATA4	// D,O	ACC Enable
// #define EE_EN		LATAbits.LATA3	// D,O	EEPROM Enable
// #define LCD_EN		LATAbits.LATA2	// D,O 	LCD Enable
// #define				PORTAbits.RA1	// Till temperaturkrets på lab-kortet
// #define 				PORTAbits.RA0	// Till spänningsdelare på lab-kortet

// **********************************************************************
// TRISB = 0b.1110.0111; PORTB = 0b.0000.0000;		Ok
// **********************************************************************
// #define N_R4			PORTBbits.RB7	// D,I/O	
// #define N_R3			PORTBbits.RB6	// D,I/O	
// #define N_R2			PORTBbits.RB5	// D,I		
#define ACC_IRQ			PORTBbits.RB4	// I,O/O	INT1
#define RTC_INT			PORTBbits.RB3	// D I
// #define CTS			PORTBbits.RB2	// D,O	används som CTS (9-polig DSUB) på labkortet
// #define RTS			PORTBbits.RB1	// D,I	används som RTS (9-polig DSUB) på labkortet
#define RB0_SWITCH		PORTBbits.RB0	// D,I	används till switch på lab-kortet

// **********************************************************************
// TRISC = 0b.1000.0000; PORTC = 0b.0000.0000;		Ok
// **********************************************************************
// 						@ PORTC.7 		// RX USART
//						@ PORTC.6		// TX USART
// 						@ PORTC.5 		// SDO
//						@ PORTC.4		// SDI
//						@ PORTC.3		// SCK
// #define FREE			LATCbits.LATC2	// ECCP1 Enhanced PWM output, Channel A
// #define FREE			LATCbits.LATC1	// ECCP2 Enhanced PWM output, Channel A
// #define 				LATCbits.LATC0	// Används som räknaringång, ansluten till RTC:s 32768 Hz utgång

// **********************************************************************
// TRISD = 0b.0000.0000, används för LEDar på labkortet
// **********************************************************************
#define LED_7			LATDbits.LATD7	// D,O
#define LED_6			LATDbits.LATD6	// D,O
#define LED_5			LATDbits.LATD5	// D,O
#define LED_4			LATDbits.LATD4	// D,O
#define LED_3			LATDbits.LATD3	// D,O
#define LED_2			LATDbits.LATD2	// D,O
#define LED_1		 	LATDbits.LATD1	// D,O
#define LED_0 			LATDbits.LATD0	// D,O

// **********************************************************************
// Hela PORTE används till LCD:n
// TRISE = 0b.0000.0000; PORTE = 0b.0000.0000;		Ok
// **********************************************************************
// #define FREE				LATEbits.LATE7	// D,O
// #define FREE				LATEbits.LATE6	// D,O
// #define FREE				LATEbits.LATE5	// D,O
// #define FREE			 	LATEbits.LATE4	// D,O
// #define FREE				LATEbits.LATE3	// D,O
// #define FREE				LATEbits.LATE2	// D,O
#define RTC_ENABLE			LATEbits.LATE1	// D,O
#define ACC_ENABLE			LATEbits.LATE0	// D,O

// **********************************************************************
// TRISF = 0b10010000; LATF = 0b01000000;		Ok
// **********************************************************************
//#define SEND_NMEA_0			PORTFbits.RF7	// D,I		Detekterar en TCP-anslutning            ÄNDRAT
/* #define FREE				LATFbits.LATF6	// D,O
#define LCD_RS				LATFbits.LATF5	// D,O
#define MEMORY_ENABLE_0		LATFbits.LATF4	// D,O		(Vill inte fungera som ingång!!!!!!!!!!)	
#define CLOCK_LED			LATFbits.LATF3	// D,O		Led-kort
#define DATA_LED			LATFbits.LATF2	// D,O 		Led-kort
#define STROBE_LED			LATFbits.LATF1	// D,O		Led-kort
// #define 					LATFbits.LATF0	// SAKNAS !!!!!!!!!!!!!!!!!!
*/
// #define TCP_DETECT		PORTFbits.RF7	// D,I		Detekterar en TCP-anslutning
// #define FREE				LATFbits.LATF6	// D,O
// #define FREE				LATFbits.LATF5	// D,O
// #define MEMORY_ENABLE_0	LATFbits.LATF4	// D,O		(Vill inte fungera som ingång!!!!!!!!!!)	
// #define FREE				LATFbits.LATF3	// D,O
// #define MEMORY_ENABLE_0	LATFbits.LATF2	// D,O
// #define TCP_DETECT		LATFbits.LATF1	// D,O		Detekterar en TCP-anslutning
// #define FREE				LATFbits.LATF0	// SAKNAS !!!!!!!!!!!!!!!!!!

// ***********************************************

// **********************************************************************
// TRISG = 0b.0000.0100; PORTG = 0b.0000.0000;		Ok
// **********************************************************************
// #define USART2_MUX1			LATGbits.LATG4	// D,O
// #define USART2_MUX0			LATGbits.LATG3	// D,O
// #define RX_2				LATGbits.LATG2	// RX USART
// #define TX_2				LATGbits.LATG1	// TX USART
// #define USB_DETECT			PORTGbits.RG0	// D,I

// **********************************************************************
// TRISH = 0b.00000000; PORTH = 0b.00000000;	Ok
// **********************************************************************
//#define SEND_NMEA_0			PORTHbits.RH7	// D,O
//#define SEND_NMEA_1			PORTHbits.RH6	// D,I
// #define FREE				LATHbits.LATH5	// D,O
// #define FREE				LATHbits.LATH4	// D,O
// #define FREE				LATHbits.LATH3	// D,O
// #define FREE				LATHbits.LATH2	// D,O
// #define FREE				LATHbits.LATH1	// D,O
// #define FREE				LATHbits.LATH0	// D,I

// **********************************************************************
// TRISJ = 0b.0000.0000; PORTJ = 0b.0000.0000;	Ok
// **********************************************************************
// #define FREE				LATJbits.LATJ7	// D,O
// #define FREE				LATJbits.LATJ6	// D,O
// #define FREE				LATJbits.LATJ5	// D,O
// #define FREE				LATJbits.LATJ4	// D,O
// #define KEYPAD			LATJbits.LATJ3	// D,I
// #define KEYPAD			LATJbits.LATJ2	// D,O
// #define KEYPAD			LATJbits.LATJ1	// D,O
// #define KEYPAD			LATJbits.LATJ0	// D,O

// -----------------------------------------------------------------------------
// Main.c
void SkrivBuffert(char *szUt, char nVal);

// -----------------------------------------------------------------------------
// RTC-RC3049
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
// ACC-ADXL362
void ToggleACC(void);
long int DoReadInc(void);
char DoStartST_ACC(void);

// -----------------------------------------------------------------------------
// Util.c
unsigned char MyReadSPI(void);
unsigned char MyWriteSPI(unsigned char data_out);
int dow(int y, int m, int d);
unsigned char BCD2Byte(unsigned char nIn);

// -----------------------------------------------------------------------------
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
void UseUtilMenu(void);
void InitCPU(void);
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
// Se LCD_h.h

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

// -----------------------------------------------------------------------------
//#pragma udata USART_Buffer0
	extern char szUSART_1[16];
//#pragma udata

//#pragma udata USART_Buffer1
	extern char szUSART_Out[128];
//#pragma udata

//#pragma udata USART_Buffer2
	extern char szUSART_2[16];
//#pragma udata

// extern unsigned char nTICK;

extern const rom unsigned char AccWrite, AccRead, Fifo;
extern const rom unsigned char RTCWrite, RTCRead;

extern const rom unsigned char RTCControl, RTCClock, RTCAlarm, RTCTimer;
extern const rom unsigned char RTCTemp, RTCEE, RTCEECtrl, RTCRAM;

extern short long lTid, lDatum;
extern unsigned char nWeekDay;
extern short long lGPSTid, lGPSDatum;

#endif
