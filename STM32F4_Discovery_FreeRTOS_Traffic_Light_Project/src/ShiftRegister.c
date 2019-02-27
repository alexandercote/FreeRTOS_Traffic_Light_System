/*
 * ShiftRegister.C
 *
 *  Created on: Feb 25, 2019
 *      Author: brendanb
 */

#include "ShiftRegister.h"

void ShiftRegisterValuePreLight( uint16_t value )
{
	//printf("Shifting prelight register, value is %u.\n", value );
	//GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);      // ensure shift register clock is low
	if (value == 0)                                             // no car present
		GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);	    // set output low
	else                                                        // car on the road at this location
		GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);        // set output high
	GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);        // set clock high
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);      // set clock low again


}

void ShiftRegisterValuePostLight( uint16_t value )
{
	//printf("Shifting postlight register, value is %u.\n", value );
	//GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_CLK_2_PIN);      // ensure shift register clock is low
	if (value == 0)                                             // no car present
		GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_2_PIN);	    // set output low
	else                                                        // car on the road at this location
		GPIO_SetBits(SHIFT_REG_2_PORT, SHIFT_REG_2_PIN);        // set output high
	GPIO_SetBits(SHIFT_REG_2_PORT, SHIFT_REG_CLK_2_PIN);        // set clock high
	GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_CLK_2_PIN);      // set clock low again
}
