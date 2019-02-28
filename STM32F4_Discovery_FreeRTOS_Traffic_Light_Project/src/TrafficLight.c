/*
 * TrafficLight.c
 *
 *  Created on: Feb 25, 2019
 */


#include "TrafficLight.h"


// Callback for the xGreenLightSoftwareTimer
void vGreenLightTimerCallback( xTimerHandle xTimer ) // green light time is up, change the light to yellow (red)
{
	printf("GreenLightTimerCallback: Green light off, yellow light on. \n");
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN);        // turn off green light
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW_PIN);         // turn on yellow light

	if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE ) // can't wait for semaphore in callback, so ticks to wait is 0
    {
		g_light_colour = 0;											// Update global light value to red as green is done (1 = green, 0 = red); yellow light is treated as red.
		xSemaphoreGive( xMutexLight );                              // Updated global light value, release light mutex
		printf("GreenLightTimerCallback: Updated light colour to red. \n");
    }
	else
	{
		printf("GreenLightTimerCallback: xMutexLight unavailable \n");
	}

	xTimerStart( xYellowLightSoftwareTimer, 0 );                    // Green light finished, light colour changed, light state changed, start yellow light timer
}

// Callback for the xYellowLightSoftwareTimer
void vYellowLightTimerCallback( xTimerHandle xTimer ) // yellow light time is up, change the light to red. Note: Yellow light is like red.
	{
	printf("YellowLightTimerCallback: Yellow light off, red light on. \n");
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW_PIN);       // turn off yellow light
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED_PIN);            // turn on red light

	xTimerStart( xRedLightSoftwareTimer, 0 );                           // Yellow light finished, light colour changed, start red light timer
}

// Callback for the xRedLightSoftwareTimer
void vRedLightTimerCallback( xTimerHandle xTimer )
{
	printf("RedLightTimerCallback: Red light off, green light on. \n");
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED_PIN);          // turn off red light
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN);          // turn on green light

	if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE ) // can't wait for semaphore in callback, so ticks to wait is 0
    {
		g_light_colour = 1;											// Update global light value to green as red is done. (1 = green, 0 = red)
		xSemaphoreGive( xMutexLight );                              // Updated global light value, release light mutex
		printf("RedLightTimerCallback: Updated light colour to green. \n");
    }
	else
	{
		printf("RedLightTimerCallback: xMutexLight unavailable \n");
	}

	xTimerStart( xGreenLightSoftwareTimer, 0 );                     // Red light finished, light colour changed, light state changed, start green light timer
}


/*  Traffic light task: This task controls the timing of the traffic light. This timing is
	affected by the load of the traffic which is received from the traffic flow
	adjustment task.
*/
void TrafficLightTask ( void *pvParameters )
{

	uint16_t new_speed_value = 4;           // Set default speed value to 4, update it when an ADC value is read.
	uint16_t current_speed_value = 0;       // Set to 0 to force an update of the timers.

	while(1)
	{
		// Update local flow/speed variable
		if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE ) // get flowrate semaphore to update with new traffic flowrate
	    {
			new_speed_value = g_flowrate;                               // Update local variable with new flow/speed value
			xSemaphoreGive( xMutexFlow );                               // Updated local speed value, release flow mutex
			printf("LightTask: Accessed xMutexFlow, updated local flowrate:  %u.\n", new_speed_value );
	    }
		else
		{
			printf("LightTask: xMutexFlow unavailable \n");
		}


		if(current_speed_value !=  new_speed_value) // speed changed, changed timer
		{
			if(xTimerIsTimerActive( xGreenLightSoftwareTimer ))
			{
				xTimerStop(xGreenLightSoftwareTimer, 0);                                                                      // stop the green timer to change the period
				xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 );  // starts the green timer
				xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1500 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 );     // starts the red timer    (don't want it started)
				xTimerStop(xRedLightSoftwareTimer, 0);                                                                        // stop the red timer
			}
			else if(xTimerIsTimerActive( xYellowLightSoftwareTimer ))
			{
				xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 );  // starts the green timer (don't want it started)
				xTimerStop(xGreenLightSoftwareTimer, 0);                                                                      // stop the green timer
				xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1500 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 );     // starts the red timer    (don't want it started)
				xTimerStop(xRedLightSoftwareTimer, 0);                                                                        // stop the red timer
			}
			else if(xTimerIsTimerActive( xRedLightSoftwareTimer ))
			{
				xTimerStop(xRedLightSoftwareTimer, 0);																		  // stop the red timer to change the period
				xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 );  // starts the green timer (don't want it started)
				xTimerStop(xGreenLightSoftwareTimer, 0);																	  // stop the green timer since red is on
				xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1500 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 );     // starts the red timer again
			}
		} // end if(current_speed_value !=  new_speed_value)

		current_speed_value = new_speed_value; // update speed value

		vTaskDelay(500);
	}
} // end Traffic_Light_Task
