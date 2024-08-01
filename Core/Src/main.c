/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sse_util.h"
#include "sse_protocol.h"
#include "sse_meters.h"
#include "RFM69_Base.h"
#include "st7789.h"
#include "custom_spi.h"
#include "custom_timer.h"
#include "custom_uart.h"
#include "sse_flash_memory.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

//SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

//TIM_HandleTypeDef htim1;
//TIM_HandleTypeDef htim3;

//UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
#if SPI_CUSTOM == 1
SPI_HandleTypeDef hspi1;
#endif
#if TIMER_CUSTOM == 1
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
#endif
#if UART_CUSTOM == 1
UART_HandleTypeDef huart1;
#define SERIAL0_BAUD_RATE	9600
#define SERIAL0_BUFFER_LEN  300//300
uint8_t  SERIAL0_rx_buffer[SERIAL0_BUFFER_LEN];
static volatile uint16_t SERIAL0_rx_buffer_ini = 0;
uint16_t SERIAL0_rx_buffer_end=0;
volatile bool UART0_writeStatus = false;
uint8_t aTxBuffer[] = "TESTE-123456789";
uint32_t timerSend=0;
#endif

volatile uint8_t autoBaudFlag;
uint8_t cont = 0;
uint8_t tot = 0;
uint8_t idx = 0;

