/*
 * TrafficCreator.c
 *
 *  Created on: Feb 25, 2019
 *      Author: brendanb
 */

#include "TrafficCreator.h"


/*  Traffic creator task: This task generates random traffic with a rate that is
	based on the potentiometer value reading. This value is received from the traffic
	flow adjustment task. The created traffic is sent to the task that displays the flow
	of the cars on the road.
 */

void TrafficCreatorTask ( void *pvParameters )
{
	uint16_t received;
	uint16_t send;

	while(1)
		{
			if(xQueueReceive(xQueue_handle_speed_creator, &received, 10))
			{
				xQueueSend(xQueue_handle_speed_creator, &received, 10);	// print the received value to console

				printf("CreatorTask: received  %u. \n", received );

				/* compute the value for the display (0/1)
				 * received should be a value 1-8
				 * generate random number range[0:100]
				 * if the random number is below 100/(8 - value from traffic flow task) create a car
				 * if the value from traffic flow task is high, there is a higher probability of a car being created
				*/
				send = (rand() % 100 ) < 100/(8 - received);

				if(xQueueSend(xQueue_handle_display_traffic, &send, 10))	// send the display value to the display queue
				{
					printf("CreatorTask: sending the value %d to display queue \n", send);
				}
				else
				{
					printf("CreatorTask: error Nothing to send\n");
				}

			}
			else
			{
				printf("CreatorTask: Nothing in the Speed Queue");
			}
			vTaskDelay(500);
		}
} // end Traffic_Creator_Task
