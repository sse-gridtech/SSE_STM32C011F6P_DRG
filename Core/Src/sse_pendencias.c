/*
 * mdc_pendencias.c
 *
 *  Created on: 23/07/2015
 *      Author: Marcelo
 */
#include "sse_pendencias.h"
#include "custom_timer.h"

SSE_pendencia list_pendencias[NUM_PENDENCIAS];
SSE_pendencia_control PEND;
SSE_pendencia list_pendenciasABS[NUM_MED][NUM_PENDENCIAS];
SSE_pendencia_control PENDABS[NUM_MED];

#define PEND_DEFAULT_INIT	0x00000000
#define PEND_DEFAULT_INIT2	0x00FFFFFF
void SSE_pendencias_init(void)
{
	uint8_t i=0;
    uint8_t _medj=0;
    for (i=0;i<NUM_PENDENCIAS;i++)
	{
		list_pendencias[i].id=i;
		list_pendencias[i].valor=PEND_DEFAULT_INIT2;
		list_pendencias[i].last=PEND_DEFAULT_INIT2;
		list_pendencias[i].times=0;
	}
	for (_medj=0;_medj<NUM_MED;_medj++)
	{
		for (i=0;i<NUM_PENDENCIAS;i++)
		{
			list_pendenciasABS[_medj][i].id=i;
			list_pendenciasABS[_medj][i].valor=PEND_DEFAULT_INIT2;
			list_pendenciasABS[_medj][i].last=PEND_DEFAULT_INIT2;
			list_pendenciasABS[_medj][i].times=0;
		}
	}
}

void SSE_pendencias_check(void)
{
	uint8_t i=0;
    uint8_t _medj=0;
	for (i=0;i<NUM_PENDENCIAS;i++)
	{
		if(CHECK_PENDENCIA(PEND.pool,i))
		{
			if(list_pendencias[i].last!=list_pendencias[i].valor)
			{
				list_pendencias[i].times++;
				if(list_pendencias[i].times>0)
				{
					SET_PENDENCIA(PEND.push,i);
					list_pendencias[i].times=0;
					list_pendencias[i].latencia_last=list_pendencias[i].latencia;
					list_pendencias[i].latencia=TIM_getSecs();
				}
			}
			list_pendencias[i].last=list_pendencias[i].valor;
		}
	}

	for (_medj=0;_medj<NUM_MED;_medj++)
	{
		for (i=0;i<NUM_PENDENCIAS;i++)
		{
			if(CHECK_PENDENCIA(PENDABS[_medj].pool,i))
			{
				if(list_pendenciasABS[_medj][i].last!=list_pendenciasABS[_medj][i].valor)
				{
					list_pendenciasABS[_medj][i].times++;
					if(list_pendenciasABS[_medj][i].times>0)
					{
						SET_PENDENCIA(PENDABS[_medj].push,i);
						list_pendenciasABS[_medj][i].times=0;
						list_pendenciasABS[_medj][i].latencia_last=list_pendenciasABS[_medj][i].latencia;
                        list_pendenciasABS[_medj][i].latencia=TIM_getSecs();
					}
				}
				list_pendenciasABS[_medj][i].last=list_pendenciasABS[_medj][i].valor;
			}
		}
	}
}

void SSE_pendencias_write(uint8_t position, uint8_t it, uint32_t value)
{
	switch (position)
	{
		case 1:
			list_pendencias[it].valor=value;
			break;
        case 2:
            list_pendencias[it].last=value;
			break;
		default:
			break;
	}
}

void SSE_pendenciasABS_write(uint8_t position, uint8_t it, uint8_t _medj, uint32_t value)
{
	switch (position)
	{
		case 1:
			list_pendenciasABS[_medj][it].valor=value;
			break;
		case 2:
			list_pendenciasABS[_medj][it].last=value;
			break;
		default:
			break;
	}
}
