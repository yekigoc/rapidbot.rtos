#include "common.h"

#define BOARD_ADC_FREQ 5000000
#define ADC_VREF       3300  // 3.3 * 1000

#define ADC_NUM_1  ADC_CHANNEL_5

unsigned int
ConvHex2mV( unsigned int valueToConvert );
