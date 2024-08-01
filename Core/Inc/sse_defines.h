/***********************************************************************************************************************
* File Name    : sse_define.h
* Version      : v0: initial release
* Description  : SSE definitions about hardware and firmware
* Creation Date: 17/03/2017
***********************************************************************************************************************/
//#include "r_cg_macrodriver.h"
//#include "sse_hw_def.h"
//#include <stdlib.h>
#ifndef _SSE_DEF_H
#define _SSE_DEF_H
#include "sse_definitions.h"

//89550680257001197236
//--------FW VERSION----------------------
static const uint16_t SSE_FLASH_CODE_INIT = 0x2000;
static const uint32_t SSE_HW_VERSION    =0x02010003;

static const uint32_t SSE_FW_VERSION    =0x01000012;
static const uint32_t SSE_FW_VERSION_0=0x01;
static const uint32_t SSE_FW_VERSION_1=0x00;
static const uint32_t SSE_FW_VERSION_2=0x00;
static const uint32_t SSE_FW_VERSION_3=0x12;
static const uint32_t DTHL=0x10130319; 	// HH DD MM AA
static const uint8_t  DTHM=0x25;		// mm
//--------MEDIDOR TIPO----------------------
#define RADIO 1
#if RADIO == 0
#define MEDIDOR_TIPO    MEDIDOR_TIPO_1_PIMA //MEDIDOR_TIPO_2_NBR //MEDIDOR_TIPO_3_NBR_EXT  //MEDIDOR_TIPO_1_PIMA
#else
#define MEDIDOR_TIPO    MEDIDOR_TIPO_0_NOMED
#endif
#endif
