/*
 * mdc_pendencias.h
 *
 *  Created on: 24/07/2015
 *      Author: Marcelo
 */
#ifndef SSE_PENDENCIAS_H_
#define SSE_PENDENCIAS_H_

#include "sse_defines.h"

#define NUM_MED         12
#define NUM_PENDENCIAS  16
#define CHECK_PENDENCIA(var,pos) ((var) & (1<<(pos)))
#define CHECK_PENDENCIA_MASK(var,pos) ((var) & (pos))
#define SET_PENDENCIA(var,pos) ((var) |= (1<<(pos)))

typedef struct{
	uint8_t 	id;
	uint32_t 	valor;
	uint32_t 	last;
	uint32_t 	latencia;
	uint32_t 	latencia_last;
	uint16_t 	times;
}SSE_pendencia;
extern SSE_pendencia list_pendencias[NUM_PENDENCIAS];
extern SSE_pendencia list_pendenciasABS[NUM_MED][NUM_PENDENCIAS];

typedef struct{
	uint16_t 	push;
	uint16_t 	pool;
}SSE_pendencia_control;
extern SSE_pendencia_control PEND;
extern SSE_pendencia_control PENDABS[NUM_MED];

enum PENDENCIA_TYPE {
	PEND_1_TYPE_VALOR			= 0x01,
	PEND_2_TYPE_LAST			= 0x02,
};
enum PENDENCIA_VTC_SET {
    PEND_VTC_0_SET_TURNON               = 0,
	PEND_VTC_1_SET_READABS              = 1,
    PEND_VTC_2_CONTATOR                 = 2,
    PEND_VTC_3_ALARME                   = 3,
    PEND_VTC_4_CUSTOM                   = 4,
    PEND_VTC_5_OCORRENCIA               = 5,
    PEND_VTC_3_ALARME_FECHADO           = 6,
    PEND_VTC_3_ALARME_ABERTO            = 3,
};
enum PENDENCIA_ABS {
    PEND_ABS_0_UC               = 0,
	PEND_ABS_1_ENATIVA			= 1,
	PEND_ABS_2_CONTATOR			= 2,
    PEND_ABS_3_CUSTOM			= 3,
};
void SSE_pendencias_init(void);
void SSE_pendencias_check(void);
void SSE_pendencias_write(uint8_t position, uint8_t it, uint32_t value);
void SSE_pendenciasABS_write(uint8_t position, uint8_t it, uint8_t _medj, uint32_t value);

#endif /* MDC500_PENDENCIAS_H_ */
