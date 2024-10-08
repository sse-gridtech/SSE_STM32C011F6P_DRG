/*
 * custom_spi.c
 *
 *  Created on: Oct 19, 2023
 *      Author: marce
 */
#include "custom_spi.h"
#include "custom_gpio.h"

enum
{
  TRANSFER_WAIT,
  TRANSFER_COMPLETE,
  TRANSFER_ERROR
};

SPI_HandleTypeDef hspi1;
/* transfer state */
__IO uint32_t wTransferState = TRANSFER_WAIT;

static void MX_SPI1_Init(void);
void SPI1_Error_Handler(void);

void SPI1_select(uint8_t _cs)
{
    switch (_cs)
    {
        case 0: // MEM CS
            break;
        case 1: // RADIO CS
            break;
        case 2: // ACC CS
			break;
    }
}

void SPI1_clear(void)
{
	GPIO_PB3_RADIO_NSS_Set();
	//GPIO_PB2_FLASH_NSS_Set();
}

uint8_t SPI1_IsBusy(void)
{
    switch(wTransferState)
    {
    	case TRANSFER_WAIT:
    		break;
    	case TRANSFER_COMPLETE:
    		return 0;
    	case TRANSFER_ERROR:
    		SPI1_Error_Handler();
    		break;
    }
    return 1;
}

void SPI1_TransmitReceive(void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize)
{
	wTransferState = TRANSFER_WAIT;
	/*##-1- Start the Full Duplex Communication process ########################*/
	/* While the SPI in TransmitReceive process, user can transmit data through
	 "aTxBuffer" buffer & receive data through "aRxBuffer" */
	if (HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t *)pTransmitData, (uint8_t *)pReceiveData, txSize) != HAL_OK)
	{
		/* Transfer error in transmission process */
		SPI1_Error_Handler();
	}
}

void SPI1_Init(void)
{
	MX_SPI1_Init();
}
/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
//void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
	  SPI1_Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of Interrupt TxRx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  wTransferState = TRANSFER_COMPLETE;
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  wTransferState = TRANSFER_ERROR;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void SPI1_Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
