/*
 * TrafficDisplay.c
 *
 *  Created on: Feb 25, 2019
 *      Author: brendanb
 */

#include "TrafficDisplay.h"




/*  Traffic light task: This task controls the timing of the traffic light. This timing is
	affected by the load of the traffic which is received from the traffic flow
	adjustment task.
*/
void TrafficDisplayTask ( void *pvParameters )
{
	//get value from traffic creator
	uint16_t car_value = 0;
	uint16_t light_colour = 1;
    uint16_t currentactiveprelighttraffic[8] = {0}; // 0 is newest element, 7 is at the traffic light
    uint16_t newactiveprelighttraffic[8] = {0};

	while(1)
	{
		if( xSemaphoreTake( xMutexCars, ( TickType_t ) 10 ) == pdTRUE ) // get current car value
		{
			car_value = g_car_value;
			xSemaphoreGive( xMutexCars );
			printf("DisplayTask: Accessed xMutexCars, updated local car_value:  %u. \n", car_value );
		}

		if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE ) // can't wait for semaphore in callback, so ticks to wait is 0
	    {
			light_colour = g_light_colour;					// 1 = green, 0 = red
			xSemaphoreGive( xMutexLight );
			printf("DisplayTask: Updated light colour: %u. (1 = green, 0 = red) \n", light_colour);
	    }

		if(light_colour == 1)		// light is green, shift values normally
		{
			printf("DisplayTask: Light is green, shifting normally. \n ");

			ShiftRegisterValuePreLight(car_value);
			ShiftRegisterValuePostLight(currentactiveprelighttraffic[7]);

			newactiveprelighttraffic[0] = car_value;

			for (uint16_t i = 1; i != 8; i++)
			{
				newactiveprelighttraffic[i] = currentactiveprelighttraffic[i-1];
			}
		}
		else if(light_colour == 0)		// light is red
		{
			printf("DisplayTask: Light is red, doing fast shift. \n ");

			// need to account for new value, and not push off cars. Prepare data.
			uint16_t encounteredzero = 0;

			for (uint16_t i = 7; i != 0; i--)
			{
	            if(currentactiveprelighttraffic[i] == 0)
	            {
	            	encounteredzero = 1;
	            	newactiveprelighttraffic[0] = car_value;
	            }

	            if(encounteredzero == 1)
	            {
	            	newactiveprelighttraffic[i] = currentactiveprelighttraffic[i-1];
	            }
	            else
	            {
	            	newactiveprelighttraffic[i] = currentactiveprelighttraffic[i];
	            }

			}// end for


			for (uint16_t i = 7; i >= 0 ; i--)
			{
				ShiftRegisterValuePreLight(newactiveprelighttraffic[i] );
				if(i == 0)
				{
					break;
				}
			}
			ShiftRegisterValuePostLight(0);
		}

		// update current values for next iteration
		for(uint16_t i = 0; i != 8; i++)
		{
			currentactiveprelighttraffic[i] = newactiveprelighttraffic[i];
		}

		vTaskDelay(500);
	}
} // end Traffic_Display_Task
