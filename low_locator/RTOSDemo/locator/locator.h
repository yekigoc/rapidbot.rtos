#include "common.h"

#define BOARD_ADC_FREQ 5000000
#define ADC_VREF       3300  // 3.3 * 1000

unsigned int
ConvHex2mV( unsigned int valueToConvert );


unsigned short 
adchanfindmax(unsigned char chan);
