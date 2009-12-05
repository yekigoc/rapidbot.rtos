#include "proximity_sensor.h"

static volatile unsigned char conversionDone;

//-----------------------------------------------------------------------------
/// Convert a digital value in milivolt
/// /param valueToconvert Value to convert in milivolt
//-----------------------------------------------------------------------------
static unsigned int ConvHex2mV( unsigned int valueToConvert )
{
    return( (ADC_VREF * valueToConvert)/0x3FF);
}

//------------------------------------------------------------------------------
/// Interrupt handler for the ADC. Signals that the conversion is finished by
/// setting a flag variable.
//------------------------------------------------------------------------------
static void 
ISR_Adc(void)
{
  unsigned int status;
  unsigned int id_channel;
  
  status = ADC_GetStatus(AT91C_BASE_ADC);
  
  /*    for(id_channel=ADC_NUM_1;id_channel<=ADC_NUM_4;id_channel++) {
	
        if (ADC_IsChannelInterruptStatusSet(status, id_channel)) {
	
	ADC_DisableIt(AT91C_BASE_ADC, id_channel);
	conversionDone |= 1<<id_channel;
        }
	}*/
  id_channel=ADC_NUM_1;
  if (ADC_IsChannelInterruptStatusSet(status, id_channel)) 
    {	
      ADC_DisableIt(AT91C_BASE_ADC, id_channel);
      conversionDone |= 1<<id_channel;
    }
}

void 
adcinit()
{
  unsigned int id_channel;
  
  ADC_Initialize( AT91C_BASE_ADC,
		  AT91C_ID_ADC,
		  AT91C_ADC_TRGEN_DIS,
		  0,
		  AT91C_ADC_SLEEP_NORMAL_MODE,
		  AT91C_ADC_LOWRES_10_BIT,
		  BOARD_MCK,
		  BOARD_ADC_FREQ,
		  10,
		  1200);
  
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_1);
  /*  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_2);
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_3);
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_4);*/
  
  AIC_ConfigureIT(AT91C_ID_ADC, 0, ISR_Adc);
  AIC_EnableIT(AT91C_ID_ADC);
}

unsigned int 
adcgetvalue()
{
  conversionDone = 0;
  
  ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_1);
  /*        ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_2);
	    ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_3);
	    ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_4);*/
  
  // Start measurement
  ADC_StartConversion(AT91C_BASE_ADC);
  
  while( conversionDone != ((1<<ADC_NUM_1) /*|(1<<ADC_NUM_2)|(1<<ADC_NUM_3)|(1<<ADC_NUM_4))*/ ));
  
  /*        for(id_channel=ADC_NUM_1;id_channel<=ADC_NUM_4;id_channel++) {
	    
	    printf("Channel %d : %d mV\n\r",
	    id_channel,
	    ConvHex2mV(ADC_GetConvertedData(AT91C_BASE_ADC, id_channel))
	    );*/
  return ConvHex2mV(ADC_GetConvertedData(AT91C_BASE_ADC, AT91C_BASE_ADC));
}
