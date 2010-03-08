#include "common.h"
#include <adc/adc.h>

#define BOARD_ADC_FREQ 5000000
#define ADC_VREF       3300  // 3.3 * 1000

#define ADC_NUM_1  ADC_CHANNEL_0
#define ADC_NUM_2  ADC_CHANNEL_1
#define ADC_NUM_3  ADC_CHANNEL_2
#define ADC_NUM_4  ADC_CHANNEL_3
#define ADC_NUM_5  ADC_CHANNEL_4
#define ADC_NUM_6  ADC_CHANNEL_5
#define ADC_NUM_7  ADC_CHANNEL_6
#define ADC_NUM_8  ADC_CHANNEL_7

unsigned int
ConvHex2mV( unsigned int valueToConvert );

void
adcinit();

unsigned int
adcgetvalue();

