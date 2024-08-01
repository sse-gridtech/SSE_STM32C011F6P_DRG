/*
 * custom_uart.c
 *
 *  Created on: Oct 19, 2023
 *      Author: marce
 */
#include "custom_uart.h"
//#include "custom_gpio.h"
#include "custom_timer.h"
#include "string.h"

#define SERIAL0_MINIMAL     0

#define SERIAL0_BAUD_RATE	9600
#define SERIAL0_BUFFER_LEN  300//300
UART_HandleTypeDef huart1;
uint8_t  SERIAL0_rx_buffer[SERIAL0_BUFFER_LEN];
static volatile uint16_t SERIAL0_rx_buffer_ini = 0;
uint16_t SERIAL0_rx_buffer_end=0;
volatile uint8_t UART0_writeStatus = 0;

static void MX_USART1_UART_Init(void);
void UART_Error_Handler(void);

void SERIAL0_Init(void)
{
	MX_USART1_UART_Init();
}

void SERIAL0_Receive(void)
{
	HAL_UART_Receive_IT(&huart1, &SERIAL0_rx_buffer[SERIAL0_rx_buffer_end], 1);
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = SERIAL0_BAUD_RATE;//SERIAL0_BAUD_RATE;//9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV4;//UART_PRESCALER_DIV4;//UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  //huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_TXINVERT_INIT|UART_ADVFEATURE_RXINVERT_INIT
  //                            |UART_ADVFEATURE_DATAINVERT_INIT;
  //huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXINVERT_INIT;
  //huart1.AdvancedInit.RxPinLevelInvert = UART_ADVFEATURE_RXINV_ENABLE;
  //huart1.AdvancedInit.MSBFirst = UART_ADVFEATURE_MSBFIRST_ENABLE;
  //huart1.AdvancedInit.DataInvert = UART_ADVFEATURE_DATAINV_ENABLE;
  //UART_ADVFEATURE_MSBFIRST_DISABLE
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
	  UART_Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
	  UART_Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
	  UART_Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
	  UART_Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
// UART0
void SERIAL0_BaudRate(uint32_t _baud)
{
	huart1.Init.BaudRate = _baud;
}

volatile uint16_t SERIAL0_available(void)
{
	if(SERIAL0_rx_buffer_end==SERIAL0_rx_buffer_ini)
	{
		return 0;
	}
	else if(SERIAL0_rx_buffer_end>SERIAL0_rx_buffer_ini)
	{
		return (SERIAL0_rx_buffer_end - SERIAL0_rx_buffer_ini);
	}
	else
	{
		return (SERIAL0_BUFFER_LEN-SERIAL0_rx_buffer_ini+SERIAL0_rx_buffer_end);
	}
}
volatile uint16_t SERIAL0_read(uint8_t* _buffer, uint16_t _size)
{
    uint16_t leitura = 0;
    uint16_t restante = 0;
    if((SERIAL0_rx_buffer_ini + _size) >= SERIAL0_BUFFER_LEN)
    {

        restante = (SERIAL0_rx_buffer_ini + _size - SERIAL0_BUFFER_LEN);
        leitura = (_size - restante);

        memcpy(_buffer,&SERIAL0_rx_buffer[SERIAL0_rx_buffer_ini],leitura);

        SERIAL0_rx_buffer_ini%=SERIAL0_rx_buffer_ini;

        memcpy(&_buffer[leitura],&SERIAL0_rx_buffer[SERIAL0_rx_buffer_ini],restante);

        SERIAL0_rx_buffer_ini+=restante;
    }
    else
    {
        memcpy(_buffer,&SERIAL0_rx_buffer[SERIAL0_rx_buffer_ini],_size);
        SERIAL0_rx_buffer_ini+=_size;
    }
    return _size;
}

volatile void SERIAL0_send (uint8_t* _buffer, uint16_t _size)
{
	if(_size>0)
	{
		UART0_writeStatus=0;
		//R_UART2_Send(_buffer,_size);
        //SERCOM0_USART_Write(_buffer,_size);
		HAL_UART_Transmit_IT(&huart1, _buffer, _size);
		while(UART0_writeStatus==0);
	}
}

#if SERIAL0_MINIMAL == 0
void SERIAL0_clear(void)
{
    SERIAL0_rx_buffer_ini=SERIAL0_rx_buffer_end;
}
uint8_t SERIAL0_receiveUntil(uint16_t _size, uint16_t _timeout)
{
    uint32_t serial_time_start = TIM_getMillis();

    while((SERIAL0_available()<_size)&&((TIM_getMillis()-serial_time_start)<_timeout))
    {
        //WDT_Clear();
    	__NOP();
    }
    if(SERIAL0_available()>=_size)
	{
		return 1;
	}
    return 0;
}
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  //UartReady = SET;
	UART0_writeStatus=1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  //UartReady = SET;
	SERIAL0_rx_buffer_end++;
	if(SERIAL0_rx_buffer_end>=SERIAL0_BUFFER_LEN)
	{
		//SERIAL0_rx_buffer_end-=SERIAL0_BUFFER_LEN;
		SERIAL0_rx_buffer_end=0;
	}
	//SERCOM0_USART_Read(&SERIAL0_rx_buffer[SERIAL0_rx_buffer_end], 1);
	HAL_UART_Receive_IT(&huart1, &SERIAL0_rx_buffer[SERIAL0_rx_buffer_end], 1);
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
	HAL_UART_Receive_IT(&huart1, &SERIAL0_rx_buffer[SERIAL0_rx_buffer_end], 1);
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void UART_Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
