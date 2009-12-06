/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "proximity_sensor_task.h"
#include "common.h"
#include "proximity_sensor.h"

unsigned char conversionDone;

/* The ISR can cause a context switch so is declared naked. */
void vADC_ISR_Wrapper( void ) __attribute__ ((naked));

/* The function that actually performs the ISR work.  This must be separate
from the wrapper function to ensure the correct stack frame gets set up. */
void vADC_ISR_Handler( void );

//------------------------------------------------------------------------------
/// Interrupt handler for the ADC. Signals that the conversion is finished by
/// setting a flag variable.
//------------------------------------------------------------------------------
void
vADC_ISR_Handler(void)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
  unsigned int status;
  unsigned int id_channel;
  
  status = ADC_GetStatus(AT91C_BASE_ADC);
  
  /*    for(id_channel=ADC_NUM_1;id_channel<=ADC_NUM_4;id_channel++) {
	
        if (ADC_IsChannelInterruptStatusSet(status, id_channel)) {
	
	ADC_DisableIt(AT91C_BASE_ADC, id_channel);
	conversionDone |= 1<<id_channel;
        }
	}*/
  id_channel=ADC_NUM_1;
  if (ADC_IsChannelInterruptStatusSet(status, id_channel)) 
    {	
      ADC_DisableIt(AT91C_BASE_ADC, id_channel);
      conversionDone |= 1<<id_channel;
    }

  /* Clear AIC to complete ISR processing */
  AT91C_BASE_AIC->AIC_EOICR = 0;
  
  /* Do a task switch if needed */
  if( xHigherPriorityTaskWoken )
    {
      /* This call will ensure that the unblocked task will be executed
	 immediately upon completion of the ISR if it has a priority higher
	 than the interrupted task. */
      portYIELD_FROM_ISR();
    }
}

//------------------------------------------------------------------------------
/// Interrupt handler wrapper for the ADC.
//------------------------------------------------------------------------------
void 
vADC_ISR_Wrapper(void)
{
  /* Save the context of the interrupted task. */
  portSAVE_CONTEXT();
  
  vADC_ISR_Handler();
  
  /* Restore the context of whichever task will execute next. */
  portRESTORE_CONTEXT();
}
