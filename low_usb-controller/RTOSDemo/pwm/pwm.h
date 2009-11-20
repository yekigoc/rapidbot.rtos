//#include <board.h>
#include "../pio/pio.h"
#include "../aic/aic.h"
#include "../pwmc/pwmc.h"
#include "../common.h"


//#define PB21   {1 << 21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}

#define PA0  {1 << 0, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PA1  {1 << 1, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

static const Pin pins[] = {
  PA0,
  PA1
};


void ISR_Pwmc(void);
