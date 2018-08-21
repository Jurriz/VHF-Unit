#line 1 "mcc_generated_files/pin_manager.c"
#line 1 "mcc_generated_files/pin_manager.c"

#line 24 "mcc_generated_files/pin_manager.c"
 


#line 47 "mcc_generated_files/pin_manager.c"
 


#line 1 "mcc_generated_files/pin_manager.h"

#line 22 "mcc_generated_files/pin_manager.h"
 


#line 45 "mcc_generated_files/pin_manager.h"
 


#line 49 "mcc_generated_files/pin_manager.h"

#line 51 "mcc_generated_files/pin_manager.h"
#line 52 "mcc_generated_files/pin_manager.h"

#line 54 "mcc_generated_files/pin_manager.h"
#line 55 "mcc_generated_files/pin_manager.h"

#line 57 "mcc_generated_files/pin_manager.h"
#line 58 "mcc_generated_files/pin_manager.h"

#line 60 "mcc_generated_files/pin_manager.h"
#line 61 "mcc_generated_files/pin_manager.h"


#line 71 "mcc_generated_files/pin_manager.h"
 
void PIN_MANAGER_Initialize (void);


#line 83 "mcc_generated_files/pin_manager.h"
 
void PIN_MANAGER_IOC(void);



#line 89 "mcc_generated_files/pin_manager.h"

#line 91 "mcc_generated_files/pin_manager.h"
 #line 50 "mcc_generated_files/pin_manager.c"







void PIN_MANAGER_Initialize(void)
{
    
#line 61 "mcc_generated_files/pin_manager.c"
 
    LATE = 0x00;
    LATD = 0x00;
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;

    
#line 70 "mcc_generated_files/pin_manager.c"
 
    TRISE = 0x07;
    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISC = 0xFF;
    TRISD = 0xFF;

    
#line 79 "mcc_generated_files/pin_manager.c"
 
    ANSELD = 0xFF;
    ANSELC = 0xFC;
    ANSELB = 0x3F;
    ANSELE = 0x07;
    ANSELA = 0x2F;

    
#line 88 "mcc_generated_files/pin_manager.c"
 
    WPUB = 0x00;
    INTCON2bits.nRBPU = 1;

    
#line 94 "mcc_generated_files/pin_manager.c"
 





   
    
}
  
void PIN_MANAGER_IOC(void)
{   
	
    INTCONbits.RBIF = 0;
}


#line 112 "mcc_generated_files/pin_manager.c"
 