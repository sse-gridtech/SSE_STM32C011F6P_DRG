/*************************************************************
filename: sse.protocol.h

Author: Marcelo Sugita
description: Cria e formata os pacotes no protocolo SSE
version: v1.0a (03/03/2017)

based on protocolo BEi
*************************************************************/
#include "sse_defines.h"
#include "sse_util.h"

#ifndef SSE_PROTOCOL_H_
#define SSE_PROTOCOL_H_

/*************************************************************
Protocolo SSE

SYC SYC ESC IND SIZ   ID COMANDO		DADOS	CRC
AA  55  0F  01  XX  00  00  00  01  XX  XX  XX  CC  CC
*************************************************************/

#define ETH_DADOS_MAX_SIZE		100U
#define RFM69_DADOS_MAX_SIZE	66U
#define TRANSPORT_MAX_SIZE		ETH_DADOS_MAX_SIZE

#define SSE_SYNC_0				0xAA
#define SSE_SYNC_1				0x55
#define SSE_SIZE_CRC16			2U

#define SSE_START_SYNC			0U
#define SSE_SIZE_SYNC			2U
#define SSE_START_ESCOPO		(SSE_START_SYNC + SSE_SIZE_SYNC)
#define SSE_SIZE_ESCOPO			2U
#define SSE_START_TAMANHO		(SSE_START_ESCOPO + SSE_SIZE_ESCOPO)
#define	SSE_SIZE_TAMANHO		1U
#define SSE_START_IDCOMANDO		(SSE_START_TAMANHO + SSE_SIZE_TAMANHO)
#define SSE_SIZE_IDCOMANDO		4U
#define SSE_SIZE_HEADER_TOTAL	(SSE_SIZE_SYNC + \
								 SSE_SIZE_ESCOPO + \
								 SSE_SIZE_TAMANHO + \
								 SSE_SIZE_IDCOMANDO + \
								 SSE_SIZE_CRC16)
#define SSE_SIZE_HEADER			(SSE_SIZE_SYNC + \
								 SSE_SIZE_ESCOPO + \
								 SSE_SIZE_TAMANHO + \
								 SSE_SIZE_CRC16)
#define SSE_START_DADOS			(SSE_START_IDCOMANDO + SSE_SIZE_IDCOMANDO)
#define SSE_SIZEMAX_DADOS		(TRANSPORT_MAX_SIZE - SSE_SIZE_HEADER_TOTAL)
#define SSE_SIZE_TOTAL			(SSE_SIZE_HEADER_TOTAL + SSE_SIZEMAX_DADOS)

//  SSE - EQUIP/TRANSPORT TYPE
#define SSE_COLETOR				1U
#define SSE_CCD					2U
#define SSE_CCD_COLETOR			3U
//  SSE - DADOS TEMPLATES
#define SSE_SNID0_START			SSE_START_DADOS
#define SSE_SNID0_SIZE			6U
#define SSE_SNID0_IDX			0U
#define SSE_SNID1_START			(SSE_SNID0_START + SSE_SNID0_SIZE)
#define SSE_SNID1_SIZE			6U
#define SSE_SNID1_IDX			1U

// SSE - AUXILIAR COLETOR
#define SSE_COLETOR_SNID0		(SSE_SNID0_START)
#define SSE_COLETOR_SNID1		(SSE_SNID1_START)
#define SSE_COLETOR_SNID_TOTAL	(SSE_SNID0_SIZE+SSE_SNID1_SIZE)
#define SSE_COLETOR_DADOS		(SSE_SNID1_START + SSE_SNID1_SIZE)
// SSE - AUXILIAR CCD
#define SSE_CCD_SNID			(SSE_SNID0_START)
#define SSE_CCD_SNID_TOTAL		(SSE_SNID0_SIZE)
#define SSE_CCD_DADOS			(SSE_SNID0_START + SSE_SNID0_SIZE)
// SSE - AUXILIAR COLETOR
#define SSE_CCD_COLETOR_SNID0		(SSE_SNID0_START)
#define SSE_CCD_COLETOR_SNID1		(SSE_SNID1_START)
#define SSE_CCD_COLETOR_SNID_TOTAL	(SSE_SNID0_SIZE+SSE_SNID1_SIZE)
#define SSE_CCD_COLETOR_DADOS		(SSE_SNID1_START + SSE_SNID1_SIZE)
// SSE - AUXILIAR CCD
#define ETH_DADOS_SN_CCD_START	    (ETH_ID_COMANDO_START + ETH_ID_COMANDO_SIZE)
#define ETH_DADOS_SN_CCD_SIZE		 6U
#define ETH_DADOS_SN_MEDIDOR_START  (ETH_DADOS_SN_CCD_START  + ETH_DADOS_SN_CCD_SIZE)
#define ETH_DADOS_SN_MEDIDOR_SIZE	 6U
#define ETH_DADOS_RESTANTES_START	(ETH_DADOS_SN_MEDIDOR_START + ETH_DADOS_SN_MEDIDOR_SIZE)

