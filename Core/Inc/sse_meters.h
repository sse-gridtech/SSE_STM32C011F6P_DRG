/*************************************************************
filename: ETC_3_11.h

Modified by: Marcelo Sugita
description: Recebe os dados via serial padrao ETC311 e coloca em um pacote
version: v1.0a (02/04/2015)

Created on: 07/03/2014
filename:ETC_3_11.h
Author: THIAGO
*************************************************************/
#ifndef SSE_METERS_H_
#define SSE_METERS_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sse_defines.h"

#define PIMA_DISCONNECT_TO 30
#define MDC_READ_NBR_TO 7

#if MEDIDOR_TIPO == MEDIDOR_TIPO_1_PIMA
#define NUM_MEDIDORES		1U	/* número de medidores conectados */

/* definições acerca dos campos do pacote */
#define SYNC_BYTE_0_START	0U
#define SYNC_BYTE_1_START	1U
#define PREAMBULO_START		0U
#define PREAMBULO_SIZE		2U
#define ID_START			(PREAMBULO_START + PREAMBULO_SIZE)
#define ID_SIZE				5U
#define TAMANHO_START		(ID_START + ID_SIZE)
#define TAMANHO_SIZE		1U
#define ESCOPO_START		(TAMANHO_START + TAMANHO_SIZE)
#define ESCOPO_SIZE			2U
#define DADOS_START			(ESCOPO_START + ESCOPO_SIZE)
#define DADOS_SIZE			21U//(255U - ESCOPO_SIZE)
#define	CRC16_SIZE			2U
#define TOTAL_SIZE			(PREAMBULO_SIZE + 	/*Preambulo */  \
							ID_SIZE + 			/* ID */ 		\
							TAMANHO_SIZE + 		/* Tamanho */	\
							ESCOPO_SIZE +		/* Escopo */	\
							DADOS_SIZE +		/* Dados */		\
							CRC16_SIZE)			/* CRC */		\

/* valores para os campos */
#define SYNC_BYTE_0			0xAA
#define SYNC_BYTE_1			0x55

/* definiçoes acerca dos tipos de medição enviados */
#define TOTAL_E_ATIVA				0x0A02	/* Totalizador de Energia Ativa */
#define TOTAL_E_INDUTIVA			0x0A07	/* Totalizador de Energia Indutiva */
#define TOTAL_E_CAPACITIVA			0x0A0C 	/* Totalizador de Energia Capacitiva */
#define TOTAL_E_ATIVA_REVERSA		0x0A51	/* Totalizador de Energia Ativa Reversa */

#define PARAM_PKT					0x1F99 // PARAMETRIZACAO

typedef struct {
	uint8_t				pacote_status;			/* status atual do pacote */
	uint32_t			t;						/* instante em q foi recebido */
	union {
		struct
		{
			uint8_t				sync0;
			uint8_t				sync1;
			uint8_t				id[ID_SIZE];			/* identificador do pacote */
			uint8_t				tamanho[TAMANHO_SIZE];	/* tamanho da parte de escopo + dados */
			uint8_t				escopo[ESCOPO_SIZE];	/* escopo e índice do pacote */
			uint8_t				dados[DADOS_SIZE];		/* parte de dados */
			uint8_t				crc16[CRC16_SIZE];		/* crc16 do pacote */
		};
		uint8_t					raw[TOTAL_SIZE];
	};
}PacoteETC_3_11;
extern PacoteETC_3_11	pima_pacote;

uint8_t ReadETC_311(void);
uint16_t PIMA_checkCRC(void);

#else
#define NBR_NUM_MEDIDORES		1U

#define NBR_COMANDO_0		0U
#define NBR_ID_0			1U
//COMANDO 0x23
#define NBR_ATIVA_0			5U
#define NBR_ATIVA_SIZE		5U
#define NBR_INDUTIVA_0		80U
#define NBR_INDUTIVA_SIZE	5U
#define NBR_CAPACITIVA_0	154U
#define NBR_CAPACITIVA_SIZE	5U
#define NBR_CRC16_0			256U
#define	NBR_CRC16_SIZE			2U
#define NBR_TOTAL_SIZE			258U

/* valores para os campos */
#define NBR_COM_0x23LEITURAS	0x23

typedef struct
{
#if (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR)
	uint8_t		raw[66];
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT)
	uint8_t		raw[66+5];
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_4_FAKE_EXT)
	uint8_t		raw[66+5];
#endif
	uint8_t		_id[4];
	uint8_t		_lastCommand;
	uint8_t		_reply;
}NBR14522_Tx;

typedef struct
{
    uint8_t		raw[NBR_TOTAL_SIZE];
}NBR14522_Rx;

typedef struct{
    float		potA;
	float		potB;
	float		potC;
    uint16_t    seqFas;
    uint8_t     potenciasNegativas;
    uint8_t     sequenciaFases;
    uint16_t    c21_difftime;
    uint8_t     c21_signaltime;
}NBR_com14_data;
extern NBR_com14_data NBR_COM14;

typedef struct{
	uint8_t		version[2];
    uint8_t		model[2];
    uint8_t     fechamentos;
    uint8_t     inc_relogio;
}NBR_com21_data;

extern NBR14522_Rx  nbr_received;		/* ponteiro para buffer para ser lido */
extern NBR14522_Tx	nbr_transmit;
extern uint8_t	nbr_n;

void NBR14522_Init(uint8_t *mdcID, uint8_t *_nbrMedID);
uint8_t NBR14522_BuildPacketSend(uint8_t command, uint8_t idx, uint8_t* datas, uint8_t datas_size);
uint8_t NBR14522_BuildPacketSendEXT(uint8_t command, uint8_t idx, uint8_t subcommand, uint8_t type, uint8_t* datas, uint8_t datas_size);
uint16_t NBR14522_CRC16(uint16_t size);
uint16_t NBR_RecebeOptica(uint8_t* byte, uint16_t n);

uint8_t NBR14522_read(uint8_t command, uint8_t idx, uint8_t *_pay, uint8_t _psize, uint8_t _ack);
uint8_t NBR14522_readEXT(uint8_t command, uint8_t idx, uint8_t subcommand, uint8_t type, uint8_t* _pay, uint8_t _psize, uint8_t _ack);
uint8_t NBR14522_readBypass(uint8_t *_pkt, uint8_t _psize);
uint8_t NBR14522_ProcessSerial(void);
uint8_t NBR14522_getErrors(void);
#endif
#endif /* ETC_3_11_H_ */
