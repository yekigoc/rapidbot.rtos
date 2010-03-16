/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include "locator.h"
#include "locator_task.h"


/* The ISR can cause a context switch so is declared naked. */
void vLOC_ISR_Wrapper( void ) __attribute__ ((naked));

/* The function that actually performs the ISR work.  This must be separate
from the wrapper function to ensure the correct stack frame gets set up. */
void vLOC_ISR_Handler( void );

//------------------------------------------------------------------------------
/// Interrupt handler for the ADC. Signals that the conversion is finished by
/// setting a flag variable.
//------------------------------------------------------------------------------
void
vLOC_ISR_Handler(void)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
  unsigned int status;
  unsigned int id_channel;
  
  status = ADC_GetStatus(AT91C_BASE_ADC);
  
  id_channel=ADC_NUM_1;
  
  if (ADC_IsChannelInterruptStatusSet(status, id_channel)) 
    {
      ADC_DisableIt(AT91C_BASE_ADC, id_channel);
      if (trspistat.wbufidx==255)
	{
	  ADC_GetConvertedData(AT91C_BASE_ADC, id_channel);
	}
      else
	{
	  trspistat.adcbuf1[trspistat.wbufidx] = ConvHex2mV(ADC_GetConvertedData(AT91C_BASE_ADC, id_channel));
	}
    }
  AT91C_BASE_AIC->AIC_EOICR = 0;
  if (trspistat.wbufidx<255)
    {
      trspistat.wbufidx++;
      ADC_EnableIt(AT91C_BASE_ADC, ADC_NUM_1);
      
      // Start measurement
      ADC_StartConversion(AT91C_BASE_ADC);
    }
  else
    {
      trspistat.wbufidx=0;
      trspistat.usbdataready = 1;
    }

  //  trspistat.counter=conversionDone;
  /* Clear AIC to complete ISR processing */
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
vLOC_ISR_Wrapper(void)
{
  /* Save the context of the interrupted task. */
  portSAVE_CONTEXT();
  
  vLOC_ISR_Handler();
  
  /* Restore the context of whichever task will execute next. */
  portRESTORE_CONTEXT();
}
