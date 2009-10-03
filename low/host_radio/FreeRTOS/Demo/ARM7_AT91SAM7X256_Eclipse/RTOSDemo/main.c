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
#include "BlockQ.h"
#include "blocktim.h"
#include "flash.h"
#include "QPeek.h"
#include "dynamic.h"

//#include "BasicWEB.h"

/* lwIP includes. */
//#include "lwip/api.h" 

/* usr tasks */

#include "pwm/pwm.h"

//xLCDParameters * lcdpar;

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

static void prvSetupHardware( void );


//------------------------------------------------------------------------------
/// Wait time in ms
//------------------------------------------------------------------------------
void UTIL_Loop(unsigned int loop)
{
    while(loop--);	
}


void UTIL_WaitTimeInMs(unsigned int mck, unsigned int time_ms)
{
    register unsigned int i = 0;
    i = (mck / 1000) * time_ms;
    i = i / 3;
    UTIL_Loop(i);
}

//------------------------------------------------------------------------------
/// Wait time in us
//------------------------------------------------------------------------------
void UTIL_WaitTimeInUs(unsigned int mck, unsigned int time_us)
{
    volatile unsigned int i = 0;
    i = (mck / 1000000) * time_us;
    i = i / 3;
    UTIL_Loop(i);
}

/*-----------------------------------------------------------*/
unsigned int count;
unsigned int duty;
int fadein;
/*
 * Starts all the other tasks, then starts the scheduler.
 */
int main( void )
{
  /* Setup any hardware that has not already been configured by the low
     level init routines. */
  prvSetupHardware();

  //  UTIL_WaitTimeInMs(BOARD_MCK, 1000);
  //  UTIL_WaitTimeInUs(BOARD_MCK, 1000);

  //  

  PWMC_ConfigureChannel(CHANNEL_PWM_1, AT91C_PWMC_CPRE_MCKA, 0, 0);
  PWMC_SetPeriod(CHANNEL_PWM_1, MAX_DUTY_CYCLE);
  PWMC_SetDutyCycle(CHANNEL_PWM_1, MIN_DUTY_CYCLE);

  PWMC_ConfigureChannel(CHANNEL_PWM_2, AT91C_PWMC_CPRE_MCKA, 0, 0);
  PWMC_SetPeriod(CHANNEL_PWM_2, MAX_DUTY_CYCLE);
  PWMC_SetDutyCycle(CHANNEL_PWM_2, MIN_DUTY_CYCLE);

  PWMC_ConfigureChannel(CHANNEL_PWM_3, AT91C_PWMC_CPRE_MCKA, 0, 0);
  PWMC_SetPeriod(CHANNEL_PWM_3, MAX_DUTY_CYCLE);
  PWMC_SetDutyCycle(CHANNEL_PWM_3, MIN_DUTY_CYCLE);

  PWMC_ConfigureChannel(CHANNEL_PWM_4, AT91C_PWMC_CPRE_MCKA, 0, 0);
  PWMC_SetPeriod(CHANNEL_PWM_4, MAX_DUTY_CYCLE);
  PWMC_SetDutyCycle(CHANNEL_PWM_4, MIN_DUTY_CYCLE);

  
  /*  AIC_ConfigureIT(AT91C_ID_PWMC, 0, ISR_Pwmc);
  AIC_EnableIT(AT91C_ID_PWMC);
  PWMC_EnableChannelIt(CHANNEL_PWM_3);*/

  PWMC_EnableChannel(CHANNEL_PWM_1);  
  PWMC_EnableChannel(CHANNEL_PWM_2);
  PWMC_EnableChannel(CHANNEL_PWM_3);
  PWMC_EnableChannel(CHANNEL_PWM_4);

  InitLCD();
  LCDSettings();
  Backlight(BKLGHT_LCD_ON);

  count = 0;
  duty = MIN_DUTY_CYCLE;
  fadein = 1;
  
  /* Setup lwIP. */
  //    vlwIPInit();
  
  /* Create the lwIP task.  This uses the lwIP RTOS abstraction layer.*/
  //    sys_thread_new( vBasicWEBServer, ( void * ) NULL, mainWEBSERVER_PRIORITY );
  
  xTaskCreate( vUSBCDCTask, ( signed portCHAR * ) "USB", mainUSB_TASK_STACK, NULL, mainUSB_PRIORITY, NULL );
  /* Start the task that handles the TCP/IP and WEB server functionality. */
  //  xTaskCreate( vuIP_Task, "uIP", mainUIP_TASK_STACK_SIZE, NULL, mainUIP_PRIORITY, NULL );
  
  /* Also start the USB demo which is just for the SAM7. */
  
  /* Start the standard demo tasks. */
  
  //  vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
  //  vCreateBlockTimeTasks();
  //  vStartLEDFlashTasks( mainFLASH_PRIORITY );
  //  vStartGenericQueueTasks( mainGEN_QUEUE_TASK_PRIORITY );
  //  vStartQueuePeekTasks();   
  //  vStartDynamicPriorityTasks();
  
  //  lcdpar = vStartLCDTasks(mainFLASH_PRIORITY);
 
  /* Start the scheduler.
     
     NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
     The processor MUST be in supervisor mode when vTaskStartScheduler is
     called.  The demo applications included in the FreeRTOS.org download switch
     to supervisor mode prior to main being called.  If you are not using one of
     these demo application projects then ensure Supervisor mode is used here. */

      //      LCDclearbg (131,0,0,131,WHITE);

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
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOB;
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_EMAC;

  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PWMC;
  PWMC_ConfigureClocks(PWM_FREQUENCY * MAX_DUTY_CYCLE, 0, BOARD_MCK);
  PIO_Configure(pins, PIO_LISTSIZE(pins));

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
// term_print(ts);
	if( ( xTaskGetTickCount() - xLastTx ) > mainUSB_TX_FREQUENCY )
	{
		xLastTx = xTaskGetTickCount();
		for( cTxByte = mainFIRST_TX_CHAR; cTxByte <= mainLAST_TX_CHAR; cTxByte++ )
		{
		  vUSBSendByte( cTxByte );
		}		
	}
	      

      //      UTIL_WaitTimeInUs(BOARD_MCK, 100);

  // Interrupt on channel #1
  //  if ((AT91C_BASE_PWMC->PWMC_ISR & AT91C_PWMC_CHID2) == AT91C_PWMC_CHID2) {
      
      //      count++;
      
      // Fade in/out
      //      if (count == (PWM_FREQUENCY / (MAX_DUTY_CYCLE - MIN_DUTY_CYCLE))) 
      //	{
	  // Fade in
	if (fadein == 1)
	{
	  duty = duty+1;
	  if (duty==MAX_DUTY_CYCLE)
	    {
	      duty = MAX_DUTY_CYCLE;
	      fadein=0;
	    }
	}
      else
	{
	  duty = duty-1;
	  if (duty==MIN_DUTY_CYCLE)
	    {
	      duty = MIN_DUTY_CYCLE;
	      fadein=1;
	    }

	}
      PWMC_SetDutyCycle(CHANNEL_PWM_1, duty);
      PWMC_SetDutyCycle(CHANNEL_PWM_2, duty);
      PWMC_SetDutyCycle(CHANNEL_PWM_3, duty);
      PWMC_SetDutyCycle(CHANNEL_PWM_4, duty);
	  
      UTIL_WaitTimeInMs(BOARD_MCK, 10);
      
      //            PWMC_SetDutyCycle(CHANNEL_PWM, duty);
	  //    }

}


