/*
 * custom_uart.h
 *
 *  Created on: Oct 18, 2023
 *      Author: marce
 */
#include "stm32c0xx_hal.h"

#ifndef CUSTOM_UART_H_
#define CUSTOM_UART_H_

/* USER CODE BEGIN Private defines */

void SERIAL0_Init(void);
void SERIAL0_Receive(void);
volatile void SERIAL0_send (uint8_t* _buffer, uint16_t _size);
volatile uint16_t SERIAL0_read(uint8_t* _buffer, uint16_t _size);
volatile uint16_t SERIAL0_available(void);
void SERIAL0_BaudRate(uint32_t _baud);
void SERIAL0_clear(void);
uint8_t SERIAL0_receiveUntil(uint16_t _size, uint16_t _timeout);

void SERIAL1_Init(void);
void SERIAL1_Receive(void);
volatile void SERIAL1_send (uint8_t* _buffer, uint16_t _size);
volatile uint16_t SERIAL1_read(uint8_t* _buffer, uint16_t _size);
volatile uint16_t SERIAL1_available(void);
void SERIAL1_BaudRate(uint32_t _baud);
void SERIAL1_clear(void);
uint8_t SERIAL1_receiveUntil(uint16_t _size, uint16_t _timeout);

#endif /* CUSTOM_SPI_H_ */
