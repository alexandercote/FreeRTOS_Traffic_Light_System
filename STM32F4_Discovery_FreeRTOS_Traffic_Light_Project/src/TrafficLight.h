/*
 * TrafficLight.h
 *
 *  Created on: Feb 25, 2019
 */

#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

#include "STMRTOSconfig.h"

//Timer declarations
void vGreenLightTimerCallback( xTimerHandle xTimer );
void vYellowLightTimerCallback( xTimerHandle xTimer );
void vRedLightTimerCallback( xTimerHandle xTimer );

void TrafficLightTask( void *pvParameters );

xTimerHandle xRedLightSoftwareTimer;
xTimerHandle xYellowLightSoftwareTimer;
xTimerHandle xGreenLightSoftwareTimer;

#endif /* TRAFFICLIGHT_H_ */


