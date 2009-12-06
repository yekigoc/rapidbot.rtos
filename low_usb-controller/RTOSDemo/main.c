/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "partest.h"
#include "USB-CDC.h"

#include "pio/pio.h"
#include "pwm/pwm.h"

#include "adc/adc.h"

#include "proximity_sensor/proximity_sensor_task.h"

/* Priorities for the demo application tasks. */
#define mainUSB_PRIORITY			( tskIDLE_PRIORITY + 1 )
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

  trspistat.counter = 0;
  trspistat.usbinited = 0;
  trspistat.dutycycle1 = 0;
  trspistat.changecycle1 = 0;
  trspistat.dutycycle2 = 0;
  trspistat.changecycle2 = 0;
  trspistat.led = 0;

  xTaskCreate( vUSBCDCTask, ( signed portCHAR * ) "USB", mainUSB_TASK_STACK, NULL, mainUSB_PRIORITY, NULL );
  vStartProximitySensorTask( tskIDLE_PRIORITY + 1 );

  vTaskStartScheduler();

  return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
  portDISABLE_INTERRUPTS();
	
  AT91C_BASE_AIC->AIC_EOICR = 0;

  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOA;

  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PWMC;
  PWMC_ConfigureClocks(PWM_FREQUENCY * MAX_DUTY_CYCLE, 0, BOARD_MCK);
  PIO_Configure(pins, PIO_LISTSIZE(pins));
  	
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOA;

  PWMC_ConfigureChannel(CHANNEL_PWM_1, AT91C_PWMC_CPRE_MCKA, 0, 0);
  PWMC_SetPeriod(CHANNEL_PWM_1, MAX_DUTY_CYCLE);
  PWMC_SetDutyCycle(CHANNEL_PWM_1, MIN_DUTY_CYCLE);

  PWMC_ConfigureChannel(CHANNEL_PWM_2, AT91C_PWMC_CPRE_MCKA, 0, 0);
  PWMC_SetPeriod(CHANNEL_PWM_2, MAX_DUTY_CYCLE);
  PWMC_SetDutyCycle(CHANNEL_PWM_2, MIN_DUTY_CYCLE);

  PWMC_EnableChannel(CHANNEL_PWM_1);
  PWMC_EnableChannel(CHANNEL_PWM_2);

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
 
 trspistat.counter++;
 if (trspistat.changecycle1==1)
   {
     PWMC_SetDutyCycle(CHANNEL_PWM_1, trspistat.dutycycle1);
     trspistat.changecycle1=0;
   }
 if (trspistat.changecycle2==1)
   {
     PWMC_SetDutyCycle(CHANNEL_PWM_2, trspistat.dutycycle2);
     trspistat.changecycle2=0;
   }

 if (trspistat.led==1)
   {
     Pin led=PA8;
     PIO_Clear(&led);
   }
 else
   {
     Pin led=PA8;
     PIO_Set(&led);
   }
 
}
