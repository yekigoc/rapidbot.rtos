/*
  FreeRTOS V5.4.2 - Copyright (C) 2009 Real Time Engineers Ltd.

  This file is part of the FreeRTOS distribution.

  FreeRTOS is free software; you can redistribute it and/or modify it	under 
  the terms of the GNU General Public License (version 2) as published by the 
  Free Software Foundation and modified by the FreeRTOS exception.
  **NOTE** The exception to the GPL is included to allow you to distribute a
  combined work that includes FreeRTOS without being obliged to provide the 
  source code for proprietary components outside of the FreeRTOS kernel.  
  Alternative commercial license and support terms are also available upon 
  request.  See the licensing section of http://www.FreeRTOS.org for full 
  license details.

  FreeRTOS is distributed in the hope that it will be useful,	but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along
  with FreeRTOS; if not, write to the Free Software Foundation, Inc., 59
  Temple Place, Suite 330, Boston, MA  02111-1307  USA.


  ***************************************************************************
  *                                                                         *
  * Looking for a quick start?  Then check out the FreeRTOS eBook!          *
  * See http://www.FreeRTOS.org/Documentation for details                   *
  *                                                                         *
  ***************************************************************************

  1 tab == 4 spaces!

  Please ensure to read the configuration and relevant port sections of the
  online documentation.

  http://www.FreeRTOS.org - Documentation, latest information, license and
  contact details.

  http://www.SafeRTOS.com - A version that is certified for use in safety
  critical systems.

  http://www.OpenRTOS.com - Commercial support, development, porting,
  licensing and training services.
*/

/*
  NOTE : Tasks run in System mode and the scheduler runs in Supervisor mode.
  The processor MUST be in supervisor mode when vTaskStartScheduler is
  called.  The demo applications included in the FreeRTOS.org download switch
  to supervisor mode prior to main being called.  If you are not using one of
  these demo application projects then ensure Supervisor mode is used.
*/

/*
 * This demo includes a (basic) USB mouse driver and a WEB server.  It is
 * targeted for the AT91SAM7X EK prototyping board which includes a small
 * joystick to provide the mouse inputs.  The WEB interface provides some basic
 * interactivity through the use of a check box to turn on and off an LED.
 *
 * main() creates the WEB server, USB, and a set of the standard demo tasks
 * before starting the scheduler.  See the online FreeRTOS.org documentation 
 * for more information on the standard demo tasks.  
 *
 * LEDs D1 to D3 are controlled by the standard 'flash' tasks - each will 
 * toggle at a different fixed frequency.
 *
 * A tick hook function is used to monitor the standard demo tasks - with LED
 * D4 being used to indicate the system status.  D4 toggling every 5 seconds
 * indicates that all the standard demo tasks are executing without error.  The
 * toggle rate increasing to 500ms is indicative of an error having been found
 * in at least one demo task.
 *
 * See the online documentation page that accompanies this demo for full setup
 * and usage information.
 */

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

/* Priorities for the demo application tasks. */
#define mainUIP_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define mainUSB_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainUSB_TASK_STACK			( 200 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainFLASH_PRIORITY                  ( tskIDLE_PRIORITY + 2 )
#define mainGEN_QUEUE_TASK_PRIORITY			( tskIDLE_PRIORITY ) 
#define mainWEBSERVER_PRIORITY      ( tskIDLE_PRIORITY + 2 )

/* The task allocated to the uIP task is large to account for its use of the
   sprintf() library function.  Use of a cut down printf() library would allow
   the stack usage to be greatly reduced. */
#define mainUIP_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 6 )

/* The rate at which the idle hook sends data to the USB port. */
#define mainUSB_TX_FREQUENCY		( 100 / portTICK_RATE_MS )

/* The string that is transmitted down the USB port. */
#define mainFIRST_TX_CHAR			'a'
#define mainLAST_TX_CHAR			'z'


/* The LED toggle by the tick hook should an error have been found in a task. */
#define mainERROR_LED						( 1 )

/*-----------------------------------------------------------*/

/*
 * Configure the processor for use with the Atmel demo board.  Setup is minimal
 * as the low level init function (called from the startup asm file) takes care
 * of most things.
 */

