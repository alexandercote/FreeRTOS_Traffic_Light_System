/*
 * TrafficLight.c
 *
 *  Created on: Feb 25, 2019
 *      Author: Alex Cote
 */

#include "TrafficLight.h"


void vGreenLightTimerCallback( xTimerHandle xTimer ) // green light time is up, change the light to yellow (red)
{
	printf("GreenLightTimerCallback: Green light off, yellow light on. \n");
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN);        // turn off green light
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW_PIN);         // turn on yellow light

	if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE ) // can't wait for semaphore in callback, so ticks to wait is 0
    {
		g_light_colour = 0;											// 1 = green, 0 = red
		xSemaphoreGive( xMutexLight );
		printf("GreenLightTimerCallback: Updated light colour to red. \n");
    }

	xTimerStart( xYellowLightSoftwareTimer, 0 );
}
void vYellowLightTimerCallback( xTimerHandle xTimer ) // yellow light time is up, change the light to red. Note Yellow light is like red.
	{
	printf("YellowLightTimerCallback: Yellow light off, red light on. \n");
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW_PIN);       // turn off yellow light
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED_PIN);            // turn on red light

	xTimerStart( xRedLightSoftwareTimer, 0 );
}
void vRedLightTimerCallback( xTimerHandle xTimer )
{
	printf("RedLightTimerCallback: Red light off, green light on. \n");
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED_PIN);          // turn off red light
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN);          // turn on green light

	if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE ) // can't wait for semaphore in callback, so ticks to wait is 0
    {
		g_light_colour = 1;											// 1 = green, 0 = red
		xSemaphoreGive( xMutexLight );
		printf("RedLightTimerCallback: Updated light colour to green. \n");
    }

	xTimerStart( xGreenLightSoftwareTimer, 0 );
}

/*  Traffic light task: This task controls the timing of the traffic light. This timing is
	affected by the load of the traffic which is received from the traffic flow
	adjustment task.
*/


void TrafficLightTask ( void *pvParameters )
{

	uint16_t new_speed_value = 0;
	uint16_t current_speed_value = 0;

	while(1)
	{

		if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE ) // get flowrate semaphore to update with new traffic flowrate
	    {
			new_speed_value = g_flowrate;
			xSemaphoreGive( xMutexFlow );
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
				xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1000 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 );     // starts the red timer    (don't want it started)
				xTimerStop(xRedLightSoftwareTimer, 0);                                                                        // stop the red timer
			}
			else if(xTimerIsTimerActive( xYellowLightSoftwareTimer ))
			{
				xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 );  // starts the green timer (don't want it started)
				xTimerStop(xGreenLightSoftwareTimer, 0);                                                                      // stop the green timer
				xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1000 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 );     // starts the red timer    (don't want it started)
				xTimerStop(xRedLightSoftwareTimer, 0);                                                                        // stop the red timer
			}
			else if(xTimerIsTimerActive( xRedLightSoftwareTimer ))
			{
				xTimerStop(xRedLightSoftwareTimer, 0);																		  // stop the red timer to change the period
				xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 );  // starts the green timer (don't want it started)
				xTimerStop(xGreenLightSoftwareTimer, 0);																	  // stop the green timer since red is on
				xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1000 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 );     // starts the red timer again
			}
		} // end if(current_speed_value !=  new_speed_value)

		current_speed_value = new_speed_value; // update speed value

		vTaskDelay(500);
	}
} // end Traffic_Light_Task
