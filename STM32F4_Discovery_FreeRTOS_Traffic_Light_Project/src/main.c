/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4_discovery.h"

/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

/*-----------------------------------------------------------*/

/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );



// Pinout Defines

#define ADC_PORT                 GPIOC
#define ADC_PIN                  GPIO_Pin_1

#define SHIFT_REG_1_PORT         GPIOE
#define SHIFT_REG_1_PIN          GPIO_Pin_5
#define SHIFT_REG_CLK_1_PIN      GPIO_Pin_3

#define SHIFT_REG_2_PORT         GPIOC
#define SHIFT_REG_2_PIN          GPIO_Pin_15
#define SHIFT_REG_CLK_2_PIN      GPIO_Pin_14
#define SHIFT_REG_RST_PIN        GPIO_Pin_13

#define TRAFFIC_LIGHT_PORT       GPIOD
#define TRAFFIC_LIGHT_RED_PIN    GPIO_Pin_6
#define TRAFFIC_LIGHT_YELLOW_PIN GPIO_Pin_4
#define TRAFFIC_LIGHT_GREEN_PIN  GPIO_Pin_2


// Traffic light task priorities
#define TRAFFIC_FLOW_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1 )
#define TRAFFIC_CREATE_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
#define TRAFFIC_LIGHT_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2 )
#define TRAFFIC_DISPLAY_TASK_PRIORITY	( tskIDLE_PRIORITY  )


// Initialization declaration
void HardwareInit(void);

// Test task declaration
void ADCTestTask( void *pvParameters );
void ShiftTestTask( void *pvParameters );

// Helper function declarations
void ShiftRegisterValuePreLight( bool value );
void ShiftRegisterValuePostLight( bool value );

// Traffic Light task declarations
void TrafficFlowAdjustmentTask( void *pvParameters );
void TrafficCreatorTask( void *pvParameters );
void TrafficLightTask( void *pvParameters );
void TrafficDisplayTask( void *pvParameters );



/*-----------------------------------------------------------*/

int main(void)
{

	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */
	prvSetupHardware();

	HardwareInit();

	//xTaskCreate( ADCTestTask, "ADCTestTask1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( ShiftTestTask, "ShiftTestTask1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);



	// Traffic light tasks
	/*
	xTaskCreate( Traffic_Flow_Adjustment_Task, "FlowAdjust",configMINIMAL_STACK_SIZE ,NULL ,TRAFFIC_FLOW_TASK_PRIORITY,   NULL);
	xTaskCreate( Traffic_Creator_Task        , "Creator"   ,configMINIMAL_STACK_SIZE ,NULL ,TRAFFIC_CREATE_TASK_PRIORITY, NULL);
	xTaskCreate( Traffic_Light_Task          , "Light"	   ,configMINIMAL_STACK_SIZE ,NULL ,TRAFFIC_LIGHT_TASK_PRIORITY,  NULL);
	xTaskCreate( Traffic_Display_Task        , "Display"   ,configMINIMAL_STACK_SIZE ,NULL ,TRAFFIC_DISPLAY_TASK_PRIORITY,NULL);
	*/


	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}
/*-----------------------------------------------------------*/

// Traffic light questions

/*  Traffic flow adjustment task: The traffic flow that enters the intersection is set
	by a potentiometer. This task reads the value of the potentiometer at proper
	intervals. The low resistance of the potentiometer corresponds to light traffic and
	a high resistance corresponds to heavy traffic. The reading by this task is sent
	and used by other tasks.
 */
void TrafficFlowAdjustmentTask ( void *pvParameters )
{

} // end Traffic_Flow_Adjustment_Task

/*  Traffic creator task: This task generates random traffic with a rate that is
	based on the potentiometer value reading. This value is received from the traffic
	flow adjustment task. The created traffic is sent to the task that displays the flow
	of the cars on the road.
 */

void TrafficCreatorTask ( void *pvParameters )
{

} // end Traffic_Creator_Task

