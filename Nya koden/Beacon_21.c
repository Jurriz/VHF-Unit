#include <p18f46k22.h>
#include <stdlib.h>
#include <timers.h>
#include <usart.h>
#include <string.h>
#include <spi.h>
#include <delays.h>
#include <adc.h>

#include "Header_h.h"

void GreenLedPulse(int nDelay);
void RedLedPulse(int nDelay);
void NoLedPulse(int nDelay);

// ------------------------------------------------------------------------------------
void GreenLedPulse(int nDelay)
{
	GREEN_LED = 1;
	DoTurnBeaconPulseOn();
	Delay(nDelay);
	GREEN_LED = 0;
	Delay(nDelay);
	DoTurnBeaconPulseOff();
}

// ------------------------------------------------------------------------------------
void RedLedPulse(int nDelay)
{
	RED_LED = 1;
	DoTurnBeaconPulseOn();
	Delay(nDelay);
	RED_LED = 0;
	Delay(nDelay);
	DoTurnBeaconPulseOff();
}

// ------------------------------------------------------------------------------------
void NoLedPulse(int nDelay)
{
	DoTurnBeaconPulseOn();
	Delay(nDelay);
	DoTurnBeaconPulseOff();
}

#ifdef SKROT
		
		// Recovery styr dubbelpuls och Mortaliteten pulsperioden vid "MortOvery"
		else if ( (Flaggs.bRecoveryBeacon == 1) && (BeaconBits.MORTALITET == 1) )
		{
			if (TICK != BeaconPP.nMortPP)
			{
				TICK = BeaconPP.nMortPP;
				DoChangeTickRV3049(TICK);
			}
		}
		// Bara ren Recovery kvar
		else
		{
			if (TICK != BeaconPP.nRecoveryPP)
			{
				TICK = BeaconPP.nRecoveryPP;
				DoChangeTickRV3049(TICK);
			}
		}
	
// (Mortalitet OCH INTE DroppedOff) ELLER Recovery ELLER (Recovery OCH Mortalitet)

		// Är det schemalagd beacon?
		if (nScheduledBeacon == 0x02)
		{
		if ( ( (BeaconBits.MORTALITET == 1) && (Flaggs.bDroppedOff == 0) ) || () || (Flaggs.bRecoveryBeacon == 1) ) 
		{
			// nRecPulser i förhållande till nRecoveryControl 
			if (nRecoveryControl != 0x00)
			{
				// Ex: nRecoveryControl == 5, 00 10 20 30 40 50 61
				nRecPulser++;
				if (nRecPulser >= nRecoveryControl)
				{
					nRecPulser = 0;
					Flaggs.bRecToggle = !Flaggs.bRecToggle;
				}
			}
			else
			{
				Flaggs.bRecToggle = 1;
			}	
		
			if (Flaggs.bRecToggle == 1)
			{
				// Första puls
				RED_LED = LED_ON;
				DoTurn151BeaconPulseOn();
				Delay(3);
				Delay(15);
				DoTurn151BeaconPulseOff();
				RED_LED = LED_OFF;
							
				if ( (Flaggs.bRecoveryBeacon == 1) && (nScheduledBeacon == 0x02) )
				{
					if (TICK != BeaconPP.nRecoveryPP)
					{
						TICK = BeaconPP.nRecoveryPP;
						DoChangeTickRV3049(TICK);
					}

					Delay(80);
	
					DoTurn151BeaconPulseOn();
					Delay(18);
					DoTurn151BeaconPulseOff();
				}
				else if ( (BeaconBits.MORTALITET == 1) && (nScheduledBeacon == 0x02) )
				{
					if (TICK != BeaconPP.nMortPP)
					{
						TICK = BeaconPP.nMortPP;
						DoChangeTickRV3049(TICK);
					}

					if (Flaggs.bMort_Toggle == 1)
					{
						Flaggs.bMort_Toggle = 0;
						Delay(80);
		
						DoTurn151BeaconPulseOn();
						Delay(18);
						DoTurn151BeaconPulseOff();
					}
					else if (Flaggs.bMort_Toggle == 0)
					{
						Flaggs.bMort_Toggle = 1;
					}
				}
			}
		}
		else if ( (BeaconBits.MORTALITET == 1) && (Flaggs.bDroppedOff == 1) )
		{
			if (TICK != 12)
			{
				TICK = 12;
				DoChangeTickRV3049(TICK);
			}

			DoTurn151BeaconPulseOn();
			Delay(18);
			DoTurn151BeaconPulseOff();

			if (Flaggs.bDO_Toggle == 1)
			{
				Flaggs.bDO_Toggle = 0;

				Delay(120);

				DoTurn151BeaconPulseOn();
				Delay(18);
				DoTurn151BeaconPulseOff();

				Delay(120);

				DoTurn151BeaconPulseOn();
				Delay(18);
				DoTurn151BeaconPulseOff();
			}
			else 	// if (Flaggs.bDO_Toggle == 0)
			{
				Flaggs.bDO_Toggle = 1;
			}
		}
		else if (Flaggs.bDroppedOff == 1)
		{
			if (TICK != 12)
			{
				TICK = 12;
				DoChangeTickRV3049(TICK);
			}

			DoTurn151BeaconPulseOn();
			Delay(18);
			DoTurn151BeaconPulseOff();
			
			if (Flaggs.bDO_Toggle == 1)
			{
				Flaggs.bDO_Toggle = 0;

				Delay(80);
			}
			else if (Flaggs.bDO_Toggle == 0)
			{
				Flaggs.bDO_Toggle = 1;

				Delay(140);
			}

			DoTurn151BeaconPulseOn();
			Delay(18);
			DoTurn151BeaconPulseOff();
		}
#endif
