/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "compass_task.h"
#include "common.h"
#include "compass.h"

/* The ISR can cause a context switch so is declared naked. */
void vSPI_ISR_Wrapper( void ) __attribute__ ((naked));

/* The function that actually performs the ISR work.  This must be separate
from the wrapper function to ensure the correct stack frame gets set up. */
void vSPI_ISR_Handler( void );

//------------------------------------------------------------------------------
/// Interrupt handler for the ADC. Signals that the conversion is finished by
/// setting a flag variable.
//------------------------------------------------------------------------------
void
vSPI_ISR_Handler(void)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  unsigned int status;
  unsigned int i;

  // Check PIO controller status
  status = AT91C_BASE_PIOA->PIO_ISR;
  status &= AT91C_BASE_PIOA->PIO_IMR;
  if (status != 0) 
    {
      // Source has PIOs which have changed
      if ((status & 1<<12) != 0) 
	{
	  if (cmpstat.currentoffset<4)
	    {
	      cmpstat.header |= (AT91C_BASE_PIOA->PIO_PDSR & 1<<12)<<cmpstat.currentoffset;
	      cmpstat.currentoffset += 1;
	    }
	}
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
vSPI_ISR_Wrapper(void)
{
  /* Save the context of the interrupted task. */
  portSAVE_CONTEXT();
  
  vSPI_ISR_Handler();
  
  /* Restore the context of whichever task will execute next. */
  portRESTORE_CONTEXT();
}
