#include "FreeRTOS.h"
#include "proximity_sensor.h"
#include "proximity_sensor_task.h"
#include "common.h"
#include "task.h"

#define ledSTACK_SIZE		configMINIMAL_STACK_SIZE

static void vProximitySensorTask( void *pvParameters );

/*-----------------------------------------------------------*/

void vStartProximitySensorTask( unsigned portBASE_TYPE uxPriority )
{
unsigned portBASE_TYPE uxLEDTask;

/* Spawn the task. */
 xTaskCreate( vProximitySensorTask, "ProximitySensor", ledSTACK_SIZE, ( void * ) NULL, uxPriority, ( xTaskHandle * ) NULL );
}
/*-----------------------------------------------------------*/

static void vProximitySensorTask( void *pvParameters )
{
  /* Save the context of the interrupted task. */
  for(;;)
    {
      /* Delay */
      vTaskDelay( 10 / portTICK_RATE_MS );
      trspistat.adcvalue = adcgetvalue();
    }
}
