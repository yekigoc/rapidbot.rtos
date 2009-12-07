#include "FreeRTOS.h"
#include "proximity_sensor.h"
#include "proximity_sensor_task.h"
#include "common.h"
#include "task.h"

#define adcSTACK_SIZE		(200)

void vProximitySensorTask( void *pvParameters );

/*-----------------------------------------------------------*/

void vStartProximitySensorTask( unsigned portBASE_TYPE uxPriority )
{
unsigned portBASE_TYPE uxLEDTask;

/* Spawn the task. */
 xTaskCreate( vProximitySensorTask, "Prox", adcSTACK_SIZE, ( void * ) NULL, uxPriority, ( xTaskHandle * ) NULL );
}
/*-----------------------------------------------------------*/

void vProximitySensorTask( void *pvParameters )
{
  adcinit();
  /* Save the context of the interrupted task. */
  for(;;)
    {
      //      Pin led=PA8;
      //      PIO_Clear(&led);
      /* Delay */
      //      vTaskDelay( 5 / portTICK_RATE_MS );
      trspistat.adcvalue = adcgetvalue();
      //      PIO_Set(&led);
      //      vTaskDelay( 5 / portTICK_RATE_MS );
    }
}