/* futuras necessidades
#define ETH_SN_CCD_START		(ETH_ID_COMANDO_START + ETH_ID_COMANDO_SIZE)
#define ETH_SN_CCD_SIZE			6U
#define ETH_ID_GRUPO_START		(ETH_SN_CCD_START + ETH_SN_CCD_SIZE)
#define ETH_ID_GRUPO_SIZE		2U
#define ETH_POS_START			(ETH_ID_GRUPO_START + ETH_ID_GRUPO_SIZE)
#define ETH_PAS_SIZE			1U
 */

//	SSE - ESCOPOS
#define SSE_ESCOPO_COLETOR_GET		0x0F
#define SSE_ESCOPO_COLETOR_SET		0x1F
#define SSE_ESCOPO_COLETOR_ABS_GET	0x4F
#define SSE_ESCOPO_COLETOR_ABS_SET	0x5F
#define SSE_ESCOPO_COLETOR_DRG  	0x9F
#define SSE_ESCOPO_PACK_DRG  	0x02
#define SSE_ESCOPO_CCD_GET		0x0C
#define SSE_ESCOPO_CCD_SET		0x1C
#define SSE_ESCOPO_CCD_FORCE		0xFC
#define SSE_ESCOPO_FW			0x0B
#define	SSE_ESCOPO_ERRO			0xEE
#define	SSE_ESCOPO_ACK			0x06

// SSE - INDICES COLETOR
enum SSE_COLETOR_INDICE {
	SSE_COLETOR_0x01_USER_ID			= 0x01,				// sinalização pelo medidor de que está disponível
	SSE_COLETOR_0x02_ID_MEDIDOR			= 0x02,				// status do medidor
	SSE_COLETOR_0x03_TEMPERATURA			= 0x03,				// histórico de medições
	SSE_COLETOR_0x04_ALARMES			= 0x04,
	SSE_COLETOR_0x05_PENDENCIAS			= 0x05,
	SSE_COLETOR_0x06_PEND_MASK			= 0x06,
	SSE_COLETOR_0x07_		= 0x07,
	SSE_COLETOR_0x08_		= 0x08,
	SSE_COLETOR_0x09_		= 0x09,
	SSE_COLETOR_0x0A_LATENCIA			= 0x0A,
	SSE_COLETOR_0x0B_ATTACH				= 0x0B,
	SSE_COLETOR_0x0C_ATTACH_SIP			= 0x0C,
	SSE_COLETOR_0x55_CUSTOM_MDC			= 0x55,
	SSE_COLETOR_0x66_CUSTOM_MCR			= 0x66,
	SSE_COLETOR_0x77_CUSTOM_SIP			= 0x77,
	SSE_COLETOR_0x91_CUSTOM_SIP_BROAD		= 0x91,
	SSE_COLETOR_0x92_CUSTOM_MDC_BROAD		= 0x92,
	SSE_COLETOR_Ox99_INIT_PARAMETERS	= 0x99,
	SSE_COLETOR_0xA1_RSSI				= 0xA1,
	SSE_COLETOR_0xA2_RETRY_TO			= 0xA2,
	SSE_COLETOR_0xA3_RETRY_N			= 0xA3,
	SSE_COLETOR_0xA4_RADIO_ID			= 0xA4,
	SSE_COLETOR_0xA5_NETWORK_ID			= 0xA5,
	SSE_COLETOR_0xA6_RADIO_NETWORK			= 0xA6,
	SSE_COLETOR_0xA7_RADIO_PARAM			= 0xA7,
	SSE_COLETOR_0xA8_RADIO_RESET			= 0xA8,
	SSE_COLETOR_0xA9_GATEWAY_ID			= 0xA9,
	SSE_COLETOR_0xAA_ADD_HOP			= 0xAA,
	SSE_COLETOR_0xAB_DELETE_HOP			= 0xAB,
	SSE_COLETOR_0xAC_GET_HOP			= 0xAC,
	SSE_COLETOR_0xAD_CLEAR_HOP_TABLE		= 0xAD,
	SSE_COLETOR_0xB2_ENERGIA_ATIVA			= 0xB2,
	SSE_COLETOR_0xB3_ENERGIA_ATIVA_LAT		= 0xB3,
	SSE_COLETOR_0xB4_ALARME_SEM_I			= 0xB4,
	SSE_COLETOR_0xB5_ALARME_SOBRE_I			= 0xB5,
	SSE_COLETOR_0xB6_ALARME_SUB_I			= 0xB6,
	SSE_COLETOR_0xC0_ADD_GRUPO			= 0xC0,
	SSE_COLETOR_0xD1_V_INSTANTANEA			= 0xD1,
	SSE_COLETOR_0xD2_I_CALIB			= 0xD2,
	SSE_COLETOR_0xD3_I_INSTANTANEA			= 0xD3,
	SSE_COLETOR_0xD4_CREP_INSTANTANEA		= 0xD4,
	SSE_COLETOR_0xDF_RESET_EACC			= 0xDF,
	SSE_COLETOR_0xE0_CORTE_RELIGA			= 0xE0,
	SSE_COLETOR_0xE1_CORTE_RELIGA_G 		= 0xE1,
	SSE_COLETOR_0xE2_ESTADO_CONT			= 0xE2,
	SSE_COLETOR_0xE3_ESTADO_CONT_LAT		= 0xE3,
	SSE_COLETOR_0xE4_TENSAO_ENTRADA			= 0xE4,
	SSE_COLETOR_0xE5_BASE_LIGHT_SIP			= 0xE5,
	SSE_COLETOR_0xE6_OFFSET_LIGHT_SIP		= 0xE6,
	SSE_COLETOR_0xE7_POTENCIA_LIGHT_SIP		= 0xE7,
	SSE_COLETOR_0xE8_POTENCIA_OFF_SOB		= 0xE8,
	SSE_COLETOR_0xE9_BYPASS				= 0xE9,
	SSE_COLETOR_0xEA_POTENCIA_OFF_SUB		= 0xEA,
	SSE_COLETOR_0xF0_FW_VERSION			= 0xF0,
	SSE_COLETOR_0xF1_HW_VERSION			= 0xF1,
	SSE_COLETOR_0xF2_SSEID				= 0xF2,
	SSE_COLETOR_0xF3_CLIENTID			= 0xF3,
	SSE_COLETOR_0xF4_RUNTIME			= 0xF4,
	SSE_COLETOR_0xF5_SELECTID			= 0xF5,
	SSE_COLETOR_0xFA_FLASH_INIT_SECTION		= 0xFA,
	SSE_COLETOR_0xFB_RESET_SOURCE			= 0xFB,
	SSE_COLETOR_0xFC_RADIO_ACK_TIME			= 0xFC,
	SSE_COLETOR_0xFD_FLASH_INIT_FW			= 0xFD,
	SSE_COLETOR_0xFE_RESET				= 0xFE,
	SSE_COLETOR_0xFF_UPDATE_FIRMWARE		= 0xFF,
};

