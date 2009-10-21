#ifdef AT91SAM7S

#include "FreeRTOSConfig.h"
#define TRRESET_PIN (1<<8)
#define MISO_PIN (1<<12)
#define MOSI_PIN (1<<13)
#define NPCS_PIN (1<<11)
#define SPCK_PIN (1<<14)
#define FIFOFLG_PIN (1<<16)
#define PKTFLG_PIN (1<<15)

#define TRRESET_PIO {TRRESET_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define MISO_PIO {MISO_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define MOSI_PIO {MOSI_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define NPCS_PIO {NPCS_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
//#define NPCS_PIO {NPCS_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define SPCK_PIO {SPCK_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define FIFOFLG_PIO {FIFOFLG_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PKTFLG_PIO {PKTFLG_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}

#define TR24_NPCS 1

#define TR24_BAUDRATE 9600

typedef struct
{
  unsigned short spiisrcalls;
  unsigned short trinited;
  unsigned short counter;
  unsigned short usbinited;
  unsigned int spistatreg;
} spistat;

/*
/// SPI driver instance.
SdSpi sdSpiDrv;

/// SDCard driver instance.
SdCard sdDrv;
*/

unsigned char FIFObuf[64];

AT91S_SPI trspi;
spistat trspistat;

#endif

#ifdef LPC2103
#define PLL_FCCLK 14745600
#define BOARD_MCK PLL_FCCLK//58982400
#define PIN_RESET 1<<26
#define PIN_LED 1<<21
#define PIN_FIFOFLG 1<<23
#define PIN_PKTFLG 1<<22
#define PIN_SS 1<<7

#define SPIF (1<<7)
#include "../lpc210x.h"
#endif

void tr24_init();
void tr24_initrfic();
void ISR_Spi0(void);
void UTIL_WaitTimeInMs(unsigned int mck, unsigned int time_ms);
void UTIL_WaitTimeInUs(unsigned int mck, unsigned int time_ns);
void UTIL_WaitTimeInNs(unsigned int mck, unsigned int time_ns);

