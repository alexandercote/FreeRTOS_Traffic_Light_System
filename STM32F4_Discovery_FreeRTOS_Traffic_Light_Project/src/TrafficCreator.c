/*
 * TrafficCreator.c
 *
 *  Created on: Feb 25, 2019
 */


#include "TrafficCreator.h"


/*  Traffic creator task: This task generates random traffic with a rate that is
	based on the potentiometer value reading. This value is received from the traffic
	flow adjustment task. The created traffic is sent to the task that displays the flow
	of the cars on the road.
 */

void TrafficCreatorTask ( void *pvParameters )
{
	uint16_t flowrate = 4;
	uint16_t car_value = 0;

	while(1)
	{
		printf("CreatorTask: Starting flowrate:  %u. \n", flowrate );
		if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE ) // get flowrate semaphore to update with new traffic flowrate
		{
			flowrate = g_flowrate;
			xSemaphoreGive( xMutexFlow );
			printf("CreatorTask: Accessed xMutexFlow, updated flowrate:  %u. \n", flowrate );
		}
		else
		{
			printf("CreatorTask: xMutexFlow unavailable \n");
		}


		/*
		 * compute the value for the display (0/1)
		 * received should be a value 1-8
		 * generate random number range[0:100]
		 * if the random number is below 100/(8 - value from traffic flow task) create a car
		 * if the value from traffic flow task is high, there is a higher probability of a car being created
		*/
		car_value = (rand() % 100 ) < 100/(8 - flowrate);

		printf("CreatorTask: car value updated to:  %u  \n", car_value);


		if( xSemaphoreTake( xMutexCars, ( TickType_t ) 10 ) == pdTRUE ) // get flowrate semaphore to update with new traffic flowrate
		{
			g_car_value = car_value;
			xSemaphoreGive( xMutexCars );
			printf("CreatorTask: Accessed xMutexCars, updated g_car_value:  %u. \n", car_value );
		}
		else
		{
			printf("CreatorTask: xMutexCars unavailable \n");
		}

		vTaskDelay(500);
	}
} // end Traffic_Creator_Task
