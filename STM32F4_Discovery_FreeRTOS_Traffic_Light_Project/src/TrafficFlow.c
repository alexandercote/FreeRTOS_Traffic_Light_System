/*
 * TrafficFlow.c
 *
 *  Created on: Feb 25, 2019
 *      Author: brendanb
 */

#include "TrafficFlow.h"


/*  Traffic flow adjustment task: The traffic flow that enters the intersection is set
	by a potentiometer. This task reads the value of the potentiometer at proper
	intervals. The low resistance of the potentiometer corresponds to light traffic and
	a high resistance corresponds to heavy traffic. The reading by this task is sent
	and used by other tasks.
 */
void TrafficFlowAdjustmentTask ( void *pvParameters )
{
    uint16_t adc_value;
    uint16_t speed_adc_value;
    uint16_t current_speed_value = 0;
    uint16_t change_in_speed;

	while(1)
	{
		ADC_SoftwareStartConv(ADC1);		// wait for ADC to finish conversion

		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	// grab ADC value

		adc_value = ADC_GetConversionValue(ADC1);
		speed_adc_value = adc_value/512;

		if(speed_adc_value == 8)
		{
			speed_adc_value = 7;
		}

        change_in_speed = abs(speed_adc_value - current_speed_value);


	    if(change_in_speed !=  0) 	// will only do queue stuff if the speed changed.
	    {
	    	printf("FlowAdjustmentTask: change in speed = %d \n", change_in_speed);

	    	// speed changed, change value on queue

	    	xQueueReset( xQueue_handle_speed_creator ); // empty the queue, push new speed value onto the queue
	    	xQueueReset( xQueue_handle_speed_light );  // empty the queue, push new speed value onto the queue

	    	printf("FlowAdjustmentTask: ADC Value: %d = %d /7 \n", adc_value, speed_adc_value);

			if( xQueueSend(xQueue_handle_speed_creator, &speed_adc_value, 500))
			{
				printf("FlowAdjustmentTask: adc_value sent on speed creator queue.\n");
			}
			else
			{
				printf("FlowAdjustmentTask: Failed to send data on queue from TFA to TC tasks.\n");
			}

			if( xQueueSend(xQueue_handle_speed_light, &speed_adc_value, 500))
			{
				printf("FlowAdjustmentTask: adc_value sent to light speed queue.\n");
			}
			else
			{
				printf("FlowAdjustmentTask: Failed to send data on queue from TFA to TL tasks.\n");
			}
			current_speed_value = speed_adc_value; // save previous speed value

	    } // end if speed changed

        vTaskDelay(100);

	}
} // end Traffic_Flow_Adjustment_Task