//xLCDParameters * lcdpar;
int trinited;
static void prvSetupHardware( void );


/*-----------------------------------------------------------*/
/*
 * Starts all the other tasks, then starts the scheduler.
 */
int main( void )
{
  /* Setup any hardware that has not already been configured by the low
     level init routines. */
  prvSetupHardware();

  trspistat.spiisrcalls = 0;
  trspistat.trinited = 0;
  trspistat.counter = 0;
  trspistat.usbinited = 0;

  xTaskCreate( vUSBCDCTask, ( signed portCHAR * ) "USB", mainUSB_TASK_STACK, NULL, mainUSB_PRIORITY, NULL );

  /* Start the scheduler.
     
     NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
     The processor MUST be in supervisor mode when vTaskStartScheduler is
     called.  The demo applications included in the FreeRTOS.org download switch
     to supervisor mode prior to main being called.  If you are not using one of
     these demo application projects then ensure Supervisor mode is used here. */
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler. */
  return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
  portDISABLE_INTERRUPTS();
	
  /* When using the JTAG debugger the hardware is not always initialised to
     the correct default state.  This line just ensures that this does not
     cause all interrupts to be masked at the start. */
  AT91C_BASE_AIC->AIC_EOICR = 0;
	
  /* Most setup is performed by the low level init function called from the
     startup asm file. */

  /* Enable the peripheral clock. */
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOA;

  tr24_init();
  //  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOB;

  /* Initialise the LED outputs for use by the demo application tasks. */
  //  vParTestInitialise();
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
	if ((trspistat.trinited == 0) && (trspistat.usbinited == 1))
	  {
	    //	    PIO_Clear(TRRESET_PIN);
	    //	    UTIL_WaitTimeInMs(BOARD_MCK, 2000);
	    trspistat.trinited = 11;
	    //	    vTaskDelay( 10 );	    
	    unsigned int data = 0x30;
	    data = (data & ~0x0100);
	    
	    // Wait for the transfer to complete
	    while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
	    
	    AT91C_BASE_SPI->SPI_TDR = data;
	    trspistat.trinited = 17;
	    /*	    SPI_Write(&trspi, TR24_NPCS, 0x30);
	    trspistat.trinited = 12;
	    SPI_Write(&trspi, TR24_NPCS, 0x9800);
	    trspistat.trinited = 1;
	    SPI_Write(&trspi, TR24_NPCS, 0x31);
	    SPI_Write(&trspi, TR24_NPCS, 0xFF8F);
	    trspistat.trinited = 2;
	    SPI_Write(&trspi, TR24_NPCS, 0x32);
	    SPI_Write(&trspi, TR24_NPCS, 0x8028);
	    trspistat.trinited = 3;
	    SPI_Write(&trspi, TR24_NPCS, 0x33);
	    SPI_Write(&trspi, TR24_NPCS, 0x8056);
	    trspistat.trinited = 4;
	    SPI_Write(&trspi, TR24_NPCS, 0x34);
	    SPI_Write(&trspi, TR24_NPCS, 0x4EF6);
	    trspistat.trinited = 5;
	    SPI_Write(&trspi, TR24_NPCS, 0x35);
	    SPI_Write(&trspi, TR24_NPCS, 0xF6F5);
	    trspistat.trinited = 6;
	    SPI_Write(&trspi, TR24_NPCS, 0x36);
	    SPI_Write(&trspi, TR24_NPCS, 0x185C);
	    trspistat.trinited = 7;
	    SPI_Write(&trspi, TR24_NPCS, 0x37);
	    SPI_Write(&trspi, TR24_NPCS, 0xD651);
	    trspistat.trinited = 8;
	    SPI_Write(&trspi, TR24_NPCS, 0x38);
	    SPI_Write(&trspi, TR24_NPCS, 0x4444);
	    trspistat.trinited = 9;
	    SPI_Write(&trspi, TR24_NPCS, 0x39);
	    SPI_Write(&trspi, TR24_NPCS, 0xE000);
	    trspistat.trinited = 10;*/
	  }

}


