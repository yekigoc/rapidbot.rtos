#include "FreeRTOSConfig.h"

#define CHANNEL_PWM_1 0
#define MIN_DUTY_CYCLE 0
#define MAX_DUTY_CYCLE 5000
#define PWM_FREQUENCY  50

typedef struct
{
  unsigned int counter;
  unsigned short usbinited;
  unsigned short dutycycle;
  unsigned int changecycle;
} spistat;

spistat trspistat;
