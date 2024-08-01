/*************************************************************
filename: sse.protocol.h

Author: Marcelo Sugita
description: Cria e formata os pacotes no protocolo SSE
version: v1.0a (03/03/2017)

based on protocolo BEi
*************************************************************/
#include "sse_protocol.h"
#include <string.h>

SSE_Pacote SSE_send;
SSE_Pacote SSE_rcvd;

/*
 *	Função		:	void SSE_EscreverBytes (SSE_Pacote* pkg, uint8_t _position, uint8_t* _data, uint8_t _datasize);
 *	Descrição	:	Escreve no pacote passado os dados na posicao desejada
 *
 *	Parâmetros	:	pkg 		- ponteiro para o pacote
 *					_position	- posicao a se escrever os dados
 *					_data		- ponteiro para os dados a serem escritos
 *					_datasize	- quantidade de dados
 */
void SSE_EscreverBytes(SSE_Pacote* pkg, uint8_t _position, uint8_t* _data, uint8_t _datasize)
{
	uint8_t i = 0;
	for(;i < _datasize;i++)
	{
		pkg->raw[_position + i] = _data[i];
	}
}
/*
 *	Função		:	void SSE_EscreverId(SSE_Pacote* pkg, uint8_t* id);
 *	Descrição	:	Escreve no pacote passado a informação do ID do
 *					medidor que está TX ou o ID do medidor que deve
 *					receber a mensagem
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *					id	- ponteiro para o id (deve ter o
 *						  tamanho especificado por SSE_ID_SIZE)
 */
void SSE_EscreverId(SSE_Pacote* pkg, uint8_t* id, uint8_t _idx)
{
	if(_idx == SSE_SNID0_IDX)
	{
		SSE_EscreverBytes(pkg,SSE_SNID0_START,id,SSE_SNID0_SIZE);
		pkg->dados_count = SSE_SIZE_IDCOMANDO + SSE_SNID0_SIZE;
	}
	else if(_idx == SSE_SNID1_IDX)
	{
		SSE_EscreverBytes(pkg,SSE_SNID1_START,id,SSE_SNID1_SIZE);
		pkg->dados_count = SSE_SIZE_IDCOMANDO + SSE_SNID0_SIZE+ SSE_SNID1_SIZE;
	}
}

/*
 *	Função		:	void SSE_EscreverEscopo(SSE_Pacote* pkg, uint8_t e);
 *	Descrição	:	Escreve no pacote passado o escopo ao
 *					qual o pacote se refere
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *					e	- valor do escopo
 */
void SSE_EscreverEscopo(SSE_Pacote* pkg, uint8_t e, uint8_t f)
{
	pkg->raw[SSE_START_ESCOPO] = e; //add +5 dada config para bluetooth
	pkg->raw[SSE_START_ESCOPO + 1] = f; // add +5 dada config para bluetooth
}

/*
 *	Função		:	void SSE_EscreverDados(SSE_Pacote* pkg, uint8_t* d, uint8_t t);
 *	Descrição	:	Escreve no pacote passado os dados passados
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *					d	- ponteiro para o conjunto de dados
 *					t	- tamanho do conjunto de dados passados. Deve ser
 *						  menor que o valor especificado em SSE_DADOS_MAX_SIZE,
 *						  caso contrário só os SSE_DADOS_MAX_SIZE primeiros bytes
 *						  serão copiados
 */
void SSE_EscreverDados(SSE_Pacote* pkg, uint8_t* d, uint8_t t,uint8_t _transport)
{
	t = (uint8_t)(t < SSE_SIZEMAX_DADOS ? t : SSE_SIZEMAX_DADOS);
	switch (_transport)
	{
		case SSE_CCD:
			SSE_EscreverBytes(pkg,SSE_CCD_DADOS,d,t);
			pkg->dados_count = t + SSE_CCD_SNID_TOTAL + SSE_SIZE_IDCOMANDO;
			break;
		case SSE_COLETOR:
			//SSE_EscreverBytes(pkg,SSE_COLETOR_DADOS,d,t);
			//pkg->dados_count = t + SSE_COLETOR_SNID_TOTAL + SSE_SIZE_IDCOMANDO;
		case SSE_CCD_COLETOR:
			SSE_EscreverBytes(pkg,SSE_CCD_COLETOR_DADOS,d,t);
			pkg->dados_count = t + SSE_CCD_COLETOR_SNID_TOTAL + SSE_SIZE_IDCOMANDO;
			break;
	}
	//pkg->dados_count = t;
}

