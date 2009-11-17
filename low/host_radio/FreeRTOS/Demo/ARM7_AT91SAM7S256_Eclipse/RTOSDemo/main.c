/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo application includes. */
#include "partest.h"
#include "USB-CDC.h"
//#include "uIP_Task.h"
/*#include "BlockQ.h"
#include "blocktim.h"
#include "flash.h"
#include "QPeek.h"
#include "dynamic.h"*/

#include "tr24a/tr24a.h"
#include "pio/pio.h"

/* Priorities for the demo application tasks. */
#define mainUIP_PRIORITY			( tskIDLE_PRIORITY + 2 )
#define mainUSB_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainUSB_TASK_STACK			( 200 )
#define mainBLOCK_Q_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainFLASH_PRIORITY                      ( tskIDLE_PRIORITY + 2 )
#define mainGEN_QUEUE_TASK_PRIORITY		( tskIDLE_PRIORITY ) 
#define mainWEBSERVER_PRIORITY                  ( tskIDLE_PRIORITY + 2 )

/* The task allocated to the uIP task is large to account for its use of the
   sprintf() library function.  Use of a cut down printf() library would allow
   the stack usage to be greatly reduced. */
#define mainUIP_TASK_STACK_SIZE		        ( configMINIMAL_STACK_SIZE * 6 )

/* The rate at which the idle hook sends data to the USB port. */
#define mainUSB_TX_FREQUENCY		        ( 100 / portTICK_RATE_MS )

/* The string that is transmitted down the USB port. */
#define mainFIRST_TX_CHAR			'a'
#define mainLAST_TX_CHAR			'z'


/* The LED toggle by the tick hook should an error have been found in a task. */
#define mainERROR_LED						( 1 )

//xLCDParameters * lcdpar;
int trinited;
static void prvSetupHardware( void );


int main( void )
{
  prvSetupHardware();

  trspistat.spiisrcalls = 0;
  trspistat.trinited = 0;
  trspistat.counter = 0;
  trspistat.usbinited = 0;

  xTaskCreate( vUSBCDCTask, ( signed portCHAR * ) "USB", mainUSB_TASK_STACK, NULL, mainUSB_PRIORITY, NULL );

  vTaskStartScheduler();

  return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
  portDISABLE_INTERRUPTS();
	
  AT91C_BASE_AIC->AIC_EOICR = 0;
	
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOA;

  tr24_init();
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
static portTickType xLastTx = 0;
portCHAR cTxByte;

/* The idle hook simply sends a string of characters to the USB port.
   The characters will be buffered and sent once the port is connected. */
 if( ( xTaskGetTickCount() - xLastTx ) > mainUSB_TX_FREQUENCY )
   {
     xLastTx = xTaskGetTickCount();
     for( cTxByte = mainFIRST_TX_CHAR; cTxByte <= mainLAST_TX_CHAR; cTxByte++ )
       {
	 vUSBSendByte( cTxByte );
       }
   }

 trspistat.counter = trspistat.counter + 1;
 trspistat.spistatreg=AT91C_BASE_SPI->SPI_SR;

 if ((trspistat.trinited == 0) && (trspistat.usbinited == 1))
   {
     portENTER_CRITICAL();
     trspistat.trinited = 11;
     Pin res = TRRESET_PIO;
     UTIL_WaitTimeInUs(BOARD_MCK, 1000);
     PIO_Set(&res);
     
     UTIL_WaitTimeInUs(BOARD_MCK, 100);
     
     trspistat.spistatreg=AT91C_BASE_SPI->SPI_SR;
     tr24_initframer();
     UTIL_WaitTimeInMs(BOARD_MCK, 50);
     trspistat.spistatreg=AT91C_BASE_SPI->SPI_SR;
     tr24_initrfic();
     trspistat.spistatreg=AT91C_BASE_SPI->SPI_SR;
     portEXIT_CRITICAL();
     UTIL_WaitTimeInUs(BOARD_MCK, 100);
     
     char a[2]="AA";
     tr24_writefifo(a, 2);
     //tr24_readfifo();
     PIO_Clear(&res);
     trspistat.usbinited = 0;
   }
 
}
