#include "common.h"
#include <adc/adc.h>

#define BOARD_ADC_FREQ 5000000
#define ADC_VREF       3300  // 3.3 * 1000

/// Pio pins to configure.
#ifdef PINS_ADC
static const Pin pinsADC[] = {PINS_ADC};
#endif

#define ADC_NUM_1  ADC_CHANNEL_0
#define ADC_NUM_2  ADC_CHANNEL_1
#define ADC_NUM_3  ADC_CHANNEL_4
#define ADC_NUM_4  ADC_CHANNEL_5

static unsigned int
ConvHex2mV( unsigned int valueToConvert );

void
adcinit();

unsigned int
adcgetvalue();
