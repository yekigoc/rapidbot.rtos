#include "proximity_sensor.h"
#include "FreeRTOS.h"
#include "task.h"

//-----------------------------------------------------------------------------
/// Convert a digital value in milivolt
/// /param valueToconvert Value to convert in milivolt
//-----------------------------------------------------------------------------
unsigned int ConvHex2mV( unsigned int valueToConvert )
{
    return( (ADC_VREF * valueToConvert)/0x3FF);
}


void 
adcinit()
{
  extern void ( vADC_ISR_Wrapper )( void );
  unsigned int id_channel;
  
  ADC_Initialize( AT91C_BASE_ADC,
		  AT91C_ID_ADC,
		  AT91C_ADC_TRGEN_DIS,
		  0,
		  AT91C_ADC_SLEEP_NORMAL_MODE,
		  AT91C_ADC_LOWRES_10_BIT,
		  BOARD_MCK,
		  BOARD_ADC_FREQ,
		  20,
		  600);
  
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_1);
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_2);
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_3);
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_4);  
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_5);
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_6);
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_7);
  ADC_EnableChannel(AT91C_BASE_ADC, ADC_NUM_8);  


  AIC_ConfigureIT(AT91C_ID_ADC, 0, vADC_ISR_Wrapper);

  AIC_EnableIT(AT91C_ID_ADC);
}

unsigned int 
adcgetvalue()
{
  
  ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_1);
  ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_2);
  ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_3);
  ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_4);
  ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_5);
  ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_6);
  ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_7);
  ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_8);
  
  // Start measurement
  ADC_StartConversion(AT91C_BASE_ADC);
  

  unsigned int status;
  unsigned int id_channel;


  //  Pin led=PA8;
  //  PIO_Set(&led);



  /*  while( conversionDone != ((1<<ADC_NUM_1)|(1<<ADC_NUM_2)|(1<<ADC_NUM_3)|(1<<ADC_NUM_4)) )
      trspistat.counter=conversionDone;*/


  /*  for(id_channel=ADC_NUM_1;id_channel<=ADC_NUM_4;id_channel++) {
    
    printf("Channel %d : %d mV\n\r",
	   id_channel,
	   ConvHex2mV(ADC_GetConvertedData(AT91C_BASE_ADC, id_channel))
	   );*/
  //  unsigned int ret = ADC_GetConvertedData(AT91C_BASE_ADC,
  //  ADC_NUM_1);
  //  
  //  status = ADC_GetStatus(AT91C_BASE_ADC);

  /*
  trspistat.adcvalue[1] = ConvHex2mV(ADC_GetConvertedData(AT91C_BASE_ADC, ADC_NUM_2));
  trspistat.adcvalue[2] = ConvHex2mV(ADC_GetConvertedData(AT91C_BASE_ADC, ADC_NUM_3));
  trspistat.adcvalue[3] = ConvHex2mV(ADC_GetConvertedData(AT91C_BASE_ADC, ADC_NUM_4));*/
  return status;
//ConvHex2mV(ADC_GetConvertedData(AT91C_BASE_ADC, ADC_NUM_1));
  //return ret;
  //return 10;
}
