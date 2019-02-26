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
	uint16_t light_value;
    uint32_t currentactiveprelighttraffic[8]; // 0 is newest element, 7 is at the traffic light
    uint32_t newactiveprelighttraffic[8];

	while(1)
		{
			if(xQueueReceive(xQueue_handle_display_traffic, &car_value, 10)) 	// if there are values in the Display traffic queue (sent by traffic creator task)
			{
				xQueueSend(xQueue_handle_display_traffic, &car_value, 10);
				printf("DisplayTask: car = %d \n", car_value );

				if(xQueueReceive(xQueue_handle_light_colour, &light_value, 10))	//light colour queue has an item
				{
					//NOT RECEIVING GREEN LIGHT VALUE ONLY RED LIGHT???
					xQueueSend(xQueue_handle_light_colour, &light_value, 10); // put back the current light value for future use
					printf("DisplayTask: light colour = %d \n", light_value);

					if(xQueueReceive(xQueue_handle_prelight_active_traffic, &currentactiveprelighttraffic, 10))		// If there are values in the prelight queue
					{
						printf("DisplayTask: received the value %u. \n", car_value ); // print the received value
						newactiveprelighttraffic[0] = car_value;
					}

					if(light_value == 1)		// light is green, shift values normally
					{
						printf("DisplayTask: Light is green, shifting normally. \n ");

						ShiftRegisterValuePreLight(car_value);
						ShiftRegisterValuePostLight(currentactiveprelighttraffic[7]);

						for (int i = 1; i != 8; i++)
						{
							newactiveprelighttraffic[i] = currentactiveprelighttraffic[i-1];
						}
					}
					else if(light_value == 0)		// light is red
					{
						printf("DisplayTask: Light is red, doing fast shift. \n ");

						// need to account for new value, and not push off cars
						int shiftcounter = 0;
						int encounteredzero = 0;
					    for (int i = 7; i != 1; i--)
					    {
	                        if(currentactiveprelighttraffic[i] == 0 && encounteredzero == 0)
	                        {
	                        	shiftcounter = 1;
	                        }

	                        newactiveprelighttraffic[i] = currentactiveprelighttraffic[i-shiftcounter]; // grab the next value

	                        if(shiftcounter == 1 && encounteredzero == 0)
	                        {
	                        	encounteredzero = 1;
	                        	i--;
	                        }

					    }// end for

					    for (int i = 7; i != 1; i--)
					    {
					    	ShiftRegisterValuePreLight(newactiveprelighttraffic[i]);
					    }


					} // else red
					else
					{
						printf("light is not red or green??\n");
					}

				}

					ShiftRegisterValuePostLight(0); //nothing go threw traffic light, so no car.

			} // end light value else



			xQueueSend(xQueue_handle_prelight_active_traffic, &newactiveprelighttraffic, 10);
			printf("DisplayTask: pre-light traffic sent to queue \n");


			vTaskDelay(500);
		}
} // end Traffic_Display_Task
