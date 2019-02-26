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
    uint16_t adc_value = 0;
    uint16_t speed_adc_value = 0;
    uint16_t current_speed_value = 0;
    uint16_t change_in_speed;

	while(1)
	{
		printf("FlowAdjustmentTask: Starting ADC conversion for new flowrate. \n ");

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
	    	//printf("FlowAdjustmentTask: change in speed = %d \n", change_in_speed);

	    	current_speed_value = speed_adc_value; // save previous speed value

			if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE ) // get flowrate semaphore to update with new traffic flowrate
		    {
				g_flowrate = speed_adc_value;
				xSemaphoreGive( xMutexFlow );
				printf("FlowAdjustmentTask: Accessed xMutexFlow, updated flowrate:  %u, (ADC Value: %u). \n", speed_adc_value, adc_value );
		    }
			else{
				printf("FlowTask: xMutexFlow unavailable \n");
			}

	    	//printf("FlowAdjustmentTask: ADC Value: %d = %d /7 \n", adc_value, speed_adc_value);


	    } // end if speed changed

        vTaskDelay(200);

	}
} // end Traffic_Flow_Adjustment_Task
