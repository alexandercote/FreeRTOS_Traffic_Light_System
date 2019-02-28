/*
 * TrafficFlow.c
 *
 *  Created on: Feb 25, 2019
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
    uint16_t adc_value = 0;            // Start with adc_value as 0, until it is read
    uint16_t speed_adc_value = 0;
    uint16_t current_speed_value = 0;
    uint16_t change_in_speed;

	while(1)
	{
		printf("FlowAdjustmentTask: Starting ADC conversion for new flowrate. \n ");

		ADC_SoftwareStartConv(ADC1);		            // Start conversion of ADC value
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	// Wait for ADC to finish conversion
		adc_value = ADC_GetConversionValue(ADC1);       // Grab ADC value

		speed_adc_value = adc_value/512;                // Take the adc_value (range between 0 to 4096) and divide by 512 to get 8 discrete steps
		if(speed_adc_value == 8)
		{
			speed_adc_value = 7;                        // As the speed value of 8 is only for a value of adc_value of 4096 exactly, set it to 7. Gives a range of 0-7
		}
        change_in_speed = abs(speed_adc_value - current_speed_value);   // Check if speed changed.

	    if(change_in_speed !=  0) 	// will only do queue stuff if the speed changed.
	    {
	    	current_speed_value = speed_adc_value; // save previous speed value for future task calls.

			if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE ) // get flowrate semaphore to update with new traffic flowrate
		    {
				g_flowrate = speed_adc_value;                               // Update global variable with new discrete speed step
				xSemaphoreGive( xMutexFlow );                               // Updated global flow variable, release mutex.
				printf("FlowAdjustmentTask: Accessed xMutexFlow, updated flowrate:  %u, (ADC Value: %u). \n", speed_adc_value, adc_value );
		    }
			else{
				printf("FlowTask: xMutexFlow unavailable \n");
			}

	    } // end if speed changed

        vTaskDelay(200);
	}
} // end Traffic_Flow_Adjustment_Task
