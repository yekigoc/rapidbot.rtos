#include "compass.h"
#include "FreeRTOS.h"
#include "task.h"

void 
spiinit()
{
  extern void ( vADC_ISR_Wrapper )( void );
  unsigned int id_channel;
  
}