packet              pacote[103];
SSE_MDC				MDC={0};
SSE_Medidor			medidor={0};
SSE_DQ              displayQuantities[199] =
{
		{"000", "0000000000", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"001", "Data (dd/mm/aa)        ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"002", "Hora (hh:mm:ss)        ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"003", "C1:Totalizador Geral   ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"004", "C1:Totalizador Ponta   ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"005", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"006", "C1:Totalizador R       ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"007", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"008", "C1:Totalizador f. P    ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"009", "C1:Totalizador D       ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"010", "C1:Demanda Maxima P    ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"011", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"012", "C1:Demanda Maxima R    ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"013", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"014", "C1:Demanda Maxima F. P ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"015", "C1:Demanda Maxima D    ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"016", "C1:Demanda Ult. I      ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"017", "C1:Demanda Acum. P     ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"018", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"019", "C1:Demanda Acum. R     ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"020", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"021", "C1:Demanda Acum. F. P  ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"022", "C1:Demanda Acum. D     ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"023", "Num. Ope. Rep. Dem.    ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"024", "C2:Totalizador Geral   ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"025", "C2:Totalizador Ponta   ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"026", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"027", "C2:Totalizador Res     ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"028", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"029", "C2:Totalizador F Ponta ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"030", "C2:Totalizador D       ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"031", "C3:Totalizador Geral   ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"032", "Estado da bateria      ", {0xff, 0xff, 0xff, 0xff}, "", false}, // Unidade = V ?
		{"033", "Num. serie equipamento ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"034", "C2:Demanda Max P       ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"035", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"036", "C2:Demanda Max R       ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"037", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"038", "C2:Demanda Max F P     ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"039", "C2:Demanda Max D       ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"040", "C2:Demanda Ult Int Int ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"041", "C2:Demanda Acum P      ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"042", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"043", "C2:Demanda Acum R      ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"044", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"045", "C2:Demanda Acum F P    ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"046", "C2:Demanda Acum D      ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"047", "Totalizador C1 Atual   ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"048", "Totalizador C2 Atual   ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"049", "Totalizador C3 Atual   ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"050", "C1:Totalizador Comp    ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"051", "C1:Demanda Maxima C    ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"052", "C1:Demanda Maxima G    ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"053", "C1:Demanda Acum C      ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"054", "C1:Demanda Acum G      ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"055", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"056", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"057", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"058", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"059", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"060", "C2:Totalizador Comp    ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"061", "C2:Demanda Maxima C    ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"062", "C2:Demanda Maxima G    ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"063", "C2:Demanda Acum C      ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"064", "C2:Demanda Acum G      ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"065", "UFER Total (/100)      ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"066", "UFER Ponta             ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"067", "UFER Res (/100)        ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"068", "UFER F P (/100)        ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"069", "DMCR Ponta             ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"070", "DMCR Reservado         ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"071", "DMCR Fora Ponta        ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"072", "DMCR Ult Int Reat      ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"073", "DMCR Acum Ponta        ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"074", "DMCR Acum Res          ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"075", "DMCR Acum F Ponta      ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"076", "UFER Comp (/100)       ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"077", "DMCR Comp              ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"078", "DMCR Max Geral         ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"079", "DMCR Acum Comp         ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"080", "DMCR Acum Geral        ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"081", "C2:Tot. Reverso G      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"082", "C2:Tot. Reverso P      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"083", "C2:Tot. Reverso R      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"084", "C2:Tot. Reverso F P    ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"085", "C3:Totalizador P       ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"086", "C3:Totalizador R       ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"087", "C3:Totalizador F P     ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"088", "Teste do mostrador     ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"089", "C3:Tot. Reverso G      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"090", "C3:Tot. Reverso P      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"091", "C3:Tot. Reverso R      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"092", "C3:Tot. Reverso F P    ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"093", "FP.ult. dem/reat       ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"094", "Ke                     ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"095", "Kh                     ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"096", "TP                     ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"097", "TC                     ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"098", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"099", "Codigo de consistencia ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"--", "Config do equipamento   ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"101", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"102", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"103", "C4:Totalizador Geral   ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"104", "C4:Totalizador P       ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"105", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"106", "C4:Totalizador R       ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"107", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"108", "C4:Totalizador f. P    ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"109", "C4:Totalizador D       ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"110", "C4:Demanda Maxima P    ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"111", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"112", "C4:Demanda Maxima R    ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"113", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"114", "C4:Demanda Maxima F. P ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"115", "C4:Demanda Maxima D    ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"116", "C4:Demanda Ult. I      ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"117", "C4:Demanda Acum. P     ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"118", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"119", "C4:Demanda Acum. R     ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"120", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"121", "C4:Demanda Acum. F. P  ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"122", "C4:Demanda Acum. D     ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"123", "Num. Ope. Rep. Dem.    ", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"124", "C5:Totalizador Geral   ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"125", "C5:Totalizador Ponta   ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"126", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"127", "C5:Totalizador R       ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"128", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"129", "C5:Totalizador F P     ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"130", "C5:Totalizador D       ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"131", "C6:Totalizador Geral   ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"132", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"133", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"134", "C5:Demanda Max Ponta   ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"135", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"136", "C5:Demanda Max R       ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"137", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"138", "C5:Demanda Max F P     ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"139", "C5:Demanda Max D       ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"140", "C5:Demanda Ult Int Int ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"141", "C5:Demanda Acum Ponta  ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"142", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"143", "C5:Demanda Acum R      ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"144", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"145", "C5:Demanda Acum F P    ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"146", "C5:Demanda Acum D      ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"147", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"148", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"149", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"150", "C4:Totalizador Comp    ", {0xff, 0xff, 0xff, 0xff}, "Wh", false},
		{"151", "C4:Demanda Maxima C    ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"152", "C4:Demanda Maxima G    ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"153", "C4:Demanda Acum. C     ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"154", "C4:Demanda Acum. G     ", {0xff, 0xff, 0xff, 0xff}, "W", false},
		{"155", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"156", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"157", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"158", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"159", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"160", "C5:Totalizador Comp    ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"161", "C5:Demanda Max Comp    ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"162", "C5:Demanda Max G       ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"163", "C5:Demanda Acum Comp   ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"164", "C5:Demanda Acum G      ", {0xff, 0xff, 0xff, 0xff}, "Var", false},
		{"165", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"166", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"167", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"168", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"169", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"170", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"171", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"172", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"173", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"174", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"175", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"176", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"177", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"178", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"179", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"180", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"181", "C5:Tot. Reverso G      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"182", "C5:Tot. Reverso P      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"183", "C5:Tot. Reverso R      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"184", "C5:Tot. Reverso F P    ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"185", "C6:Totalizador G       ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"186", "C6:Totalizador R       ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"187", "C6:Totalizador F P     ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"188", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"189", "C6:Tot. Reverso G      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"190", "C6:Tot. Reverso P      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"191", "C6:Tot. Reverso R      ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"192", "C6:Tot. Reverso F P    ", {0xff, 0xff, 0xff, 0xff}, "Varh", false},
		{"193", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"194", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"195", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"196", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"197", "", {0xff, 0xff, 0xff, 0xff}, "", false},
		{"198", "", {0xff, 0xff, 0xff, 0xff}, "", false}
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
#if 0
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
#endif
/* USER CODE BEGIN PFP */
//static void MX_SPI1_Init(void);
//static void MX_TIM2_Init(void);
//static void MX_TIM3_Init(void);
//static void MX_USART1_UART_Init(void);
//volatile void SERIAL0_send (uint8_t* _buffer, uint16_t _size);
//volatile uint16_t SERIAL0_read(uint8_t* _buffer, uint16_t _size);
//volatile uint16_t SERIAL0_available(void);
//void SERIAL0_BaudRate(uint32_t _baud);
void MDC_init(void);
void MDC_main(void);
void LED_Initialize(void);
void FU_InitFlash(void);
void MDC550_Meter_init();
void MDC550_Meter_loop();
void MDC_readNBR(void);
void MDC_readMedidor(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  //MX_TIM1_Init();
  //MX_TIM3_Init();
  //MX_SPI1_Init();
  //MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  SPI1_Init();
  TIMER_Init();
  TIMER_Start();
  SERIAL0_Init();
  MDC_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  MDC_main();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

#if 0
/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
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
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  htim1.Init.Prescaler = 4799;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 99;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

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
  htim3.Init.Prescaler = 4799;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
#endif

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB7 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void MDC_initRadio(void)
{
    while(RFM69_initialize(RF69_915MHZ, flash1.MDC_RADIOID, flash1.MDC_RADIONET, true, false, 31) == false)
	{
		uint8_t id0=0;
		id0++;
	}
	RFM69_receiveBegin();
}

void FU_InitFlash(void)
{
    //memory_init();
    //FU_readFUControlFlash();
}

/*#define LED_TIME2   500
void LED_Initialize(void)
{
    // D2 4 1 3 5
    LED1_Set();
    LED2_Set();
    HAL_Delay(LED_TIME2);
    LED1_Clear();
    LED2_Clear();
    HAL_Delay(LED_TIME2);
    LED1_Set();
    LED2_Set();
    HAL_Delay(LED_TIME2);
    LED1_Clear();
    LED2_Clear();
    HAL_Delay(LED_TIME2);
}*/

void MDC550_Meter_init()
{
	SERIAL0_Receive();
#if (MEDIDOR_TIPO == MEDIDOR_TIPO_1_PIMA)
	//PIMA
	SERIAL0_BaudRate(2400);
    //SERIAL0_BaudRate(1200);
	MDC.read_timer=TIM_getSecs();
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR)
    uint8_t my_read_id[4];
    //GPIO_PA14_Set();
	SERIAL0_BaudRate(9600);
    my_read_id[0]=0x00;
    my_read_id[1]=0x00;
    my_read_id[2]=0x01;
    my_read_id[3]=0x00;
	//NBR14522_Init(&REM_SSEID[3],&REM_SSEID[2]);
    NBR14522_Init(&my_read_id[0],&my_read_id[0]);

	//_broadcastRTC=TIM_getSecs();
	//__BROADCASTTIME__=6;

	MDC.read_timer=TIM_getSecs();

	if(medidor.ke==0)
	{
		//NBR14522_read(0x21,0,NULL,0);
        NBR14522_read(0x80,0,NULL,0,0);
	}
	MDC_readNBR();
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT)
    //memset(medidor.en_ativa,0xFF,5);
	//memset(medidor.en_capacitiva,0xFF,5);
	//memset(medidor.en_indutiva,0xFF,5);
	//memset(medidor.en_ativareversa,0xFF,5);
    //medidor.status=MEDIDOR_DESCONECTADO;
    medidor.t_pkg=0;
    medidor.status=MEDIDOR_NULO;
	memset(ABSOLUTOID,0x00,4);
	NBR14522_Init(&flash1.MDC_SSEID[3],&ABSOLUTOID[0]);
    medidor.statemachine=0;

    GPIO_PA14_Set();
	SERIAL0_BaudRate(9600);

    NBR14522_readEXT(0xAB, 5,0x02,0x00,NULL,0,0); // comando para identificar ID dos medidores ABS abaixo
    //NBR14522_read(0x21, 5,NULL,0,1); // comando para verificar tipo de comunicacao
    //NBR14522_read(0x13, 5,NULL,0,1); // comando para verificar AUTH
    //NBR14522_read(0x80, 5,NULL,0,1); // comando para verificar ke
    MDC_readNBR();
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_4_FAKE_EXT)
    uint8_t _a=0;
    medidor.t_pkg=0;
    //medidor.status=MEDIDOR_NULO;
    medidor.status=MEDIDOR_ATIVO;
    LED1_setStatus(0);
	memset(ABSOLUTOID,0x00,4);
	NBR14522_Init(&flash1.MDC_SSEID[3],&ABSOLUTOID[0]);
    medidor.statemachine=0;

    GPIO_PA14_Set();
	SERIAL0_BaudRate(9600);

    // AB02
    memcpy(&ABSOLUTOID[0],&flash1.MDC_SSEID[2],4);
    //memcpy(&ABS_medidoresMask[0],&nbr_received.raw[9],5);
    memset(&ABS_medidoresMask[0],0x00,5);
    ABS_medidoresMask[3]=0x1F;
    ABS_medidoresMask[4]=0xFF;
    ABSOLUTO_getMedidoresAtivos();
    //ABSOLUTO_getUCs_AB02();
    for(_a=0;_a<12;_a++)
	{
        if(medidoresABS[_a].medIDXBase!=0)
        {
            medidoresABS[_a].uc[4] = IntToHexU8(medidoresABS[_a].medIDXBase);
            // AB01
            medidoresABS[_a].id[3] = IntToHexU8(medidoresABS[_a].medIDXBase);
            SET_PENDENCIA(PENDABS[_a].push,PEND_ABS_0_UC);
            medidoresABS[_a].contator=1;
        }
	}
    // AB01
    // AB07
        //estado contator
    //medidor.statemachine=1;
    SET_PENDENCIA(PEND.push,PEND_VTC_1_SET_READABS);
#endif
}

void MDC550_Meter_loop()
{
#if (MEDIDOR_TIPO == MEDIDOR_TIPO_1_PIMA)
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR)
	MDC_readMedidor();
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT)
    switch(MDC.medidor_loop_status)
    {
        case 0: // normal
            MDC_readMedidor();
            break;
        case 1: // corte
            MDC_Medidor_CorteAlarme();
            break;
        case 2: // religa
            MDC_Medidor_ReligaAlarme();
            break;
        default:
            MDC_readMedidor();
            break;
    }
    MDC_readNBR();
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_4_FAKE_EXT)
    MDC_readMedidor();
