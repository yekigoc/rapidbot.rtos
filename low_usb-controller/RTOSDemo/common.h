#ifndef __COMMON_H__
#define __COMMON_H__

#include "FreeRTOSConfig.h"
#include "../pio/pio.h"
#include "../aic/aic.h"
#include "../pwmc/pwmc.h"

//#define PB21   {1 << 21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}

#define PA0  {1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PA1  {1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PA6  {1 << 6, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PA8  {1 << 8, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PA9  {1 << 9, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define PA10 {1 << 10, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
/// ADC_AD0 pin definition.
#define PIN_ADC_AD0 {1 << 17, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
/// ADC_AD1 pin definition.
#define PIN_ADC_AD1 {1 << 18, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
/// ADC_AD2 pin definition. (mixed with PIN_PUSHBUTTON_1)
#define PIN_ADC_AD2 {1 << 19, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
/// ADC_AD3 pin definition. (mixed with PIN_PUSHBUTTON_2)
#define PIN_ADC_AD3 {1 << 20, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
/// Pins ADC
#define PINS_ADC PIN_ADC_AD0, PIN_ADC_AD1, PIN_ADC_AD2, PIN_ADC_AD3

#define NPCS3 {1 << 5, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define CDIN {1 << 12, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define CDOUT {1 << 13, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define SPCK {1 << 14, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}

static const Pin pins[] = {
  PA0,
  PA1,
  PA6,
  PA8,
  PA9,
  PA10,
  NPCS3,
  CDIN,
  CDOUT,
  SPCK
  //  PINS_ADC
};

#define CHANNEL_PWM_1 0
#define CHANNEL_PWM_2 1
#define MIN_DUTY_CYCLE 0
#define MAX_DUTY_CYCLE 5000
#define PWM_FREQUENCY  50

typedef struct
{
  unsigned int compassstat;
  unsigned int compassdata;
  unsigned int counter;
  unsigned short usbinited;
  unsigned short dutycycle1;
  unsigned int changecycle1;
  unsigned short dutycycle2;
  unsigned int changecycle2;
  int led;
  int ledctr;
  unsigned int adcvalue[8];
  char leds[3];
} spistat;

spistat trspistat;

//------------------------------------------------------------------------------
// Clocks
//------------------------------------------------------------------------------
/// Frequency of the board main oscillator, in Hz.
#define BOARD_MAINOSC           18432000

/// Master clock frequency (when using board_lowlevel.c), in Hz.
//#define BOARD_MCK               48000000
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ADC
//------------------------------------------------------------------------------
/// ADC clock frequency, at 10-bit resolution (in Hz)
#define ADC_MAX_CK_10BIT         6000000
/// ADC clock frequency, at 8-bit resolution (in Hz)
#define ADC_MAX_CK_8BIT          8000000
/// Startup time max, return from Idle mode (in µs)
#define ADC_STARTUP_TIME_MAX       20
/// Track and hold Acquisition Time min (in ns)
#define ADC_TRACK_HOLD_TIME_MIN   600

//#define BOARD_ADC_FREQ 5000000
//#define ADC_VREF       3300  // 3.3 * 1000

#endif
