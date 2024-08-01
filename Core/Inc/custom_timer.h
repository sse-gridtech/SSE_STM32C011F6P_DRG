/*
 * custom_timer.h
 *
 *  Created on: Oct 18, 2023
 *      Author: marce
 */
#include "stm32c0xx_hal.h"

#ifndef CUSTOM_TIMER_H_
#define CUSTOM_TIMER_H_

/* USER CODE BEGIN Private defines */
#define USE_SYSTICK_AS_MS	0
/* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
#define PRESCALER_VALUE (uint32_t)(((SystemCoreClock) / (10000)) - 1)
  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = (SystemCoreClock/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
#define PERIOD_VALUE_1s (10000 - 1);

#define PRESCALER_VALUE2 (uint32_t)(((SystemCoreClock) / (100000)) - 1)
#define PERIOD_VALUE2_1ms (100 - 1);
#define PERIOD_VALUE2_500ms (50000 - 1);

uint32_t TIM_getSecs(void);
uint32_t TIM_getMillis(void);
void TIM_delay(uint32_t delay);
uint8_t TIM_Fired(uint32_t _current, uint32_t *_old, uint32_t _duration);

void TIMER_Start(void);
void TIMER_Init(void);

void LED1_setStatus(uint8_t _status);
void LED1_Control(void);

#endif /* CUSTOM_TIMER_H_ */