#endif
}

#if (MEDIDOR_TIPO == MEDIDOR_TIPO_1_PIMA)
void MDC_autoBaud(void)
{
	if(autoBaudFlag!=1)
	{
		if(TIM_getSecs()-MDC._now>5)
		{
			switch (MDC._baudIdx)
			{
				case 0:
					MDC._baudIdx++;
					SERIAL0_BaudRate(1200);
					break;
				case 1:
					MDC._baudIdx++;
					SERIAL0_BaudRate(300);
					break;
				case 2:
					MDC._baudIdx++;
					SERIAL0_BaudRate(4800);
					break;
				case 3:
					MDC._baudIdx++;
					SERIAL0_BaudRate(600);
					break;
				case 4:
					MDC._baudIdx++;
					SERIAL0_BaudRate(1800);
					break;
				case 5:
					MDC._baudIdx=0;
					SERIAL0_BaudRate(2400);
					break;
			}
			MDC._now=TIM_getSecs();
		}
	}
}

void MDC_processETC(void)
{
	if(ReadETC_311())
	{
		// caso o medidor não tenha sido alocado
			medidor.id_[0] = 0x00;
			medidor.id_[1] = pima_pacote.id[0];
			medidor.id_[2] = pima_pacote.id[1];
			medidor.id_[3] = pima_pacote.id[2];
			medidor.id_[4] = pima_pacote.id[3];
			medidor.id_[5] = pima_pacote.id[4];
			medidor.t_pkg = pima_pacote.t;
			medidor.status = MEDIDOR_ATIVO; //MEDIDOR_ATIVO;
            LED1_setStatus(0);
		// atualização da medição atual
		switch((pima_pacote.escopo[0] << 8) | pima_pacote.escopo[1])
		{
		case TOTAL_E_ATIVA:
			memcpy(medidor.en_ativa,pima_pacote.dados,3);
			break;

		case TOTAL_E_INDUTIVA:
			memcpy(medidor.en_indutiva,pima_pacote.dados,3);
			break;

		case TOTAL_E_CAPACITIVA:
			memcpy(medidor.en_capacitiva,pima_pacote.dados,3);
			break;

		case TOTAL_E_ATIVA_REVERSA:
			memcpy(medidor.en_ativareversa,pima_pacote.dados,3);
			break;
		}
		memset(pima_pacote.raw,0x00,TOTAL_SIZE);
		//TIM_LED_Start(30);
	}
}
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR)
void MDC_readNBR(void)
{
	uint32_t _temp;
	uint32_t _temp2;
    if(nbr_n)
    {
        //medidor.t_pkg = TIM_getSecs();
        nbr_n=0;
        //LED1_setStatus(0);

        medidor.id_[0] = 0x00;
        medidor.id_[1] = 0x00;
        medidor.id_[2] = (uint8_t)(nbr_received.raw[1]);
        medidor.id_[3] = (uint8_t)(nbr_received.raw[2]);
        medidor.id_[4] = (uint8_t)(nbr_received.raw[3]);
        medidor.id_[5] = (uint8_t)(nbr_received.raw[4]);

        // atualização da medição atual
        switch(nbr_received.raw[0])
        {
            case 0x13:
                break;
            case 0x21:
                //medidor.read_type=nbr_received.raw[40];
                break;
            case NBR_COM_0x23LEITURAS:
				_temp=HexU8ToINT(&nbr_received.raw[5],5);
				_temp=_temp*medidor.ke;
				medidor.en_ativa[0]=IntToHexU8(_temp/100000000);
				medidor.en_ativa[1]=IntToHexU8(_temp/1000000);
				medidor.en_ativa[2]=IntToHexU8(_temp/10000);
				medidor.en_ativa[3]=IntToHexU8(_temp/100);
				medidor.en_ativa[4]=IntToHexU8(_temp/1);

				_temp=HexU8ToINT(&nbr_received.raw[80],5);
				_temp=_temp*medidor.ke;
				medidor.en_capacitiva[0]=IntToHexU8(_temp/100000000);
				medidor.en_capacitiva[1]=IntToHexU8(_temp/1000000);
				medidor.en_capacitiva[2]=IntToHexU8(_temp/10000);
				medidor.en_capacitiva[3]=IntToHexU8(_temp/100);
				medidor.en_capacitiva[4]=IntToHexU8(_temp/1);

				_temp=HexU8ToINT(&nbr_received.raw[154],5);
				_temp=_temp*medidor.ke;
				medidor.en_indutiva[0]=IntToHexU8(_temp/100000000);
				medidor.en_indutiva[1]=IntToHexU8(_temp/1000000);
				medidor.en_indutiva[2]=IntToHexU8(_temp/10000);
				medidor.en_indutiva[3]=IntToHexU8(_temp/100);
				medidor.en_indutiva[4]=IntToHexU8(_temp/1);
				medidor.t_pkg = TIM_getSecs();
				medidor.status = MEDIDOR_ATIVO;
				LED1_setStatus(0);
                break;
            case 0x80:
                _temp=HexU8ToINT(&nbr_received.raw[141],3);
                _temp2=HexU8ToINT(&nbr_received.raw[144],3);
                medidor.mode=nbr_received.raw[5];
                if(_temp==0&&_temp2==0)
                {
                    medidor.ke=1;
                }
                else
                {
                    medidor.ke=(float)_temp/(float)_temp2;
                }
                break;
            case 0x40:
                //NBR14522_readEXT(0x37, 5,nbr_received.raw[5],0x00,NULL,0,0); // 0x01 - SET
                //medidor.lastOcorrencia=nbr_received.raw[5];
                //SET_PENDENCIA(PEND.push,PEND_VTC_5_OCORRENCIA);
                break;
            default:
                break;
        }
    }
}
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT)
void MDC_readNBR(void)
{
	uint32_t _temp;
	uint32_t _temp2;
    //uint8_t _dataToSend[15]={0};
    uint8_t _readIdx=0xFF;
    uint16_t _temp16;
    float _tempfloat;
    uint32_t _pend=0;
	uint8_t _p4[4];
    if(nbr_n)
    {
        //medidor.t_pkg = TIM_getSecs();
        nbr_n=0;
        //LED1_setStatus(0);

        medidor.id_[0] = 0x00;
        medidor.id_[1] = 0x00;
        medidor.id_[2] = (uint8_t)(nbr_received.raw[1]);
        medidor.id_[3] = (uint8_t)(nbr_received.raw[2]);
        medidor.id_[4] = (uint8_t)(nbr_received.raw[3]);
        medidor.id_[5] = (uint8_t)(nbr_received.raw[4]);

        // atualização da medição atual
        switch(nbr_received.raw[0])
        {
            case 0x13:
                //medidor.auth_size=nbr_received.raw[15];
                medidor.auth_type=nbr_received.raw[255];
                //memcpy(&medidor.auth_string[0],&nbr_received.raw[16],medidor.auth_size);
                break;
            case 0x21:
                medidor.read_type=nbr_received.raw[40];
                //
                //memcpy(&ABSOLUTOID[0],&nbr_received.raw[1],4);
                //memcpy(&flash1.MDC_COPELID[2],&ABSOLUTOID[0],4);
                //ABSOLUTO_currentID[0]=ABSOLUTOID[3];
                break;
            case NBR_COM_0x23LEITURAS:
                _readIdx= ABSOLUTO_getABSIdx(&nbr_received.raw[1]);
                if(_readIdx!=0xFF)
                {
                    _temp=HexU8ToINT(&nbr_received.raw[5],5);
                    //_temp=_temp*medidor.ke;
                    //medidoresABS[ABS_readID].en_ativa[0]=medidoresABS[ABS_readID].medIDX;
                    medidoresABS[_readIdx].en_ativa[0]=IntToHexU8(_temp/100000000);
                    medidoresABS[_readIdx].en_ativa[1]=IntToHexU8(_temp/1000000);
                    medidoresABS[_readIdx].en_ativa[2]=IntToHexU8(_temp/10000);
                    medidoresABS[_readIdx].en_ativa[3]=IntToHexU8(_temp/100);
                    medidoresABS[_readIdx].en_ativa[4]=IntToHexU8(_temp/1);
                    medidoresABS[_readIdx].t_pkg = TIM_getSecs();

                    _p4[0]=(((medidoresABS[_readIdx].en_ativa[2]&0x0F)<<4)|(medidoresABS[_readIdx].en_ativa[3]>>4));
                    _p4[1]=(((medidoresABS[_readIdx].en_ativa[1]&0x0F)<<4)|(medidoresABS[_readIdx].en_ativa[2]>>4));
                    _p4[2]=(((medidoresABS[_readIdx].en_ativa[0]&0x0F)<<4)|(medidoresABS[_readIdx].en_ativa[1]>>4));
                    _p4[3]=((medidoresABS[_readIdx].en_ativa[0]>>4));
                    memcpy(&_pend,&_p4[0],4);
                    SSE_pendenciasABS_write(PEND_1_TYPE_VALOR,PEND_ABS_1_ENATIVA,_readIdx,_pend);
                    //
                    medidor.t_pkg = TIM_getSecs();
                    medidor.status = MEDIDOR_ATIVO;
                    LED1_setStatus(0);
                    if(cont == ABS_maxIDs)
                    {
                        medidor.statemachine=2;
                        cont = 0;
                    }
                    cont++;
                    //NBR14522_readEXT(0xAB, 5,0x07,0x00,NULL,0,0);
                    //TIM_LED_Start(30);
                }
                break;
            case 0x80:
                _temp=HexU8ToINT(&nbr_received.raw[141],3);
                _temp2=HexU8ToINT(&nbr_received.raw[144],3);
                medidor.mode=nbr_received.raw[5];
                if(_temp==0&&_temp2==0)
                {
                    medidor.ke=1;
                }
                else
                {
                    medidor.ke=(float)_temp/(float)_temp2;
                }
                break;
            case 0x40:
                //NBR14522_Init(&MDC_SSEID[3],&ABSOLUTO_currentID[0]);
                //NBR14522_TX(0,0x37, 5);
                NBR14522_readEXT(0x37, 5,nbr_received.raw[5],0x00,NULL,0,0); // 0x01 - SET
                medidor.lastOcorrencia=nbr_received.raw[5];
                SET_PENDENCIA(PEND.push,PEND_VTC_5_OCORRENCIA);
                break;
            case 0xAB:
                switch(nbr_received.raw[5])
                {
                    case 0x01:
                        ABSOLUTOIU[0]=nbr_received.raw[19];
                        ABSOLUTOIU[1]=nbr_received.raw[18];
                        ABSOLUTOIU[2]=nbr_received.raw[17];
                        ABSOLUTOIU[3]=nbr_received.raw[16];
                        ABSOLUTO_getIDs_AB01();
                        switch(MDC.medidor_loop_status)
                        {
                            case 0:
                                SET_PENDENCIA(PEND.push,PEND_VTC_1_SET_READABS);
                                medidor.statemachine=2;
                                MDC.read_timer=0;
                                break;
                            case 1:
                                break;
                            case 2:
                                // religa
                                //memcpy(&_temp2,&ABS_medidoresMask[0],4);
                                //if(flash3.ABS_medidoresMask==_temp2)
                                if(!memcmp(&flash3.ABS_medidoresMask[0],&ABS_medidoresMask[0],5))
                                {
                                    MDC.medidor_corte_statemachine=2;
                                }
                                else
                                {
                                    MDC.medidor_corte_statemachine=1;
                                }
                                MDC.medidor_loop_timer=0;
                                MDC.medidor_loop_retry=0;
                                //
                                break;
                            default:
                                MDC.medidor_loop_status=0;
                                break;

                        }
                        break;
                    case 0x02:
                        memcpy(&ABSOLUTOID[0],&nbr_received.raw[1],4);
                        //memcpy(&flash1.MDC_COPELID[2],&ABSOLUTOID[0],4);
                        //ABSOLUTO_currentID[0]=ABSOLUTOID[3];
                        memcpy(&ABS_medidoresMask[0],&nbr_received.raw[9],5);
                        ABSOLUTO_getMedidoresAtivos();
                        ABSOLUTO_getUCs_AB02();
                        switch(MDC.medidor_loop_status)
                        {
                            case 0:
                                medidor.statemachine=1;
                                MDC.read_timer=0;
                                break;
                            case 1:
                                break;
                            case 2:
                                // religa
                                //memcpy(&_temp2,&ABS_medidoresMask[0],4);
                                //if(flash3.ABS_medidoresMask==_temp2)
                                if(!memcmp(&flash3.ABS_medidoresMask[0],&ABS_medidoresMask[0],5))
                                {
                                    MDC.medidor_corte_statemachine=2;
                                }
                                else
                                {
                                    MDC.medidor_corte_statemachine=1;
                                }
                                MDC.medidor_loop_timer=0;
                                MDC.medidor_loop_retry=0;
                                //
                                break;
                            default:
                                MDC.medidor_loop_status=0;
                                break;
                        }
                        break;
                    case 0x06:  // ABrir/Fechar Contator
                        _readIdx= ABSOLUTO_getABSIdx(&nbr_received.raw[1]);
                        if(_readIdx!=0xFF)
                        {
#if 0
                            if((SSEMesh_getvalidGateway())&&(medidor.status&MEDIDOR_ATIVO))
                            {
                                memcpy(&_dataToSend[0],&nbr_received.raw[1],4);
                                //memcpy(&_dataToSend[4],&nbr_received.raw[7],4);
                                if(_readIdx!=0x40)
                                {
                                    _dataToSend[4]=nbr_received.raw[7];
                                }
                                else
                                {
                                    _dataToSend[4]=0;
                                }
                                if(nbr_received.raw[6]!=0x02)
                                {
                                    _dataToSend[5]=0x00;
                                }
                                else
                                {
                                    _dataToSend[5]=nbr_received.raw[6];
                                }
                                memcpy(&_dataToSend[6],&nbr_received.raw[8],3);
                                MDC_writePacket(SSE_ESCOPO_COLETOR_ABS_SET, 0xE2,_dataToSend,9);
                                SSEMesh_SendDataReply(SSE_send.raw,SSE_send.pacote_size,SSEMesh_Flags_04_ToGATEWAY);
                                //SSEMesh_SendData(ABS_toAnswerID,SSE_send.raw, SSE_send.pacote_size, 0);
                            }
#else
                            if(_readIdx!=0x40)  // nao eh medidor host
                            {
                                //AA55 5FE2 1C 11111111 250100007122 920100000072 20262560 01 00 0000 0000 0101 988D
                                break;
                                medidoresABS[_readIdx].contator=nbr_received.raw[7]; // atualiza estado contator
                                medidoresABS[_readIdx].resp_contator[0]=nbr_received.raw[7];    // estado contator

                                if(nbr_received.raw[6]!=0x02)   // diferente de erro no comando
                                {
                                    medidoresABS[_readIdx].resp_contator[1]=0x00;   // sem erro
                                }
                                else
                                {
                                    medidoresABS[_readIdx].resp_contator[1]=nbr_received.raw[6];    // retorno de erro
                                }
                                //memcpy(&medidoresABS[_readIdx].resp_contator[2],&nbr_received.raw[8],3);
                                medidoresABS[_readIdx].resp_contator[2]=nbr_received.raw[8]; // estado rele A
                                medidoresABS[_readIdx].resp_contator[4]=nbr_received.raw[9]; // estado rele B
                                medidoresABS[_readIdx].resp_contator[6]=nbr_received.raw[10]; // estado rele C
                                memcpy(&_tempfloat,&nbr_received.raw[11],4);    // tensao A
                                if(_tempfloat >= 255)
                                {
                                    _tempfloat = 255;
                                }
                                if(_tempfloat <= 0)
                                {
                                    _tempfloat = 0;
                                }
                                medidoresABS[_readIdx].resp_contator[3] = (uint8_t)_tempfloat;
                                memcpy(&_tempfloat,&nbr_received.raw[15],4);    // tensao A
                                if(_tempfloat >= 255)
                                {
                                    _tempfloat = 255;
                                }
                                if(_tempfloat <= 0)
                                {
                                    _tempfloat = 0;
                                }
                                medidoresABS[_readIdx].resp_contator[5] = (uint8_t)_tempfloat;
                                memcpy(&_tempfloat,&nbr_received.raw[18],4);    // tensao A
                                if(_tempfloat >= 255)
                                {
                                    _tempfloat = 255;
                                }
                                if(_tempfloat <= 0)
                                {
                                    _tempfloat = 0;
                                }
                                medidoresABS[_readIdx].resp_contator[7] = (uint8_t)_tempfloat;
                                /*if(_tempfloat!=0)
                                {
                                    medidoresABS[_readIdx].resp_contator[3]=1;
                                }
                                else
                                {
                                    medidoresABS[_readIdx].resp_contator[3]=0;
                                }*/
                                //memcpy(&_tempint,&nbr_received.raw[15],4);    // tensao B
                                //medidoresABS[_readIdx].resp_contator[5]= HexU8ToINT((_tempint%16),1) + HexU8ToINT((_tempint%96),1)*10;
                                /*if(_tempfloat!=0)
                                {
                                    medidoresABS[_readIdx].resp_contator[5]=1;
                                }
                                else
                                {
                                    medidoresABS[_readIdx].resp_contator[5]=0;
                                }*/
                                //memcpy(&_tempint,&nbr_received.raw[18],4);    // tensao C
                                //medidoresABS[_readIdx].resp_contator[7]= HexU8ToINT((_tempint%16),1) + HexU8ToINT((_tempint%96),1)*10;
                                /*if(_tempfloat!=0)
                                {
                                    medidoresABS[_readIdx].resp_contator[7]=1;
                                }
                                else
                                {
                                    medidoresABS[_readIdx].resp_contator[7]=0;
                                }*/
                                //SET_PENDENCIA(PENDABS[_readIdx].push,PEND_ABS_2_CONTATOR);
                            }
                            else    // medidor host apenas corte (resposta vem 1)
                            {
                                SET_PENDENCIA(PEND.push,PEND_VTC_2_CONTATOR);
                                // atualiza estado contator
                                for(_temp=0;_temp<12;_temp++)
                                {
                                    if(medidoresABS[_temp].medIDXBase!=0)
                                    {
                                        //medidoresABS[_temp].contator=nbr_received.raw[7];
                                        medidoresABS[_temp].contator=0;
                                    }
                                }
                                //
                            }

#endif
                        }
                        switch(MDC.medidor_loop_status)
                        {
                            case 0:
                                break;
                            case 1:
                                MDC.medidor_loop_status=0;
                                MDC.medidor_loop_timer=0;
                                MDC.medidor_loop_retry=0;
                                break;
                            case 2:
                                //MDC.medidor_corte_statemachine_idx++;
                                MDC.medidor_loop_timer=0;
                                medidor.statemachine=2;
                                MDC.medidor_loop_retry=0;
                                break;
                            default:
                                MDC.medidor_loop_status=0;
                                break;
                        }
                        break;
                    case 0x07:  // Estado Contatores
                        // 9 e 10 sao estados dos reles em bits
                        _temp16=nbr_received.raw[9]|(nbr_received.raw[10]<<8);
                        ABSOLUTO_getEstadoContatores(_temp16);
                        medidor.statemachine=3;
                        MDC.read_timer=0;
                        break;
                    case 0x11:  // Abertura de Secao Autenticada
                        /*medidor.auth_status=nbr_received.raw[7];
                        if((medidor.auth_status==0x00) || (medidor.auth_status==0x01))
                        {
                            MDC_include(Tipo_Medidor, Posicao_base_in, uc_t, ic_t, 1);
                            MDC_exclude(Posicao_base_ex, 1);
                            medidor.statemachine = 0;
                            MDC_readMedidor();
                            ABSOLUTO_Contator_Comando(ABS_CONT_TODO_ID, ABS_CONT_TODO_STATE, 1);
                        }
                        else
                        {
                            if((SSEMesh_getvalidGateway())&&(medidor.status&MEDIDOR_ATIVO))
                            {
                                _dataToSend[0]=SSE_ESCOPO_COLETOR_ABS_SET;
                                _dataToSend[1]=0xE2;
                                _dataToSend[2]=SSE_NACK_03_AUTH_ERROR;
                                _dataToSend[3]=medidor.auth_status;
                                MDC_writePacket(SSE_ESCOPO_COLETOR_ABS_SET, SSE_NACK,&_dataToSend[0],4);
                                SSEMesh_SendDataReply(SSE_send.raw,SSE_send.pacote_size,SSEMesh_Flags_04_ToGATEWAY);
                                //SSEMesh_SendData(ABS_toAnswerID,SSE_send.raw, SSE_send.pacote_size, 0);
                            }
                        }*/
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}
#endif
void MDC_readMedidor(void)
{
#if (MEDIDOR_TIPO == MEDIDOR_TIPO_1_PIMA)
	MDC_autoBaud();
	MDC_processETC();
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR)
    if(TIM_getSecs()- MDC.read_timer>MDC_READ_NBR_TO)
	{
        if(medidor.ke==0)
        {
            //NBR14522_read(0x21,0,NULL,0);
            NBR14522_read(0x80,0,NULL,0,0);
        }
        else
        {
            NBR14522_read(0x23,0,NULL,0,0);
        }
        MDC.read_timer= TIM_getSecs();
	}
	MDC_readNBR();
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT)
	if(TIM_getSecs()- MDC.read_timer>ABS_DEFAULT_READ_TIME)
	{
        switch(medidor.statemachine)
        {
            case 0:
#if 0
                ABSOLUTOID[0]=0x00;
                ABSOLUTOID[1]=0x00;
                ABSOLUTOID[2]=0x03;
                ABSOLUTOID[3]=0x81;
#endif
                NBR14522_Init(&flash1.MDC_SSEID[3],&ABSOLUTOID[0]);
                NBR14522_readEXT(0xAB, 5,0x02,0x00,NULL,0,0); // comando para identificar medidores ativos
                break;
            case 1:
                NBR14522_Init(&flash1.MDC_SSEID[3],&ABSOLUTOID[0]);
                NBR14522_readEXT(0xAB, 5,0x01,0x00,NULL,0,0); // comando para identificar ID dos medidores ABS
                //medidor.statemachine=2;
                break;
            case 2:
                NBR14522_Init(&flash1.MDC_SSEID[3],&ABSOLUTOID[0]);
                NBR14522_readEXT(0xAB, 5,0x07,0x00,NULL,0,0); // comando para identificar ID dos medidores ABS
                break;
            case 3:
                if(++ABS_readID>=ABS_maxIDs)
                {
                    ABS_readID=0;
                }
                if(medidoresABS[ABS_readID].medIDX!=0)
                {
                    NBR14522_Init(&flash1.MDC_SSEID[3],&medidoresABS[ABS_readID].id[0]);
                    NBR14522_read(0x23, 5,NULL,0,0);    // leitura de comando 23 para medidor individual de ABS
                    //NBR14522_readEXT(0xAB, 5,0x07,0x00,NULL,0,0);
#if 0
                    if(medidoresABS[ABS_readID].contator)
                    {
                        ABSOLUTO_Contator_Comando(&medidoresABS[ABS_readID].id[0],0,0);
                    }
                    else
                    {
                        ABSOLUTO_Contator_Comando(&medidoresABS[ABS_readID].id[0],1,0);
                    }
#endif
                }
                break;
        }
#if 0
        if(ABS_medidoresMask[0]!=0)
		{
			if(++ABS_readID>=ABS_maxIDs)
			{
				ABS_readID=0;
			}
			if(medidoresABS[ABS_readID].medIDX!=0)
			{
				ABSOLUTO_currentID[1]=medidoresABS[ABS_readID].medIDX;
				NBR14522_Init(&flash1.MDC_SSEID[3],&ABSOLUTO_currentID[0]);
				NBR14522_read(0x23, 5,NULL,0,1);    // leitura de comando 23 para medidor individual de ABS
			}
		}
		else
		{
            NBR14522_read(0x21, 5,NULL,0,1); // comando para identificar ID dos medidores ABS abaixo
			//NBR14522_readEXT(0xAB, 5,0x02,0x00,NULL,0,0); // comando para identificar ID dos medidores ABS abaixo
		}
#endif
        MDC.read_timer= TIM_getSecs();
	}
	//MDC_readNBR();
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_4_FAKE_EXT)
    uint32_t _pend=0;
	uint8_t _p4[4];
    uint32_t _temp;
    //if(TIM_getSecs()- MDC.read_timer>ABS_DEFAULT_READ_TIME)
    if(TIM_getSecs()- MDC.read_timer>600)
	{
        if(++ABS_readID>=ABS_maxIDs)
        {
            ABS_readID=0;
        }
        if(medidoresABS[ABS_readID].medIDX!=0)
        {
            //NBR14522_read(0x23, 5,NULL,0,0);    // leitura de comando 23 para medidor individual de ABS
            _temp=HexU8ToINT(&medidoresABS[ABS_readID].en_ativa[0],5);
            _temp++;
            medidoresABS[ABS_readID].en_ativa[0]=IntToHexU8(_temp/100000000);
            medidoresABS[ABS_readID].en_ativa[1]=IntToHexU8(_temp/1000000);
            medidoresABS[ABS_readID].en_ativa[2]=IntToHexU8(_temp/10000);
            medidoresABS[ABS_readID].en_ativa[3]=IntToHexU8(_temp/100);
            medidoresABS[ABS_readID].en_ativa[4]=IntToHexU8(_temp/1);
            medidoresABS[ABS_readID].t_pkg = TIM_getSecs();

            _p4[0]=(((medidoresABS[ABS_readID].en_ativa[2]&0x0F)<<4)|(medidoresABS[ABS_readID].en_ativa[3]>>4));
            _p4[1]=(((medidoresABS[ABS_readID].en_ativa[1]&0x0F)<<4)|(medidoresABS[ABS_readID].en_ativa[2]>>4));
            _p4[2]=(((medidoresABS[ABS_readID].en_ativa[0]&0x0F)<<4)|(medidoresABS[ABS_readID].en_ativa[1]>>4));
            _p4[3]=((medidoresABS[ABS_readID].en_ativa[0]>>4));
            memcpy(&_pend,&_p4[0],4);
            SSE_pendenciasABS_write(PEND_1_TYPE_VALOR,PEND_ABS_1_ENATIVA,ABS_readID,_pend);
            //
            medidor.t_pkg = TIM_getSecs();
            medidor.status = MEDIDOR_ATIVO;
            LED1_setStatus(0);
        }
        MDC.read_timer= TIM_getSecs();
    }
    medidor.t_pkg = TIM_getSecs();
#endif
    if(TIM_Fired(TIM_getSecs(), &medidor.t_pkg, PIMA_DISCONNECT_TO))
	{
        medidor.status |= MEDIDOR_DESCONECTADO;
		LED1_setStatus(1);
	}
}
#if RADIO == 1
/*
 * Quando o rádio receber algo ele vai conferir o CRC16, se o pacote é para ele e então se o comando é válido
 * (0x8F e 0x01)
 */
