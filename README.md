# RTOS_Traffic_Light_System
UVic's ECE455 Lab Project - designing a traffic light controller for a 1-way street using FreeRTOS on an STM32F4 Discovery board.

![image](go2.gif)
      

## Project Description and Specifications

The traffic light system:
![image](https://user-images.githubusercontent.com/978364/52812391-3072fa00-304c-11e9-87bd-1eafaa998393.png)

The three sections are as follows:
1. Traffic flow adjustement potentiometer: Low value of resistance corresponds to light traffic, and high value of resistance corresponds to heavy traffic.
2. LEDs represent the current position of the car (On is a car in that location). To save on GPIO outputs from the STM32F4 discovery board, shift registers will be used.
3. Traffic lights are represented by three LEDS.

Schematic Design:
![image](schematic.PNG)
      