// SSE - INDICES CCD
enum SSE_CCD_INDICE_GET {
	SSE_CCD_0x01_PING		= 0x01,
	SSE_CCD_0x02_LIST_SIZE		= 0x02,
	SSE_CCD_0x03_ETHERNET_PARAM	= 0x03,
	SSE_CCD_0x04_MQTT_PARAM		= 0x04,
	SSE_CCD_0x05_MEDIDORES_LISTA	= 0x05,
	SSE_CCD_0x06_HOP_TABLE		= 0x06,
};

enum SSE_CCD_INDICE_SET {
	SSE_CCD_0x01_DOMAIN			= 0x01,
	SSE_CCD_0x02_MQTT_COMPANY_MAINTOPIC	= 0x02,
	SSE_CCD_0x03_MQTT_PORT			= 0x03,
	SSE_CCD_0x04_CONNECTION_TYPE		= 0x04,
	SSE_CCD_0x05_IP_ADDRESS			= 0x05,
	SSE_CCD_0x06_GT_ADDRESS			= 0x06,
	SSE_CCD_0x07_SM_ADDRESS			= 0x07,
	SSE_CCD_0x08_DNS_ADDRESS		= 0x08,
	SSE_CCD_0x09_MAC_ADDRESS		= 0x09,
	SSE_CCD_0x0A_IPHOST_ADDRESS		= 0x0A,
	SSE_CCD_0x0B_ADD_REPEAT_HOP		= 0x0B,
	SSE_CCD_0x0C_DELETE_REPEAT_HOP		= 0x0C,
	SSE_CCD_0x0D_CLEAR_HOP_TABLE		= 0x0D,
	SSE_CCD_0x0E_ETHERNET_PARAM		= 0x0E,
};

