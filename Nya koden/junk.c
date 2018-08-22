/*

-----------------------------------------------------------------------------
Start
$GPRMC,063002.889,V,,,,,0.00,0.00,010316,,,N*46
$GPGGA,063002.889,,,,,0,0,,,M,,M,,*46
$GPGSA,A,1,,,,,,,,,,,,,,,*1E

-----------------------------------------------------------------------------
.
.
.
$GPRMC,063020.000,A,5936.0751,N,01511.9275,E,0.00,100.06,010316,,,A*6C
$GPGGA,063020.000,5936 .0751,N,01511.9275,E,1,11,0.73,83.9,M,30.5,M,,*5C
$GPGSA,A,3,01,22,11,23,03,19,06,25,12,31,17,,1.10,0.73,0.82*01

$GPRMC,063021.000,A,5936.0751,N,01511.9275,E,0.00,100.06,010316,,,A*6D
$GPGGA,063021.000,5936.0751,N,01511.9275,E,1,11,0 .73,83.9,M,30.5,M,,*5D
$GPGSA,A,3,01,22,11,23,03,19,06,25,12,31,17,,1.10,0.73,0.82*01

$GPRMC,063022.000,A,5936.0751,N,01511.9275,E,0.00,100.06,010316,,,A*6E
$GPGGA,063022.000,5936.0751 ,N,01511.9275,E,1,11,0.73,83.9,M,30.5,M,,*5E
$GPGSA,A,3,01,22,11,23,03,19,06,25,12,31,17,,1.10,0.73,0.82*01
.
.
.
-----------------------------------------------------------------------------

*/

// -----------------------------------------------------------------------------
unsigned char DoStartAccII(void)
{
	unsigned char nReturn, nSlask;
	
	OpenSPI(SPI_FOSC_64, MODE_11, SMPMID);	// Farten minskad för att försäkra sig om att skrivinstruktionen ska gå fram (0x03)

	ACC_ENABLE = 0;

	Delay (5);

	Nop(); Nop(); Nop();

	MyWriteSPI(AccWrite | Multi | 0x1F);	// 0001.1111	Skriv till adressen 0x1F och inkrementera adressen

	// -----------------------------------------------------------------------------
	MyWriteSPI(0xC0);						// 1100.0000	Temperature sensor (T) enable. Default value: 00 (00: T disabled; 11: T enabled)
//	MyWriteSPI(0x00);						// 0000.0000	Temperature sensor (T) disable. Default value: 00 (00: T disabled; 11: T enabled)
	
											// Aktivera kretsen genom att gå ur "Power Down Mode"
	// -----------------------------------------------------------------------------
	MyWriteSPI(0xA7);						// 0010.1111	Skriv till adressen 0x20, CTRL_REG1 "HR / Normal / Low-power mode (10 Hz)" mm

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x09);						// 0000.0000	Skriv till adressen 0x21, CTRL_REG2 
	
	// -----------------------------------------------------------------------------
	MyWriteSPI(0x40);						// 0100.000x	Skriv till adressen 0x22, CTRL_REG3, I1_AOI1 = 1

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x00);						// 1000.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 2g				 15.63 mg/digit
//	MyWriteSPI(0x80);						// 1000.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 2g				 15.63 mg/digit
//	MyWriteSPI(0x82);						// 1000.0010	Skriv till adressen 0x23, CTRL_REG4 +/- 2g Self-test 0	 15.63 mg/digit
//	MyWriteSPI(0x84);						// 1000.0100	Skriv till adressen 0x23, CTRL_REG4 +/- 2g Self-test 1	 15.63 mg/digit
//	MyWriteSPI(0x90);						// 1001.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 4gm 			 31.25 mg/digit
//	MyWriteSPI(0xA0);						// 1010.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 8g				 62.50 mg/digit
//	MyWriteSPI(0xB0);						// 1011.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 16g				188.68 mg/digit

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x08);						// 00xx.1000	Skriv till adressen 0x24, CTRL_REG5 LIR_INT1=1 interrupt request latched

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x00);						// 0000.1000	Skriv till adressen 0x25, CTRL_REG6

	ToggleACC();

	// -----------------------------------------------------------------------------
	MyWriteSPI(AccWrite | Multi | 0x30);	// 0011.0000 INT1_CFG

	MyWriteSPI(0x10);				// 0000.0001			Skriv till adressen 0x32 INT1_THS

	MyWriteSPI(0x00);				// 0000.0001			Skriv till adressen 0x33 INT1_DURATION

	ToggleACC();

	// -----------------------------------------------------------------------------
	MyWriteSPI(AccRead | 0x26);	// 0010.0110	Läs adressen 0x26 REFERENCE/DATACAPTURE
	
	nSlask = MyReadSPI();

	ToggleACC();

	MyWriteSPI(AccWrite | Multi | 0x32);	// 0011.0010	
	
	// Läs "Dummy Registret"
	MyWriteSPI(AccRead | 0x0F);		// 1000.0000 | 0000.1111

	nReturn = MyReadSPI();

	Nop(); Nop(); Nop();

	ACC_ENABLE = 1;

	CloseSPI();
	
	return nReturn;
}


