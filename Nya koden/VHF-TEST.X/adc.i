#line 1 "mcc_generated_files/adc.c"
#line 1 "mcc_generated_files/adc.c"

#line 22 "mcc_generated_files/adc.c"
 


#line 45 "mcc_generated_files/adc.c"
 


#line 49 "mcc_generated_files/adc.c"
 


#line 1 "mcc_generated_files/adc.h"

#line 22 "mcc_generated_files/adc.h"
 


#line 45 "mcc_generated_files/adc.h"
 


#line 49 "mcc_generated_files/adc.h"


#line 52 "mcc_generated_files/adc.h"
 





#line 63 "mcc_generated_files/adc.h"


#line 66 "mcc_generated_files/adc.h"
 


#line 70 "mcc_generated_files/adc.h"
 

typedef uint16_t adc_result_t;


#line 76 "mcc_generated_files/adc.h"
 
typedef struct
{
    adc_result_t adcResult1;
    adc_result_t adcResult2;
} adc_sync_double_result_t;


#line 93 "mcc_generated_files/adc.h"
 

typedef enum
{
    channel_CTMU =  0x1D,
    channel_DAC =  0x1E,
    channel_FVRBuf2 =  0x1F
} adc_channel_t;


#line 104 "mcc_generated_files/adc.h"
 


#line 134 "mcc_generated_files/adc.h"
 
void ADC_Initialize(void);


#line 164 "mcc_generated_files/adc.h"
 
void ADC_SelectChannel(adc_channel_t channel);


#line 191 "mcc_generated_files/adc.h"
 
void ADC_StartConversion();


#line 223 "mcc_generated_files/adc.h"
 
bool ADC_IsConversionDone();


#line 256 "mcc_generated_files/adc.h"
 
adc_result_t ADC_GetConversionResult(void);


#line 286 "mcc_generated_files/adc.h"
 
adc_result_t ADC_GetConversion(adc_channel_t channel);


#line 314 "mcc_generated_files/adc.h"
 
void ADC_TemperatureAcquisitionDelay(void);

#line 322 "mcc_generated_files/adc.h"

#line 324 "mcc_generated_files/adc.h"

#line 326 "mcc_generated_files/adc.h"
 

#line 52 "mcc_generated_files/adc.c"

#line 1 "mcc_generated_files/mcc.h"

#line 22 "mcc_generated_files/mcc.h"
 


#line 45 "mcc_generated_files/mcc.h"
 


#line 49 "mcc_generated_files/mcc.h"

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
 #line 50 "mcc_generated_files/mcc.h"



#line 1 "mcc_generated_files/adc.h"

#line 22 "mcc_generated_files/adc.h"
 


#line 45 "mcc_generated_files/adc.h"
 


#line 52 "mcc_generated_files/adc.h"
#line 63 "mcc_generated_files/adc.h"

#line 66 "mcc_generated_files/adc.h"

#line 70 "mcc_generated_files/adc.h"

#line 76 "mcc_generated_files/adc.h"

#line 93 "mcc_generated_files/adc.h"

#line 104 "mcc_generated_files/adc.h"

#line 134 "mcc_generated_files/adc.h"

#line 164 "mcc_generated_files/adc.h"

#line 191 "mcc_generated_files/adc.h"

#line 223 "mcc_generated_files/adc.h"

#line 256 "mcc_generated_files/adc.h"

#line 286 "mcc_generated_files/adc.h"

#line 314 "mcc_generated_files/adc.h"
#line 322 "mcc_generated_files/adc.h"
#line 324 "mcc_generated_files/adc.h"

#line 326 "mcc_generated_files/adc.h"
 

#line 53 "mcc_generated_files/mcc.h"


#line 56 "mcc_generated_files/mcc.h"



#line 68 "mcc_generated_files/mcc.h"
 
void SYSTEM_Initialize(void);


#line 81 "mcc_generated_files/mcc.h"
 
void OSCILLATOR_Initialize(void);

#line 85 "mcc_generated_files/mcc.h"

#line 87 "mcc_generated_files/mcc.h"
 #line 53 "mcc_generated_files/adc.c"




#line 58 "mcc_generated_files/adc.c"
 

void ADC_Initialize(void)
{
    
    
    
    ADCON0 = 0x01;
    
    
    ADCON1 = 0x00;
    
    
    ADCON2 = 0x00;
    
    
    ADRESL = 0x00;
    
    
    ADRESH = 0x00;
    
}

void ADC_SelectChannel(adc_channel_t channel)
{
    
    ADCON0bits.CHS = channel;    
    
    ADCON0bits.ADON = 1;  
}

void ADC_StartConversion()
{
    
    ADCON0bits.GO_nDONE = 1;
}


bool ADC_IsConversionDone()
{
    
    return ((bool)(!ADCON0bits.GO_nDONE));
}

adc_result_t ADC_GetConversionResult(void)
{
    
     return ((adc_result_t)((ADRESH << 8) + ADRESL));
}

adc_result_t ADC_GetConversion(adc_channel_t channel)
{
    
    ADCON0bits.CHS = channel;    

    
    ADCON0bits.ADON = 1;

    
    ADCON0bits.GO_nDONE = 1;

    
    while (ADCON0bits.GO_nDONE)
    {
        CLRWDT();
    }

    
    return ((adc_result_t)((ADRESH << 8) + ADRESL));
}

void ADC_TemperatureAcquisitionDelay(void)
{
    __delay_us(200);
}

#line 135 "mcc_generated_files/adc.c"
 