/*
 * TrafficLight.c
 *
 *  Created on: Feb 25, 2019
 *      Author: brendanb
 */

#include "TrafficLight.h"


void vGreenLightTimerCallback( xTimerHandle xTimer )
{
	// green light time is up, change the light to red
	// set some flag or something for the display controller
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN);        // turn off green light
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW_PIN);         // turn on yellow light
	printf("GreenLightTimer: Green Light Off, Yellow light on. \n");
	xTimerStart( xYellowLightSoftwareTimer, 0 );
}
void vYellowLightTimerCallback( xTimerHandle xTimer )
	{
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW_PIN);       // turn off yellow light
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED_PIN);            // turn on red light
	xQueueReset( xQueue_handle_light_colour );                          // wipe the current light value on the queue
	uint16_t lightcolour = 0;                                               // 1 = green, 0 = red
	xQueueSend(xQueue_handle_light_colour, &lightcolour, 10);           // send the new light colour to the queue
	printf("YellowLightTimer: Yellow light off, red light on. colour = %d was sent to queue. \n", lightcolour);
	xTimerStart( xRedLightSoftwareTimer, 0 );
}
void vRedLightTimerCallback( xTimerHandle xTimer )
{
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED_PIN);          // turn off red light
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN);          // turn on green light
	xQueueReset( xQueue_handle_light_colour );                          // wipe the current light value on the queue
	uint16_t lightcolour = 1;                                               // 1 = green, 0 = red
	xQueueSend(xQueue_handle_light_colour, &lightcolour, 10);           // send the new light colour to the queue
	printf("RedLightTimer: Red light off, green light on. colour = %d was sent to queue. \n", lightcolour);
	xTimerStart( xGreenLightSoftwareTimer, 0 );
}

/*  Traffic light task: This task controls the timing of the traffic light. This timing is
	affected by the load of the traffic which is received from the traffic flow
	adjustment task.
*/


void TrafficLightTask ( void *pvParameters )
{
	// get speed
	// given speed, change timing

	//get value from traffic flow adjustment
	uint16_t new_speed_value = 0;
	uint16_t current_speed_value = 0;

	while(1)
		{
			if(xQueueReceive(xQueue_handle_speed_light, &new_speed_value, 10))
			{
				xQueueSend(xQueue_handle_speed_light, &new_speed_value, 10); // send back the value onto the queue for future use
				// print the received value to console
				printf("LightTask: received the value %u. \n", new_speed_value );

			    if(current_speed_value !=  new_speed_value)
			    { // speed changed, changed timer
			    	if(xTimerIsTimerActive( xGreenLightSoftwareTimer ))
			    	{
			    		xTimerStop(xGreenLightSoftwareTimer, 0);
				    	xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 2000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 );
				    	xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 500 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 );
				    	xTimerStop(xRedLightSoftwareTimer, 0);
				    	//xTimerStart( xGreenLightSoftwareTimer, 0 );
			    	}
			    	else if(xTimerIsTimerActive( xYellowLightSoftwareTimer ))
			    	{
			    		//xTimerStop(xYellowLightSoftwareTimer, 0);
				    	xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 2000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 );
				    	xTimerStop(xGreenLightSoftwareTimer, 0);
				    	xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 500 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 );
				    	xTimerStop(xRedLightSoftwareTimer, 0);
				    	//xTimerStart( xYellowLightSoftwareTimer, 0 );
			    	}
			    	else if(xTimerIsTimerActive( xRedLightSoftwareTimer ))
			    	{
			    		xTimerStop(xRedLightSoftwareTimer, 0);
				    	xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 2000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 );
				    	xTimerStop(xGreenLightSoftwareTimer, 0);
				    	xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 500 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 );
				    	//xTimerStart( xRedLightSoftwareTimer, 0 );
			    	}


			    }

			}
			vTaskDelay(500);
		}
} // end Traffic_Light_Task
