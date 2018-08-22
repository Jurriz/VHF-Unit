char szInfo[32];

// Svar från CGPS III
//
// 0x22 0x44 0x60 0x00 0x86 0x00 0x00 0x06
// 0x06 0x00 0x02 0x00 0x00 0x01

// ------------------------------------------------------------------------------------
void DoResetSi4460(void)
{
	// Reset mha SDN, alla register clearas
	TRX_SDN = 1;

	Delay10TCYx(35);	// 35x10us=350us, ska vara 300us

	TRX_SDN = 0;

	Delay(6);		// 5ms
}	

// ------------------------------------------------------------------------------------
unsigned char ReadFrom4460(unsigned char nProp, unsigned char nLen, unsigned char *szData)
{
	unsigned char nLoop, nReturn, nTmp;

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

	return nReturn;
}	

// ------------------------------------------------------------------------------------
unsigned char DoCheckCTSManyTimes(void)
{
	char nReturn, nLoop;

	// The typical time for a valid FFh CTS reading is 20 µs
	TRX_EN = 0;
	Nop();
	MyWriteSPI(0x44);		// Läs CTS
	Nop();
	nReturn = MyReadSPI();
	Nop();
	TRX_EN = 1;

	TEST1 = 1;

	nLoop = 0;
	while ( (nReturn != 0xFF) && (nLoop < 5) )	// 1 varv ca. 175-200 us, Tidigare var (nLoop < 40)
	{
		TEST2 = 1;

		TRX_EN = 0;
		Nop();
		MyWriteSPI(0x44);		// Läs CTS
		Nop();
		nReturn = MyReadSPI();
		Nop();
		TRX_EN = 1;
		nLoop++;
		
		TEST2 = 0;
	}

	TEST1 = 0;

	return nReturn;
}

// ------------------------------------------------------------------------------------
// Läs ut PART_INFO
void DoRead4460_PART_INFO(void)
{
	unsigned char nTmp;

	DoResetSi4460();

	TCXO_EN = 1;

	Delay(5);

	MyOpenSPI(SPI_FOSC_4, MODE_00, SMPMID);

	nTmp = DoCheckCTSManyTimes();

	DoSendVHFSetupToSi4460(0);		// [0x02, 0x01, 0x01, 0x01, 0x8C, 0xBA, 0x80] - POWER_UP [0]	startar radion och ställer in den för TCXO

	Delay(15);		// 14ms

	nTmp = DoCheckCTSManyTimes();

	// Läs från radion
	ReadFrom4460(0x01, 8, (unsigned char *)szInfo);

	MyCloseSPI();
		
	TCXO_EN = 0;		// OBS - Sist
}

// ------------------------------------------------------------------------------------
// Läs ut FUNC_INFO
void DoRead4460_FUNC_INFO(void)
{
	unsigned char nTmp;

	DoResetSi4460();

	TCXO_EN = 1;

	Delay(5);

	MyOpenSPI(SPI_FOSC_4, MODE_00, SMPMID);

	nTmp = DoCheckCTSManyTimes();

	DoSendVHFSetupToSi4460(0);		// [0x02, 0x01, 0x01, 0x01, 0x8C, 0xBA, 0x80] - POWER_UP [0]	startar radion och ställer in den för TCXO

	Delay(15);		// 14ms

	nTmp = DoCheckCTSManyTimes();

	// Läs från radion
	ReadFrom4460(0x10, 6, (unsigned char *)szInfo);

	MyCloseSPI();
		
	TCXO_EN = 0;		// OBS - Sist
}

