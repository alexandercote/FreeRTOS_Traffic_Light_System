/*
 * STMRTOSconfig.h
 *
 *  Created on: Feb 25, 2019
 *      Author: brendanb
 */

#ifndef STMRTOSCONFIG_H_
#define STMRTOSCONFIG_H_

/* The period of the example software timer, specified in milliseconds, and
converted to ticks using the portTICK_RATE_MS constant. */
#define mainSOFTWARE_TIMER_PERIOD_MS		( 1000 / portTICK_RATE_MS )

#define speedQUEUE_LENGTH 32
#define displayQUEUE_LENGTH 64
#define prelightactiveQUEUE_LENGTH 1


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

xQueueHandle xQueue_handle_light_colour;				// maybe replaced with interrupts and local variables
xQueueHandle xQueue_handle_speed_creator;
xQueueHandle xQueue_handle_speed_light;
xQueueHandle xQueue_handle_display_traffic;
xQueueHandle xQueue_handle_prelight_active_traffic;

#endif /* STMRTOSCONFIG_H_ */