void MDC_radio(void)
{
	uint32_t buff;
    uint8_t temp_data[40];
    uint8_t _answer=1;
    uint8_t empty_mdc_id[6] = {0};


	if(RFM69_receiveDone())
	{
        if(RFM69_DATA[0]==0xAA)
        {
            SSE_EscreverRaw(&SSE_rcvd,(void*)&RFM69_DATA[0],(uint8_t)RFM69_DATALEN);
        }
        else
        {
            SSE_EscreverRaw(&SSE_rcvd,(void*)&RFM69_DATA[1],(uint8_t)RFM69_DATALEN-1);
        }
		if(SSE_ChecarCRC16(&SSE_rcvd))
		{
            SSE_EscreverIdCmd(&SSE_send,&SSE_rcvd.raw[SSE_START_IDCOMANDO]);
			SSE_EscreverId(&SSE_send,flash1.MDC_SSEID,SSE_SNID1_IDX);

			if(RFM69_TARGETID==flash1.MDC_RADIOID)
			{
                switch (SSE_rcvd.raw[SSE_START_ESCOPO])
				{
					case SSE_ESCOPO_COLETOR_GET:
						switch (SSE_rcvd.raw[SSE_START_ESCOPO+1])
						{
							case SSE_COLETOR_0x03_TEMPERATURA:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_GET,SSE_COLETOR_0x03_TEMPERATURA);
								temp_data[0] = (uint8_t)(IntToHexU8(RFM69_readTemperature(MDC.tempCorrection)));
								temp_data[1] = (uint8_t)(IntToHexU8(RFM69_readTemperature(MDC.tempCorrection)));
								SSE_EscreverDados(&SSE_send,temp_data,2,SSE_COLETOR);
								break;
							case SSE_COLETOR_0xA6_RADIO_NETWORK:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_GET,SSE_COLETOR_0xA6_RADIO_NETWORK);
								temp_data[0] = (uint8_t)(IntToHexU8(flash1.MDC_RADIOID/100));
								temp_data[1] = (uint8_t)(IntToHexU8(flash1.MDC_RADIOID));
								temp_data[2] = (uint8_t)(IntToHexU8(flash1.MDC_RADIONET/100));
								temp_data[3] = (uint8_t)(IntToHexU8(flash1.MDC_RADIONET));
								SSE_EscreverDados(&SSE_send,temp_data,4,SSE_COLETOR);
								break;
                            case SSE_COLETOR_0x01_USER_ID:
							case SSE_COLETOR_0xF0_FW_VERSION:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_GET,SSE_COLETOR_0xF0_FW_VERSION);
								//memcpy(&temp_data[0],(uint8_t*)&FW_VERSION,4);
                                memcpy(&temp_data[0],(uint8_t*)&flash1.MDC_FWVERSION,4);
								SSE_EscreverDados(&SSE_send,temp_data,4,SSE_COLETOR);
								break;
							case SSE_COLETOR_0xF1_HW_VERSION:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_GET,SSE_COLETOR_0xF1_HW_VERSION);
								//memcpy(&temp_data[0],(uint8_t*)&HW_VERSION,4);
                                memcpy(&temp_data[0],(uint8_t*)&SSE_HW_VERSION,4);
								SSE_EscreverDados(&SSE_send,temp_data,4,SSE_COLETOR);
								break;
							case SSE_COLETOR_0xF2_SSEID:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_GET,SSE_COLETOR_0xF2_SSEID);
								memcpy(&temp_data[0],(uint8_t*)flash1.MDC_SSEID,6);
								SSE_EscreverDados(&SSE_send,temp_data,6,SSE_COLETOR);
								break;
							case SSE_COLETOR_0xF3_CLIENTID:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_GET,SSE_COLETOR_0xF3_CLIENTID);
								memcpy(&temp_data[0],(uint8_t*)flash1.MDC_COPELID,6);
								SSE_EscreverDados(&SSE_send,temp_data,6,SSE_COLETOR);
								break;
							case SSE_COLETOR_0xF4_RUNTIME: //Contador de TEMPO Geral
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_GET,SSE_COLETOR_0xF4_RUNTIME);
								temp_data[0] = (uint8_t)(IntToHexU8((TIM_getSecs())/10000));
								temp_data[1] = (uint8_t)(IntToHexU8((TIM_getSecs())/100));
								temp_data[2] = (uint8_t)(IntToHexU8((TIM_getSecs())));
								SSE_EscreverDados(&SSE_send,temp_data,3,SSE_COLETOR);
								break;
							case SSE_COLETOR_0xFD_FLASH_INIT_FW:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_GET,SSE_COLETOR_0xFD_FLASH_INIT_FW);
								temp_data[0] = (uint8_t)(SSE_FLASH_CODE_INIT>>8);
								temp_data[1] = (uint8_t)SSE_FLASH_CODE_INIT;
                                memcpy(&temp_data[16],&SSE_FW_VERSION,4);
								temp_data[2] = temp_data[19];
								temp_data[3] = temp_data[18];
								temp_data[4] = temp_data[17];
								temp_data[5] = temp_data[16];
								SSE_EscreverDados(&SSE_send,temp_data,6,SSE_COLETOR);
								break;
							default:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_ERRO,SSE_INDICE_CMD_N_IMPLEMENTADO);
								SSE_EscreverDados(&SSE_send,temp_data,0,SSE_COLETOR);
								SSE_CalcularCRC16(&SSE_send);
								break;
						}
						if(_answer)
						{
						SSE_CalcularCRC16(&SSE_send);
                        //if(RFM69_sendWithRetry(RFM69_SENDERID,RFM69_SENDERID,&SSE_send.raw,SSE_send.pacote_size,0,1000,1))
						}
						break;
					case SSE_ESCOPO_COLETOR_SET:
						switch (SSE_rcvd.raw[SSE_START_ESCOPO+1])
						{
							case SSE_COLETOR_Ox99_INIT_PARAMETERS:
								if((SSE_rcvd.raw[SSE_START_IDCOMANDO]==0x99) &&
										(SSE_rcvd.raw[SSE_START_IDCOMANDO+1]==0x99) &&
										(SSE_rcvd.raw[SSE_START_IDCOMANDO+2]==0x99) &&
										(SSE_rcvd.raw[SSE_START_IDCOMANDO+3]==0x99))
								{
									if((flash1.MDC_RADIOID == (SSE_SNID4_DEFAULT << 8 | SSE_SNID5_DEFAULT)) &&
											(flash1.MDC_RADIONET == RFM69_DEFAULT_NETWORK_ID))
									{
										SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_SET,SSE_COLETOR_Ox99_INIT_PARAMETERS);
										flash1.MDC_RADIOID=(SSE_rcvd.raw[SSE_COLETOR_DADOS]<<8)|(SSE_rcvd.raw[SSE_COLETOR_DADOS+1]<<0);
										flash1.MDC_RADIONET=SSE_rcvd.raw[SSE_COLETOR_DADOS+2];
										memcpy(&flash1.MDC_SSEID,&SSE_rcvd.raw[SSE_COLETOR_DADOS+3],6);
										memcpy(&flash1.MDC_COPELID,&SSE_rcvd.raw[SSE_COLETOR_DADOS+9],6);
										memcpy(&flash1.MDC_DTH[0],&SSE_rcvd.raw[SSE_COLETOR_DADOS+15],5);
										RFM69_setAddress(flash1.MDC_RADIOID);
										RFM69_setNetwork(flash1.MDC_RADIONET);
#if FLASH_EN == 1
                                        SSE_reWriteFlash1();
                                        SSE_readFlash1();
#endif
										temp_data[0]=(flash1.MDC_RADIOID>>8)&0xFF;
										temp_data[1]=flash1.MDC_RADIOID&0xFF;
										temp_data[2]=flash1.MDC_RADIONET;
										memcpy(&temp_data[3],flash1.MDC_SSEID,6);
										memcpy(&temp_data[9],flash1.MDC_COPELID,6);
										memcpy(&temp_data[15],flash1.MDC_DTH,5);
										SSE_EscreverDados(&SSE_send,temp_data,20,SSE_COLETOR);
									}
									else
									{
										SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_ERRO,SSE_INDICE_CMD_N_IMPLEMENTADO);
										SSE_EscreverDados(&SSE_send,temp_data,0,SSE_COLETOR);
										SSE_CalcularCRC16(&SSE_send);
									}
								}
								else
								{
									SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_ERRO,SSE_INDICE_CMD_N_IMPLEMENTADO);
									SSE_EscreverDados(&SSE_send,temp_data,0,SSE_COLETOR);
									SSE_CalcularCRC16(&SSE_send);
								}
								break;
							case SSE_COLETOR_0xA4_RADIO_ID:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_SET,SSE_COLETOR_0xA4_RADIO_ID);
								//RADIO_ID=HexU8ToINT(&SSE_rcvd.raw[SSE_COLETOR_DADOS],2);
								if((!memcmp(&SSE_rcvd.raw[SSE_COLETOR_SNID1],flash1.MDC_SSEID, 6))
											        || (!memcmp(&SSE_rcvd.raw[SSE_COLETOR_SNID1],empty_mdc_id, 6)))
								{
								flash1.MDC_RADIOID=(SSE_rcvd.raw[SSE_COLETOR_DADOS]<<8)|(SSE_rcvd.raw[SSE_COLETOR_DADOS+1]<<0);
								RFM69_setAddress(flash1.MDC_RADIOID);
								}
								temp_data[0] = (uint8_t)(flash1.MDC_RADIOID>>8);
								temp_data[1] = (uint8_t)(flash1.MDC_RADIOID&0xFF);
								flash1.MDC_SSEID[4]=temp_data[0];
								flash1.MDC_SSEID[5]=temp_data[1];
