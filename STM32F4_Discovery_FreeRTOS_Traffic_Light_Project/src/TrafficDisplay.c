/*
 * TrafficDisplay.c
 *
 *  Created on: Feb 25, 2019
 */


#include "TrafficDisplay.h"


/*  Traffic light task: This task controls the timing of the traffic light. This timing is
	affected by the load of the traffic which is received from the traffic flow
	adjustment task.
*/
void TrafficDisplayTask ( void *pvParameters )
{
	//get value from traffic creator
	uint16_t car_value = 0;                            // start first car as no car
	uint16_t light_colour = 1;                         // start light as green
    uint16_t currentactiveprelighttraffic[8] = {0};    // 0 is newest element, 7 is at the traffic light
    uint16_t newactiveprelighttraffic[8] = {0};

	while(1)
	{
		// Update local car and light variables
		if( xSemaphoreTake( xMutexCars, ( TickType_t ) 10 ) == pdTRUE )       // get current car value
		{
			car_value = g_car_value;                                          // Update local variable with new car value (1 = car present, 0 = no car)
			xSemaphoreGive( xMutexCars );                                     // Return the semaphore as we are done reading the car value global variable
			printf("DisplayTask: Accessed xMutexCars, updated local car_value:  %u. \n", car_value );
		}
		else
		{
			printf("DisplayTask: xMutexCars unavailable \n");
		}

		if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE )       // can't wait for semaphore in callback, so ticks to wait is 0
	    {
			light_colour = g_light_colour;					                  // Update local variable with new light colour (1 = green, 0 = red)
			xSemaphoreGive( xMutexLight );                                    // Return the semaphore as we are done reading the light colour global variable
			printf("DisplayTask: Updated light colour: %u. (1 = green, 0 = red) \n", light_colour);
	    }
		else
		{
			printf("DisplayTask: xMutexLight unavailable \n");
		}


		if(light_colour == 1)		                                          // light is green, shift values normally
		{
			printf("DisplayTask: Light is green, shifting normally. \n ");

			ShiftRegisterValuePreLight(car_value);                            // Add the new car value on the road
			ShiftRegisterValuePostLight(currentactiveprelighttraffic[7]);     // Shift the car passing through the light to past the light

			newactiveprelighttraffic[0] = car_value;                          // Update the active car list with the new car value

			for (uint16_t i = 1; i != 8; i++)                                 // Shift the values from the current list to the active list
			{
				newactiveprelighttraffic[i] = currentactiveprelighttraffic[i-1];
			}
		}
		else if(light_colour == 0)		                                               // light is red, perform full shift-register refresh
		{
			printf("DisplayTask: Light is red, doing fast shift. \n ");

			// need to account for new value, and not push off cars. Prepare data.
			uint16_t encounteredzero = 0;

			for (uint16_t i = 7; i != 0; i--)                                          // Search through the traffic list by decrementing, looking for the first 0
			{
	            if(currentactiveprelighttraffic[i] == 0)                               // Find a zero in the active traffic. If it exists, set the encounteredzero flag
	            {
	            	encounteredzero = 1;                                               // Set encounteredzero flag high
	            	newactiveprelighttraffic[0] = car_value;                           // A zero exists in the active traffic (traffic isn't full), so shifting new car on.
	            }

	            if(encounteredzero == 1)                                               // If zero is found, shift the remaining values normally.
	            {
	            	newactiveprelighttraffic[i] = currentactiveprelighttraffic[i-1];
	            }
	            else                                                                   // Zero not found yet, so no shifting occurs as cars don't move on a red light
	            {
	            	newactiveprelighttraffic[i] = currentactiveprelighttraffic[i];
	            }

			}// end data preparing loop


			for (uint16_t i = 7; i >= 0 ; i--)                             // data is prepared, shift out new data (i = 7 is car closest to the light)
			{
				ShiftRegisterValuePreLight(newactiveprelighttraffic[i] );
				if(i == 0)                                                 // Due to using a unsigned integer, quit after i = 0, else i will overflow to 65535, and never exit the loop
				{
					break;
				}
			}
			ShiftRegisterValuePostLight(0);                                 // Ensure to move the cars after the traffic light, even though no new ones can pass.
		}


		// update current values for next iteration
		for(uint16_t i = 0; i != 8; i++)
		{
			currentactiveprelighttraffic[i] = newactiveprelighttraffic[i];
		}

		vTaskDelay(500);
	}
} // end Traffic_Display_Task
