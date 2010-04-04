/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

//#include "partest.h"
#include "USB-CDC.h"
#include "common.h"
#include "pio/pio.h"

#include "adc/adc.h"
//#include <memory.h>

#include "locator/locator_task.h"

/* Priorities for the demo application tasks. */
#define mainUSB_PRIORITY			( tskIDLE_PRIORITY +2 )
#define mainUSB_TASK_STACK			( 200 )

/* The rate at which the idle hook sends data to the USB port. */
#define mainUSB_TX_FREQUENCY		        ( 100 / portTICK_RATE_MS )

/* The string that is transmitted down the USB port. */
#define mainFIRST_TX_CHAR			'a'
#define mainLAST_TX_CHAR			'z'

static void prvSetupHardware( void );

int main( void )
{
  prvSetupHardware();

  memset(&trspistat, 0, sizeof(trspistat));
  Pin a = PA8;
  trspistat.leds[0].pioled = a ;

  //  vStartProximitySensorTask( tskIDLE_PRIORITY + 1 );
  vStartLocatorTask( tskIDLE_PRIORITY + 1 );
  xTaskCreate( vUSBCDCTask, ( signed portCHAR * ) "USB", mainUSB_TASK_STACK, NULL, mainUSB_PRIORITY, NULL );

  vTaskStartScheduler();

  return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
  portDISABLE_INTERRUPTS();
	
  AT91C_BASE_AIC->AIC_EOICR = 0;
  PIO_Configure(pins, PIO_LISTSIZE(pins));
  //  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOA;
  //  adcinit();
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
  static portTickType xLastTx = 0;
  portCHAR cTxByte;
  
  /* The idle hook simply sends a string of characters to the USB port.
     The characters will be buffered and sent once the port is connected. */
  trspistat.counter = xTaskGetTickCount();
  /*if( ( trspistat.counter - xLastTx ) > mainUSB_TX_FREQUENCY )
    {
    xLastTx = xTaskGetTickCount();
    for( cTxByte = mainFIRST_TX_CHAR; cTxByte <= mainLAST_TX_CHAR; cTxByte++ )
    {
    vUSBSendByte( cTxByte );
    }
    }*/
  
  int i = 0;
  
  for (i; i<LOC_NUMLEDS; i++)
    {
      if ((trspistat.leds[i].changed)==1)
	{
	  if ((trspistat.leds[i].state)==0)
	    {
	      trspistat.leds[i].changed = 0;
	      PIO_Clear(&trspistat.leds[i].pioled);
	    }
	  else
	    {
	      trspistat.leds[i].changed = 0;
	      PIO_Set(&trspistat.leds[i].pioled);
	    }
	}
    }
}