// -----------------------------------------------------------------------------
unsigned char DoStartAcc(void)
{
	unsigned char nReturn;
	
	OpenSPI(SPI_FOSC_64, MODE_11, SMPMID);	// Farten minskad för att försäkra sig om att skrivinstruktionen ska gå fram (0x03)

	ACC_ENABLE = 0;

	Delay (5);

	Nop(); Nop(); Nop();

	MyWriteSPI(AccWrite | Multi | 0x1F);	// 0001.1111	Skriv till adressen 0x1F och inkrementera adressen

	// -----------------------------------------------------------------------------
	MyWriteSPI(0xC0);						// 1100.0000	Temperature sensor (T) enable. Default value: 00 (00: T disabled; 11: T enabled)
//	MyWriteSPI(0x00);						// 0000.0000	Temperature sensor (T) disable. Default value: 00 (00: T disabled; 11: T enabled)
	
											// Aktivera kretsen genom att gå ur "Power Down Mode"
	// -----------------------------------------------------------------------------
//	MyWriteSPI(0x1F);						// 0001.1111	Skriv till adressen 0x20, CTRL_REG1 "HR / Normal / Low-power mode ( 1 Hz)" mm
	MyWriteSPI(0x2F);						// 0010.1111	Skriv till adressen 0x20, CTRL_REG1 "HR / Normal / Low-power mode (10 Hz)" mm
//	MyWriteSPI(0xA7);						// 0010.1111	Skriv till adressen 0x20, CTRL_REG1 "HR / Normal / Low-power mode (10 Hz)" mm

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x00);						// 0000.0000	Skriv till adressen 0x21, CTRL_REG2 
	
	// -----------------------------------------------------------------------------
	MyWriteSPI(0x40);						// 0100.000x	Skriv till adressen 0x22, CTRL_REG3, I1_AOI1 = 1

	// -----------------------------------------------------------------------------
//	MyWriteSPI(0x00);						// 1000.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 2g				 15.63 mg/digit
//	MyWriteSPI(0x80);						// 1000.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 2g				 15.63 mg/digit
//	MyWriteSPI(0x82);						// 1000.0010	Skriv till adressen 0x23, CTRL_REG4 +/- 2g Self-test 0	 15.63 mg/digit
//	MyWriteSPI(0x84);						// 1000.0100	Skriv till adressen 0x23, CTRL_REG4 +/- 2g Self-test 1	 15.63 mg/digit
//	MyWriteSPI(0x90);						// 1001.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 4gm 			 31.25 mg/digit
	MyWriteSPI(0xA0);						// 1010.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 8g				 62.50 mg/digit
//	MyWriteSPI(0xB0);						// 1011.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 16g				188.68 mg/digit

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x08);						// 00xx.1000	Skriv till adressen 0x24, CTRL_REG5 LIR_INT1=1 interrupt request latched
//	MyWriteSPI(0x00);						// 00xx.0000	Skriv till adressen 0x24, CTRL_REG5 LIR_INT1=0 interrupt request not latched

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x00);						// 0000.1000	Skriv till adressen 0x25, CTRL_REG6

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x00);						// 0000.0000	Skriv till adressen 0x26, REFERENCE/DATACAPTURE
//	MyWriteSPI(0x10);						// 0000.1000	Skriv till adressen 0x26, REFERENCE/DATACAPTURE

	ToggleACC();

	MyWriteSPI(AccWrite | 0x30);			// 0011.0000
	
	// -----------------------------------------------------------------------------
//	MyWriteSPI(0x3F);						// 0011.1111	Skriv till adressen 0x30 INT1_CFG
	MyWriteSPI(0x6A);						// 0011.1111	Skriv till adressen 0x30 INT1_CFG
//	MyWriteSPI(0x0A);						// 0011.1111	Skriv till adressen 0x30 INT1_CFG

	ToggleACC();

	MyWriteSPI(AccWrite | Multi | 0x32);	// 0011.0010	
	
	// -----------------------------------------------------------------------------
//	MyWriteSPI(0x10);				// 0000.0001			Skriv till adressen 0x32 INT1_THS
	MyWriteSPI(0x08);				// 0000.0001			Skriv till adressen 0x32 INT1_THS

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x01);				// 0000.0001			Skriv till adressen 0x33 INT1_DURATION
	MyWriteSPI(0x00);				// 0000.0001			Skriv till adressen 0x33 INT1_DURATION

	ToggleACC();

	// Läs "Dummy Registret"
	MyWriteSPI(AccRead | 0x0F);		// 1000.0000 | 0000.1111

	nReturn = MyReadSPI();

	Nop(); Nop(); Nop();

	ACC_ENABLE = 1;

	CloseSPI();
	
	return nReturn;
}

