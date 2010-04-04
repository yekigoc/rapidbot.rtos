/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "common.h"
#include "locator.h"
#include "locator_task.h"


/* The ISR can cause a context switch so is declared naked. */
void vLOC_ISR_Wrapper( void ) __attribute__ ((naked));

/* The ISR can cause a context switch so is declared naked. */
void vLOC_TC_ISR_Wrapper( void ) __attribute__ ((naked));

/* The function that actually performs the ISR work.  This must be separate
   from the wrapper function to ensure the correct stack frame gets set up. */
void vLOC_ISR_Handler( void );

/* The function that actually performs the ISR work.  This must be separate
   from the wrapper function to ensure the correct stack frame gets set up. */
void ISR_Tc0( void );


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
  
  for(id_channel=ADC_CHANNEL_5;id_channel<=ADC_CHANNEL_5;id_channel++) 
    {
      if (ADC_IsChannelInterruptStatusSet(status, id_channel)) 
	{
	  ADC_DisableIt(AT91C_BASE_ADC, id_channel);
	  //	  ADC_GetConvertedData(AT91C_BASE_ADC, id_channel);
	  trspistat.adcvals[id_channel] = ConvHex2mV(ADC_GetConvertedData(AT91C_BASE_ADC, id_channel));
	  ADC_EnableIt(AT91C_BASE_ADC,id_channel);
	}
    }

  // Start measurement
  ADC_StartConversion(AT91C_BASE_ADC);

  AT91C_BASE_AIC->AIC_EOICR = 0;

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

//------------------------------------------------------------------------------
/// Interrupt handler for TC0. Displays the number of bytes received during the
/// last second and the total number of bytes received, then restarts a read
/// transfer on the USART if it was stopped.
//------------------------------------------------------------------------------
void ISR_Tc0(void)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  unsigned int status;

  // Read TC0 status
  status = AT91C_BASE_TC0->TC_SR;
  int i = 0;

  // RC compare
  if ((status & AT91C_TC_CPCS) == AT91C_TC_CPCS) 
    {
      if (trspistat.usbdataready == 0)
	{
	  for (i = 0; i< LOC_NUMADCCHANNELS; i++)
	    {
	      if (trspistat.channels[i].wbufidx<256)
		{
		  trspistat.channels[i].adcbuf[trspistat.channels[i].wbufidx] = trspistat.adcvals[i];
		  
		  trspistat.channels[i].wbufidx++;  
		}
	      else
		{
		  if (i == (LOC_NUMADCCHANNELS - 1))
		    trspistat.usbdataready = 1;
		  trspistat.channels[i].wbufidx=0;
		  TC_Stop(AT91C_BASE_TC0);
		}
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
/// Interrupt handler wrapper for the TC.
//------------------------------------------------------------------------------
void 
vLOC_TC_ISR_Wrapper(void)
{
  /* Save the context of the interrupted task. */
  portSAVE_CONTEXT();
  
  ISR_Tc0();
  
  /* Restore the context of whichever task will execute next. */
  portRESTORE_CONTEXT();
}