/*  Traffic light task: This task controls the timing of the traffic light. This timing is
	affected by the load of the traffic which is received from the traffic flow
	adjustment task.
*/

void TrafficLightTask ( void *pvParameters )
{
	// add stuff here
} // end Traffic_Light_Task

/*
    Traffic display task: This task controls the LEDs that represent the cars at the
	intersection. It receives the traffic from traffic creator task and displays them on
	the LEDs. It refreshes the LEDs at a certain interval to emulate the flow of the
	traffic.
 */
void TrafficDisplayTask ( void *pvParameters )
{

} // end Traffic_Display_Task



void ShiftRegisterValuePreLight( bool value )
{
	printf("Shifting prelight register.\n");
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);      // ensure shift register clock is low
	if (value == false)                                             // no car present
		GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);	    // set output low
	else                                                        // car on the road at this location
		GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);        // set output high
	GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);        // set clock high
	//vTaskDelay(50);
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
	//vTaskDelay(50);
	GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_CLK_2_PIN);      // set clock low again
}

void ShiftTestTask ( void* pvParameters )
{
	while(1)
	{
		printf("ShiftTestTop!\n");
		ShiftRegisterValuePreLight( true );
		ShiftRegisterValuePreLight( true );
		ShiftRegisterValuePreLight( true );
		ShiftRegisterValuePreLight( true );
		ShiftRegisterValuePreLight( true );
		ShiftRegisterValuePreLight( true );
		ShiftRegisterValuePreLight( true );
		ShiftRegisterValuePreLight( false );
		ShiftRegisterValuePreLight( false );
	}
} // end ShiftTestTask



void ADCTestTask( void* pvParameters)
{
	uint16_t adc_value;
	while(1)
	{
		ADC_SoftwareStartConv(ADC1);
		// wait for ADC to finish conversion
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		// grab ADC value
		adc_value = ADC_GetConversionValue(ADC1);
		printf("ADC Value: %d\n", adc_value);
		vTaskDelay(500);
	}
} // end ADCTestTask

void HardwareInit()
{ // Initializes GPIO and ADC

	// 1. Init GPIO
	GPIO_InitTypeDef      SHIFT_1_GPIO_InitStructure;
	GPIO_InitTypeDef      SHIFT_2_GPIO_InitStructure;
	GPIO_InitTypeDef      TRAFFIC_GPIO_InitStructure;

	/* Enable GPIO clock for GPIO */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    SHIFT_1_GPIO_InitStructure.GPIO_Pin = SHIFT_REG_1_PIN | SHIFT_REG_CLK_1_PIN ;
    SHIFT_1_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    SHIFT_1_GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
    SHIFT_1_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(SHIFT_REG_1_PORT, &SHIFT_1_GPIO_InitStructure);

    SHIFT_2_GPIO_InitStructure.GPIO_Pin = SHIFT_REG_2_PIN | SHIFT_REG_CLK_2_PIN | SHIFT_REG_RST_PIN;
    SHIFT_2_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    SHIFT_2_GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
    SHIFT_2_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(SHIFT_REG_2_PORT, &SHIFT_2_GPIO_InitStructure);

    TRAFFIC_GPIO_InitStructure.GPIO_Pin = TRAFFIC_LIGHT_RED_PIN | TRAFFIC_LIGHT_YELLOW_PIN | TRAFFIC_LIGHT_GREEN_PIN;
    TRAFFIC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    TRAFFIC_GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
    TRAFFIC_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TRAFFIC_LIGHT_PORT, &TRAFFIC_GPIO_InitStructure);



	// 2. Init ADC
	ADC_InitTypeDef       ADC_InitStructure;
	GPIO_InitTypeDef      ADC_GPIO_InitStructure;

	/* Enable GPIO and ADC clocks for ADC */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* Configure ADC1 Channel11 pin as analog input ******************************/
    ADC_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    ADC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    ADC_GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOC, &ADC_GPIO_InitStructure);

    /* ADC1 Init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11 , 1, ADC_SampleTime_84Cycles);
} // end HardwareInit





void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

