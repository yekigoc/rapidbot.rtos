#include "FreeRTOSConfig.h"

#define TRRESET_PIN (1<<8)
#define MISO_PIN (1<<12)
#define MOSI_PIN (1<<13)
#define NPCS_PIN (1<<11)
#define SPCK_PIN (1<<14)
#define FIFOFLG_PIN (1<<16)
#define PKTFLG_PIN (1<<15)

#define TRRESET_PIO {TRRESET_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define MISO_PIO {MISO_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define MOSI_PIO {MOSI_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define NPCS_PIO {NPCS_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define SPCK_PIO {SPCK_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define FIFOLG_PIO {FIFOFLG_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PKTFLG_PIO {PKTFLG_PIN, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT}

#define TR24_NPCS 1

#define TR24_BAUDRATE 115200

typedef struct
{
  unsigned short spiisrcalls;
  unsigned short trinited;
  unsigned short counter;
  unsigned short usbinited;
} spistat;

/*
/// SPI driver instance.
SdSpi sdSpiDrv;

/// SDCard driver instance.
SdCard sdDrv;
*/

AT91S_SPI trspi;
spistat trspistat;

void tr24_init();
void ISR_Spi0(void);
