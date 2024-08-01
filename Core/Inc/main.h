/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32c0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sse_defines.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

typedef struct{
    uint32_t 	 grand;
    uint16_t     cod;
    bool         send;
}packet;

typedef struct{
	uint32_t	latencia_pendencia[8];
	uint32_t	_now;
	uint32_t	read_timer;
    uint32_t	read_timer2;
    uint32_t	t_medicao;
	uint16_t	pendencias_control;
	uint8_t		_baudIdx;
    uint8_t		_medStatus;
    uint8_t		medidor_loop_status;
    uint8_t		medidor_loop_retry;
    uint32_t	medidor_loop_timer;
    uint8_t     medidor_corte_statemachine;
    uint8_t		medidor_corte_statemachine_idx;
	int8_t 		tempCorrection;
    uint16_t	eventos_control;
	uint8_t 	packCount;
}SSE_MDC;
extern SSE_MDC				MDC;

typedef struct{
	uint8_t		id_[6];
	uint32_t 	t_pkg;			// instante em que foi recebido o último pacote
#if (MEDIDOR_TIPO == MEDIDOR_TIPO_1_PIMA)
	uint8_t 	en_ativa[3];		// energia ativa do medidor (leitura do último pacote com esse parâmetro)
	uint8_t		en_capacitiva[3];	// energia capacitiva do medidor (leitura do último pacote com esse parâmetro)
	uint8_t		en_indutiva[3];	// energia indutiva do medidor (leitura do último pacote com esse parâmetro)
	uint8_t		en_ativareversa[3];	//
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR)
	uint8_t 	en_ativa[5];		// energia ativa do medidor (leitura do último pacote com esse parâmetro)
	uint8_t		en_capacitiva[5];	// energia capacitiva do medidor (leitura do último pacote com esse parâmetro)
	uint8_t		en_indutiva[5];	// energia indutiva do medidor (leitura do último pacote com esse parâmetro)
	uint8_t		en_ativareversa[5];	//
	uint8_t		en_ativaMSB;
	float		ke;
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT)
	//uint8_t 	en_ativa[5];		// energia ativa do medidor (leitura do último pacote com esse parâmetro)
	//uint8_t		en_capacitiva[5];	// energia capacitiva do medidor (leitura do último pacote com esse parâmetro)
	//uint8_t		en_indutiva[5];	// energia indutiva do medidor (leitura do último pacote com esse parâmetro)
	//uint8_t		en_ativareversa[5];	//
	//uint8_t		en_ativaMSB;
	float		ke;
    //uint8_t     auth_size;
    uint8_t     auth_type;
    uint8_t     auth_status;
    //char        auth_string[30];
    uint8_t     read_type;
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_4_FAKE_EXT)
    float		ke;
    uint8_t     auth_type;
    uint8_t     auth_status;
    uint8_t     read_type;
#endif
    uint8_t     statemachine;
	uint8_t 	status;			// status corrente desse medidor
	uint8_t		mode;
    uint8_t     lastOcorrencia;
    //uint8_t		version[2];
    //uint8_t		model[2];
    //uint8_t		fatura;
    uint8_t     comm_error;
}SSE_Medidor;
extern SSE_Medidor			medidor;

typedef struct{
	char code[3];
	char desc[24];
	uint8_t value[4];
	char unit[4];
	bool toDisplay;
}SSE_DQ;
extern SSE_DQ			   displayQuantities[199];
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
