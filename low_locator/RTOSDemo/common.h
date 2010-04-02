#ifndef __COMMON_H__
#define __COMMON_H__

#include "FreeRTOSConfig.h"
#include "../pio/pio.h"
#include "../aic/aic.h"
#include "../pwmc/pwmc.h"
#include "common_structs.h"

//#define PB21   {1 << 21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}

//#define PA6  {1 << 6, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT} 
//#define PA8  {1 << 8, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PA9  {1 << 9, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define PA10 {1 << 10, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}

#define PIN_ADC_AD0 {1 << 17, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}        //adc0
#define PIN_ADC_AD1 {1 << 18, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}        //adc1
#define PIN_ADC_AD2 {1 << 19, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}        //adc2
#define PIN_ADC_AD3 {1 << 20, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}        //adc3

#define PINS_ADC PIN_ADC_AD0, PIN_ADC_AD1, PIN_ADC_AD2, PIN_ADC_AD3 // Pins ADC

#define NPCS_LOC {1 << 4, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}         //locator amplifier npcs
#define CDOUT_LOC {1 << 6, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}        //locator amplifier controller data out
#define SPCK_LOC {1 << 8, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}         //locator amplifier clock

#define PINS_LOC_AMP NPCS_LOC, CDOUT_LOC, SPCK_LOC // Pins for Locator Amplifier

static const Pin pins[] = {
  //  PA6,
  //  PA8,
  PA9,
  PA10,
  PINS_LOC_AMP
  //  PINS_ADC
};

#define MIN_DUTY_CYCLE 0
#define MAX_DUTY_CYCLE 5000
#define PWM_FREQUENCY  50

typedef struct
{
  unsigned long counter;
  unsigned short usbinited;
  unsigned short adcbuf1[256];
  char converted;               //wether conversion is over
  unsigned char readingadcbuf;  //usb reading buf flag
  unsigned char usbdataready;
  unsigned char wbufidx;        //current write index
  unsigned char amp;            //amplifierstate (0-16)
  unsigned char part;
  unsigned char ampchanged;     //wether amplification changed
  char leds;
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
#define ADC_TRACK_HOLD_TIME_MIN   300

//#define BOARD_ADC_FREQ 5000000
//#define ADC_VREF       3300  // 3.3 * 1000

#endif
