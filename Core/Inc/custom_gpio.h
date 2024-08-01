/*
 * custom_gpio.h
 *
 *  Created on: Oct 18, 2023
 *      Author: marce
 */
#include "stm32c0xx_hal.h"

#ifndef CUSTOM_GPIO_H_
#define CUSTOM_GPIO_H_

/* USER CODE BEGIN Private defines */
#define LED0_PIN                         GPIO_PIN_0
#define LED0_GPIO_PORT                   GPIOA
#define LED1_PIN                         GPIO_PIN_1
#define LED1_GPIO_PORT                   GPIOA
#define RADIO_NSS_PIN                    GPIO_PIN_8
#define RADIO_NSS_GPIO_PORT              GPIOA
#define RADIO_RST_PIN                    GPIO_PIN_1
#define RADIO_RST_GPIO_PORT              GPIOA

// GPIO_Functions_Defines
#define GPIO_PA0_LED0_Set()			(HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_PIN, GPIO_PIN_SET))
#define GPIO_PA0_LED0_Clear()		(HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_PIN, GPIO_PIN_RESET))
#define GPIO_PA0_LED0_Toggle()		(HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_PIN))

#define GPIO_PA1_LED1_Set()			(HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET))
#define GPIO_PA1_LED1_Clear()		(HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET))
#define GPIO_PA1_LED1_Toggle()		(HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN))

#define GPIO_PB3_RADIO_NSS_Set()		(HAL_GPIO_WritePin(RADIO_NSS_GPIO_PORT, RADIO_NSS_PIN, GPIO_PIN_SET))
#define GPIO_PB3_RADIO_NSS_Clear()		(HAL_GPIO_WritePin(RADIO_NSS_GPIO_PORT, RADIO_NSS_PIN, GPIO_PIN_RESET))
#define GPIO_PB4_RADIO_RST_Set()		(HAL_GPIO_WritePin(RADIO_RST_GPIO_PORT, RADIO_RST_PIN, GPIO_PIN_SET))
#define GPIO_PB4_RADIO_RST_Clear()		(HAL_GPIO_WritePin(RADIO_RST_GPIO_PORT, RADIO_RST_PIN, GPIO_PIN_RESET))

#endif /* CUSTOM_GPIO_H_ */
