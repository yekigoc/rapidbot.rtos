#include "FreeRTOSConfig.h"

#define CHANNEL_PWM_1 0
#define CHANNEL_PWM_2 1
#define MIN_DUTY_CYCLE 0
#define MAX_DUTY_CYCLE 5000
#define PWM_FREQUENCY  50

typedef struct
{
  unsigned int counter;
  unsigned short usbinited;
  unsigned short dutycycle1;
  unsigned int changecycle1;
  unsigned short dutycycle2;
  unsigned int changecycle2;
  int led;
} spistat;

spistat trspistat;
