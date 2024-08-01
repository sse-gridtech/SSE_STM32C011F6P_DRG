/*
 * custom_gpio.h
 *
 *  Created on: Oct 18, 2023
 *      Author: marce
 */
#include "stm32c0xx_hal.h"

#ifndef CUSTOM_SPI_H_
#define CUSTOM_SPI_H_

/* USER CODE BEGIN Private defines */

void SPI1_select(uint8_t _cs);
void SPI1_clear(void);
uint8_t SPI1_IsBusy(void);
void SPI1_TransmitReceive(void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize);
void SPI1_Init(void);

#endif /* CUSTOM_SPI_H_ */