#if FLASH_EN == 1
								SSE_reWriteFlash1();
                                SSE_readFlash1();
#endif
								//memcpy(medidor.id_,MDC_SSEID,6);
								//SSE_EscreverId(&SSE_send,medidor.id_,SSE_SNID1_IDX);
								SSE_EscreverDados(&SSE_send,temp_data,2,SSE_COLETOR);
								break;
							case SSE_COLETOR_0xA5_NETWORK_ID:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_SET,SSE_COLETOR_0xA5_NETWORK_ID);
								if((!memcmp(&SSE_rcvd.raw[SSE_COLETOR_SNID1],flash1.MDC_SSEID, 6))
											        || (!memcmp(&SSE_rcvd.raw[SSE_COLETOR_SNID1],empty_mdc_id, 6)))
								{
								flash1.MDC_RADIONET=HexU8ToINT(&SSE_rcvd.raw[SSE_COLETOR_DADOS],2);
								RFM69_setNetwork(flash1.MDC_RADIONET);
								}
								temp_data[0] = (uint8_t)(IntToHexU8(flash1.MDC_RADIONET/100));
								temp_data[1] = (uint8_t)(IntToHexU8(flash1.MDC_RADIONET));
#if FLASH_EN == 1
								SSE_reWriteFlash1();
                                SSE_readFlash1();