/*
 *	Função		:	void SSE_EscreverRaw(SSE_Pacote* pkg, uint8_t* d, uint8_t t);
 *	Descrição	:	Sobrescreve o pacote passado com tudo que está em *d
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *					d	- ponteiro com os bytes de interesse
 *					t	- tamanho do conjunto de bytes passados. Deve ser
 *						  menor que o valor especificado em SSE_TOTAL_SIZE
 */
void SSE_EscreverRaw(SSE_Pacote* pkg, uint8_t* d, uint8_t t)
{
	t = (uint8_t)(t < SSE_SIZE_TOTAL ? t : SSE_SIZE_TOTAL);

	SSE_EscreverBytes(pkg,0,d,t);

	pkg->pacote_size = t;
	pkg->dados_count = (uint8_t)(((int8_t)(t - SSE_SIZE_HEADER) < 0) ? 0 : (t - SSE_SIZE_HEADER));
}

/*
 *	Função		:	uint16_t SSE_CalcularCRC16(SSE_Pacote* pkg);
 *	Descrição	:	Calcula o CRC-16 do pacote passado retornando
 *					seu valor.
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *
 *	Retorno		:	(uint16_t) - valor crc-16 calculado
 */
uint16_t SSE_CalcularCRC16(SSE_Pacote* pkg)
{
	uint16_t crc;

	// o tamanho do pacote antes do cálculo do crc16
	pkg->pacote_size = (uint8_t)((SSE_SIZE_HEADER) + pkg->dados_count);
	pkg->raw[SSE_START_TAMANHO] = pkg->dados_count;

	crc=CRC_16(&pkg->raw[SSE_START_ESCOPO],pkg->pacote_size-SSE_SIZE_SYNC-SSE_SIZE_CRC16,0);
	//crc=CRC_16(&pkg->raw[SSE_START_ESCOPO],pkg->raw[SSE_START_TAMANHO]+SSE_SIZE_ESCOPO+SSE_SIZE_TAMANHO,0);

	pkg->raw[pkg->pacote_size-2] = (uint8_t)(crc >> 8);
	pkg->raw[pkg->pacote_size-1] = (uint8_t)(crc & 0x00ff);

	pkg->raw[SSE_START_SYNC] = SSE_SYNC_0;
	pkg->raw[SSE_START_SYNC+1] = SSE_SYNC_1;

	return crc;
}

/*
 *	Função		:	uint16_t SSE_ChecarCRC16(SSE_Pacote* pkg);
 *	Descrição	:	Checa se o pacote passado tem o CRC-16 válido
 *
 *	Parâmetros	:	pkg - ponteiro para o pacote
 *
 *	Retorno		:	(BOOL) - TRUE caso sucesso
 *							 FALSE caso falha
 */
bool SSE_ChecarCRC16(SSE_Pacote* pkg)
{
	uint16_t crc;

	crc=CRC_16(&pkg->raw[SSE_START_ESCOPO],pkg->pacote_size-SSE_SIZE_SYNC-SSE_SIZE_CRC16,0);

	crc = crc - (uint16_t)((pkg->raw[pkg->pacote_size-2] << 8) | (pkg->raw[pkg->pacote_size-1]));

	return (bool)((crc == 0) ? true : false);
}

void SSE_EscreverIdCmd(SSE_Pacote* pkg, uint8_t* id_cmd)
{
	memcpy(&pkg->raw[SSE_START_IDCOMANDO],id_cmd,4);
}
