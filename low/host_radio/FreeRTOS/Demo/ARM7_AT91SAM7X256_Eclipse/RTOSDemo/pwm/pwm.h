//#include <board.h>
#include "../pio/pio.h"
#include "../aic/aic.h"
#include "../pwmc/pwmc.h"
#include "../usr/liblcd/lcd.h"


#define MIN_DUTY_CYCLE 0
#define MAX_DUTY_CYCLE 500
#define PWM_FREQUENCY   14000
#define CHANNEL_PWM_1 1
#define CHANNEL_PWM_2 2
#define CHANNEL_PWM_3 3
#define CHANNEL_PWM_4 4
#define PB19  {1 << 19, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PB20  {1 << 20, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PB21  {1 << 21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PB22  {1 << 22, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}

//#define PB21   {1 << 21, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}

/// Pio pins to configure.
static const Pin pins[] = {
  PB19,
  PB20,
  PB21,
  PB22
};

void ISR_Pwmc(void);
