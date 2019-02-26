/*
 * ShiftRegister.C
 *
 *  Created on: Feb 25, 2019
 *      Author: brendanb
 */

#include "ShiftRegister.h"

void ShiftRegisterValuePreLight( bool value )
{
	printf("Shifting prelight register.\n");
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);      // ensure shift register clock is low
	if (value == false)                                             // no car present
		GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);	    // set output low
	else                                                        // car on the road at this location
		GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);        // set output high
	GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);        // set clock high
	//vTaskDelay(10);
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);      // set clock low again


}

void ShiftRegisterValuePostLight( bool value )
{
	GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_CLK_2_PIN);      // ensure shift register clock is low
	if (value == false)                                             // no car present
		GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_2_PIN);	    // set output low
	else                                                        // car on the road at this location
		GPIO_SetBits(SHIFT_REG_2_PORT, SHIFT_REG_2_PIN);        // set output high
	GPIO_SetBits(SHIFT_REG_2_PORT, SHIFT_REG_CLK_2_PIN);        // set clock high
	//vTaskDelay(10);
	GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_CLK_2_PIN);      // set clock low again
}