// -----------------------------------------------------------------------------
unsigned char DoReadTemp(void)
{
	unsigned char nLoop, nSlask, nTL, nTH, nStatus, nReturn;
	int nIntSlask, nOrord;
	nStatus = 0;
	nLoop = 0;
	nSlask = 0;
	nTL = 0;
	nTH = 0;

	OpenSPI(SPI_FOSC_64, MODE_11, SMPMID);	

	ACC_ENABLE = 0;

	Nop();

	MyWriteSPI(AccRead | 0x07);	// 0000.1100	Läs från adressen 0x07, STATUS_REG_AUX

	nStatus = MyReadSPI();

	ToggleACC();

	MyWriteSPI(AccRead | Multi | 0x0C);	// 0000.1100	Läs från adressen 0x0C, flera läsningar ger auto inc på adressen

	nTL = MyReadSPI();

	nTH = MyReadSPI();

	Nop(); 	Nop(); Nop();

	ACC_ENABLE = 1;

	CloseSPI();

	nIntSlask = nTH;
	nIntSlask <<= 8;
	nIntSlask |= nTL;
	nOrord = nIntSlask;
	nIntSlask >>= 6;
	nIntSlask &= 0x00FF;
	nReturn = (unsigned char)nIntSlask;

	sprintf(szUSART_1_Out, (const rom far char *)"%08b\t%d\t%08b\t%d\t%08b\t%d\t%08b\r\n", nTH, (signed char)nTH, nTL, (signed char)nTL, nReturn, (signed char)nReturn, nStatus);
//	sprintf(szUSART_1_Out, (const rom far char *)"%016b - %08b %d %08b - %08b %d\r\n", nOrord, nTH, (signed char)nTH, nTL, nReturn, (signed char)nReturn);
//	sprintf(szUSART_1_Out, (const rom far char *)"%d %d\r\n", nTH, (signed char)nTH);
	SkrivBuffert(szUSART_1_Out);

	Nop();
	Nop();
	nReturn = (int) nTH;
	nReturn <<= 8;
	nReturn |= nTL;
	nReturn >>= 6;

//	return (unsigned char)((~nReturn) + 1);
//	return (unsigned char)nReturn;

	return nTH;
}	

08:53
// -----------------------------------------------------------------------------
unsigned char DoStartAcc(void)
{
	unsigned char nReturn;
	
	OpenSPI(SPI_FOSC_64, MODE_11, SMPMID);	// Farten minskad för att försäkra sig om att skrivinstruktionen ska gå fram (0x03)

	ACC_ENABLE = 0;

	Delay (5);

	Nop(); Nop(); Nop();

	MyWriteSPI(AccWrite | Multi | 0x1F);	// 0001.1111	Skriv till adressen 0x1F och inkrementera adressen

	// -----------------------------------------------------------------------------
	MyWriteSPI(0xC0);						// 1100.0000	Temperature sensor (T) enable. Default value: 00 (00: T disabled; 11: T enabled)
//	MyWriteSPI(0x00);						// 0000.0000	Temperature sensor (T) disable. Default value: 00 (00: T disabled; 11: T enabled)
	
											// Aktivera kretsen genom att gå ur "Power Down Mode"
	// -----------------------------------------------------------------------------
	MyWriteSPI(0x1F);						// 0001.1111	Skriv till adressen 0x20, CTRL_REG1 "HR / Normal / Low-power mode ( 1 Hz)" mm
//	MyWriteSPI(0x2F);						// 0010.1111	Skriv till adressen 0x20, CTRL_REG1 "HR / Normal / Low-power mode (10 Hz)" mm
//	MyWriteSPI(0xA7);						// 0010.1111	Skriv till adressen 0x20, CTRL_REG1 "HR / Normal / Low-power mode (10 Hz)" mm

	// -----------------------------------------------------------------------------
//x	MyWriteSPI(0x00);						// 0000.0000	Skriv till adressen 0x21, CTRL_REG2 
	MyWriteSPI(0x41);						// 1100.0001	Skriv till adressen 0x21, CTRL_REG2 
	
	// -----------------------------------------------------------------------------
	MyWriteSPI(0x40);						// 0100.000x	Skriv till adressen 0x22, CTRL_REG3, I1_AOI1 = 1

	// -----------------------------------------------------------------------------
//	MyWriteSPI(0x00);						// 1000.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 2g				 15.63 mg/digit
//	MyWriteSPI(0x80);						// 1000.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 2g				 15.63 mg/digit
//	MyWriteSPI(0x82);						// 1000.0010	Skriv till adressen 0x23, CTRL_REG4 +/- 2g Self-test 0	 15.63 mg/digit
//	MyWriteSPI(0x84);						// 1000.0100	Skriv till adressen 0x23, CTRL_REG4 +/- 2g Self-test 1	 15.63 mg/digit
//	MyWriteSPI(0x90);						// 1001.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 4gm 			 31.25 mg/digit
	MyWriteSPI(0xA0);						// 1010.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 8g				 62.50 mg/digit
//	MyWriteSPI(0xB0);						// 1011.0000	Skriv till adressen 0x23, CTRL_REG4 +/- 16g				188.68 mg/digit

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x08);						// 00xx.1000	Skriv till adressen 0x24, CTRL_REG5 LIR_INT1=1 interrupt request latched
//	MyWriteSPI(0x00);						// 00xx.0000	Skriv till adressen 0x24, CTRL_REG5 LIR_INT1=0 interrupt request not latched

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x00);						// 0000.1000	Skriv till adressen 0x25, CTRL_REG6

	// -----------------------------------------------------------------------------
	MyWriteSPI(0x00);						// 0000.0000	Skriv till adressen 0x26, REFERENCE/DATACAPTURE