#endif
								SSE_EscreverDados(&SSE_send,temp_data,2,SSE_COLETOR);
								break;
							case SSE_COLETOR_0xA6_RADIO_NETWORK:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_SET,SSE_COLETOR_0xA6_RADIO_NETWORK);
								//RADIO_ID=HexU8ToINT(&SSE_rcvd.raw[SSE_COLETOR_DADOS],2);
								if((!memcmp(&SSE_rcvd.raw[SSE_COLETOR_SNID1],flash1.MDC_SSEID, 6))
											        || (!memcmp(&SSE_rcvd.raw[SSE_COLETOR_SNID1],empty_mdc_id, 6)))
								{
								flash1.MDC_RADIOID=(SSE_rcvd.raw[SSE_COLETOR_DADOS]<<8)|(SSE_rcvd.raw[SSE_COLETOR_DADOS+1]<<0);
								flash1.MDC_RADIONET=HexU8ToINT(&SSE_rcvd.raw[SSE_COLETOR_DADOS+2],2);
								RFM69_setAddress(flash1.MDC_RADIOID);
								RFM69_setNetwork(flash1.MDC_RADIONET);
								}
								temp_data[0] = (uint8_t)(flash1.MDC_RADIOID>>8);
								temp_data[1] = (uint8_t)(flash1.MDC_RADIOID&0xFF);
								temp_data[2] = (uint8_t)(IntToHexU8(flash1.MDC_RADIONET/100));
								temp_data[3] = (uint8_t)(IntToHexU8(flash1.MDC_RADIONET));
								flash1.MDC_SSEID[4]=temp_data[0];
								flash1.MDC_SSEID[5]=temp_data[1];
