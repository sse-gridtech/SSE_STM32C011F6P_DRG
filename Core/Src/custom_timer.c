/*
 * custom_timer.c
 *
 *  Created on: Oct 19, 2023
 *      Author: marce
 */
#include "custom_timer.h"
#include "custom_gpio.h"

TIM_HandleTypeDef htim1;
#if USE_SYSTICK_AS_MS == 0
TIM_HandleTypeDef htim3;
#endif

volatile uint32_t TIM_secs=0;
volatile uint32_t TIM_millis=0;
static uint8_t LED1_control;
static uint8_t LED1_acc;
static uint8_t LED1_acc2;

static void MX_TIM1_Init(void);
#if USE_SYSTICK_AS_MS == 0
static void MX_TIM3_Init(void);
#endif
void TIMER_Error_Handler(void);

uint32_t TIM_getSecs(void)
{
	return TIM_secs;
}
uint32_t TIM_getMillis(void)
{
#if USE_SYSTICK_AS_MS == 0
	return TIM_millis;
#else
	return HAL_GetTick();
#endif
}

void TIM_delay(uint32_t delay)
{
	uint32_t start = TIM_getMillis();
	while((TIM_getMillis() - start) < delay);
}

uint8_t TIM_Fired(uint32_t _current, uint32_t *_old, uint32_t _duration)
{
	uint32_t _last=*_old;

	if(_last>_current)
	{
		*_old=_current;
		return 0;
	}
	if((_current-_last)>_duration)
	{
		*_old=_current;
		return 1;
	}
	return 0;
}

void TIMER_Start(void)
{
	/* USER CODE BEGIN 2 */
	/* Start Channel1 - TIMER2 */
	if (HAL_TIM_Base_Start_IT(&htim1) != HAL_OK)
	{
		/* Starting Error */
		TIMER_Error_Handler();
	}
#if USE_SYSTICK_AS_MS == 0
	/* Start Channel2 - TIMER3 */
	if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
	{
		/* Starting Error */
		TIMER_Error_Handler();
	}
#endif
	/* USER CODE END 2 */
}

void TIMER_Init(void)
{
	MX_TIM1_Init();
#if USE_SYSTICK_AS_MS == 0
	MX_TIM3_Init();
#endif
}

void LED1_setStatus(uint8_t _status)
{
#if 0
	LED1_control=_status;
#else
	if(_status)
		GPIO_PA0_LED0_Set();
	else
		GPIO_PA0_LED0_Clear();
	//GPIO_PA0_LED0_Toggle();
#endif
}
void LED1_Control(void)
{
	LED1_acc++;

	if(LED1_control)
	{
		if(LED1_acc>9)
		{
            LED1_acc=0;
            switch(LED1_acc2++)
            {
                case 0:
                	GPIO_PA0_LED0_Clear();
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                	GPIO_PA0_LED0_Toggle();
                    break;
                case 5:
                case 6:
                case 7:
                case 8:
                    break;
                case 9:
                    LED1_acc2=0;
                    break;
                default:
                    LED1_acc2=0;
                    break;
            }
		}
	}
	else
	{
		if(LED1_acc>99)
		{
			GPIO_PA0_LED0_Toggle();
			LED1_acc=0;
            LED1_acc2=0;
		}
	}
}
/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = PRESCALER_VALUE;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = PERIOD_VALUE_1s;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
	  TIMER_Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
	  TIMER_Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
	  TIMER_Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

#if USE_SYSTICK_AS_MS == 0
/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = PRESCALER_VALUE2; //6399;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  //htim3.Init.Period = PERIOD_VALUE2_500ms; //99;
  htim3.Init.Period = PERIOD_VALUE2_1ms; //99;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
	  TIMER_Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
	  TIMER_Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
	  TIMER_Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}
#endif

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void TIMER_Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