//	MyWriteSPI(0x10);						// 0000.1000	Skriv till adressen 0x26, REFERENCE/DATACAPTURE

	ToggleACC();

	MyWriteSPI(AccWrite | 0x30);			// 0011.0000
	
	// -----------------------------------------------------------------------------
//	MyWriteSPI(0x3F);						// 0011.1111	Skriv till adressen 0x30 INT1_CFG
//x	MyWriteSPI(0x6A);						// 0011.1111	Skriv till adressen 0x30 INT1_CFG
	MyWriteSPI(0x3F);						// 0011.1111	Skriv till adressen 0x30 INT1_CFG
//	MyWriteSPI(0x0A);						// 0011.1111	Skriv till adressen 0x30 INT1_CFG

	ToggleACC();

	MyWriteSPI(AccWrite | Multi | 0x32);	// 0011.0010	
	
	// -----------------------------------------------------------------------------
//	MyWriteSPI(0x10);				// 0000.0001			Skriv till adressen 0x32 INT1_THS
	MyWriteSPI(0x04);				// 0000.0001			Skriv till adressen 0x32 INT1_THS
//	MyWriteSPI(0x10);				// 0000.0001			Skriv till adressen 0x32 INT1_THS
	
	// -----------------------------------------------------------------------------
	MyWriteSPI(0x01);				// 0000.0001			Skriv till adressen 0x33 INT1_DURATION
	MyWriteSPI(0x00);				// 0000.0001			Skriv till adressen 0x33 INT1_DURATION

	ToggleACC();

	// Läs "Dummy Registret"
	MyWriteSPI(AccRead | 0x0F);		// 1000.0000 | 0000.1111

	nReturn = MyReadSPI();

	Nop(); Nop(); Nop();

	ACC_ENABLE = 1;

	CloseSPI();
	
	return nReturn;
}
// Oscillator på 10MHz ger mha PLLEN=1 40Mhz, 40/4=10MHz effektivt, en instruktion tar 0.1us
// (Fosc = 10MHz ger 2.5MHz effektiv klocka)
// -----------------------------------------------------------------------------
// PIC18F8722
/*#pragma config OSC = HSPLL
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config PWRT = ON
#pragma config BOREN = OFF
#pragma config WDT = OFF
#pragma config MODE = MC
#pragma config ADDRBW = ADDR20BIT
#pragma config DATABW = DATA16BIT
#pragma config WAIT = OFF
#pragma config CCP2MX = PORTC
#pragma config ECCPMX = PORTE
#pragma config LPT1OSC = ON
#pragma config MCLRE = ON

#pragma config STVREN = ON
#pragma config STVREN = OFF

#pragma config LVP = OFF
#pragma config BBSIZ = BB2K
#pragma config XINST = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF
#pragma config CP4 = OFF
#pragma config CP5 = OFF
#pragma config CP6 = OFF
#pragma config CP7 = OFF
#pragma config CPB = OFF
#pragma config CPD = OFF
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF
#pragma config WRT4 = OFF
#pragma config WRT5 = OFF
#pragma config WRT6 = OFF
#pragma config WRT7 = OFF
#pragma config WRTC = OFF
#pragma config WRTB = OFF
#pragma config WRTD = OFF
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF
#pragma config EBTR4 = OFF
#pragma config EBTR5 = OFF
#pragma config EBTR6 = OFF
#pragma config EBTR7 = OFF
#pragma config EBTRB = OFF
*/