#if FLASH_EN == 1
								SSE_reWriteFlash1();
                                SSE_readFlash1();
#endif
								//memcpy(medidor.id_,MDC_SSEID,6);
								//SSE_EscreverId(&SSE_send,medidor.id_,SSE_SNID1_IDX);
								SSE_EscreverDados(&SSE_send,temp_data,4,SSE_COLETOR);
								break;
							case SSE_COLETOR_0xF3_CLIENTID:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_SET,SSE_COLETOR_0xF3_CLIENTID);
								memcpy(&flash1.MDC_COPELID,&SSE_rcvd.raw[SSE_COLETOR_DADOS],6);
#if FLASH_EN == 1
								SSE_reWriteFlash1();
#endif
#if MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR
								NBR14522_Init(&flash1.MDC_SSEID[3],&flash1.MDC_COPELID[2]);
#endif
								memcpy(&temp_data[0],(uint8_t*)flash1.MDC_COPELID,6);
								SSE_EscreverDados(&SSE_send,temp_data,6,SSE_COLETOR);
								break;
							case SSE_COLETOR_0xFE_RESET: // RESET
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_COLETOR_SET,SSE_COLETOR_0xFE_RESET);
								while(1);
								break;
							default:
								SSE_EscreverEscopo(&SSE_send,SSE_ESCOPO_ERRO,SSE_INDICE_CMD_N_IMPLEMENTADO);
								SSE_EscreverDados(&SSE_send,temp_data,0,SSE_COLETOR);
								SSE_CalcularCRC16(&SSE_send);
								break;
						}
						SSE_CalcularCRC16(&SSE_send);
						//if(RFM69_sendWithRetry(RFM69_SENDERID,RFM69_SENDERID,&SSE_send.raw,SSE_send.pacote_size,0,1000,1))
						break;
                    case SSE_ESCOPO_COLETOR_DRG:
                    	switch(SSE_rcvd.raw[SSE_START_ESCOPO+1])
                    	{
                    		case SSE_ESCOPO_PACK_DRG:
                    			for(uint8_t i=0;i<=6;i++){

                    			}
                    			break;
                    	}
                    default:
                        break;
                }
            }
		}
        RFM69_receiveBegin();
	}
}
#endif
void MDC_init(void)
{
	//LED_Initialize();
#if USE_FLASH == 1
	FU_InitFlash();
    SSE_Flash_Initialize();
#else
    SSE_Flash_Initialize();
	flash1.MDC_SSEID[4] = 0x01;
	flash1.MDC_SSEID[5] = 0x02;
    flash1.MDC_RADIOID = flash1.MDC_SSEID[4] << 8 | flash1.MDC_SSEID[5];
    flash1.MDC_RADIONET = 200;
#endif
#if RADIO == 1
	MDC_initRadio();
#endif
	MDC550_Meter_init();
}
void MDC_main(void)
{
#if RADIO == 1
	MDC_radio();
#endif
	MDC550_Meter_loop();
}

#if 0
// UART0
void SERIAL0_BaudRate(uint32_t _baud)
{
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
#endif
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