enum SSE_FW
{
	SSE_FW_SAME_VERSION_OR_OLDER 	= 0xFF,
	SSE_FW_OK			= 0x06,
	SSE_FW_RECEPTION_PERM		= 0x10,
	SSE_FW_RECEPTION_OK		= 0x11,
	SSE_FW_FORCE			= 0x20,
	SSE_SWAP_SIP			= 0x30,
	SSE_SWAP_CCD			= 0x40,
};
enum SSE_RESPOSTA_CMD
{
	SSE_INDICE_ACK				= 0x06,
	SSE_INDICE_ERRO_DE_COMUNIC		= 0xEE,
	SSE_INDICE_CMD_N_IMPLEMENTADO		= 0x00
};

enum SSE_NAKERROR_CMD
{
	SSE_ACK                         = 0x06,
    SSE_NACK                        = 0x15,
    SSE_NACK_01_ABS_NOT_FOUND		= 0x01,
    SSE_NACK_02_SESSION_ERROR		= 0x02,
    SSE_NACK_03_AUTH_ERROR			= 0x03,
};

typedef struct {
	uint8_t             pacote_size;			/* tamanho do pacote */
	uint8_t				dados_count;			/* quantidade de dados */
	uint8_t				pacote_status;			/* status atual do pacote */

	uint8_t				raw[SSE_SIZE_TOTAL];	/* bytes que compõe o pacote */
}SSE_Pacote;

extern	SSE_Pacote SSE_send;
extern  SSE_Pacote SSE_rcvd;

/*
 *	Função		:	void SSE_EscreverBytes (SSE_Pacote* pkg, uint8_t _position, uint8_t* _data, uint8_t _datasize);
 *	Descrição	:	Escreve no pacote passado os dados na posicao desejada
 *
 *	Parâmetros	:	pkg 		- ponteiro para o pacote
 *					_position	- posicao a se escrever os dados
 *					_data		- ponteiro para os dados a serem escritos
 *					_datasize	- quantidade de dados
 */
void SSE_EscreverBytes(SSE_Pacote* pkg, uint8_t _position, uint8_t* _data, uint8_t _datasize);

/*
 *	Função		:	void SSE_EscreverId(SSE_Pacote* pkg, uint8_t* id);
 *	Descrição	:	Escreve no pacote passado a informação do ID do
 *					medidor
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *					id	- ponteiro para o id (deve ter o
 *						  tamanho especificado por SSE_ID_SIZE)
 */
void SSE_EscreverId(SSE_Pacote* pkg, uint8_t* id, uint8_t _idx);

/*
 *	Função		:	void SSE_EscreverEscopo(SSE_Pacote* pkg, uint8_t e, uint8_t f);
 *	Descrição	:	Escreve no pacote passado o escopo ao
 *					qual o pacote se refere
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *					e	- valor do escopo
 */
void SSE_EscreverEscopo(SSE_Pacote* pkg, uint8_t e, uint8_t f);

/*
 *	Função		:	void SSE_EscreverDados(SSE_Pacote* pkg, uint8_t* d, uint8_t t);
 *	Descrição	:	Escreve no pacote passado os dados passados
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *					d	- ponteiro para o conjunto de dados
 *					t	- tamanho do conjunto de dados passados. Deve ser
 *						  menor que o valor especificado em SSE_DADOS_MAX_SIZE
 */
void SSE_EscreverDados(SSE_Pacote* pkg, uint8_t* d, uint8_t t, uint8_t _transport);

/*
 *	Função		:	void SSE_EscreverRaw(SSE_Pacote* pkg, uint8_t* d, uint8_t t);
 *	Descrição	:	Sobrescreve o pacote passado com tudo que está em *d
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *					d	- ponteiro com os bytes de interesse
 *					t	- tamanho do conjunto de bytes passados. Deve ser
 *						  menor que o valor especificado em SSE_TOTAL_SIZE
 */
void SSE_EscreverRaw(SSE_Pacote* pkg, uint8_t* d, uint8_t t);

/*
 *	Função		:	uint16_t SSE_CalcularCRC16(SSE_Pacote* pkg);
 *	Descrição	:	Calcula o CRC-16 do pacote passado retornando
 *					seu valor.
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *
 *	Retorno		:	(uint16_t) - valor crc-16 calculado
 */
uint16_t SSE_CalcularCRC16(SSE_Pacote* pkg);

/*
 *	Função		:	uint16_t SSE_ChecarCRC16(SSE_Pacote* pkg);
 *	Descrição	:	Checa se o pacote passado tem o CRC-16 válido
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *
 *	Retorno		:	(BOOL) - TRUE caso sucesso
 *							 FALSE caso falha
 */
bool SSE_ChecarCRC16(SSE_Pacote* pkg);

void SSE_EscreverIdCmd(SSE_Pacote* pkg, uint8_t* id_cmd);

#endif /* SSE_PROTOCOLO_H_ */
