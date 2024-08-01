/*************************************************************
filename: ETC_3_11.c

Modified by: Marcelo Sugita
description: Recebe os dados via serial padrao ETC311 e coloca em um pacote
version: v1.0a (02/04/2015)

Created on: 07/03/2014
filename:ETC_3_11.c
Author: THIAGO
*************************************************************/
#include "sse_meters.h"

#include "custom_timer.h"
#include "custom_uart.h"
#include "sse_util.h"
//#include "custom_usart"
//#include "peripheral/port/plib_port.h"
//#include "peripheral/wdt/plib_wdt.h"

#if MEDIDOR_TIPO == MEDIDOR_TIPO_1_PIMA
/* buffer FIFO */
PacoteETC_3_11		  		pima_pacote;

extern volatile uint8_t autoBaudFlag;

#define PIMA_available      SERIAL0_available
#define PIMA_read           SERIAL0_read

uint8_t ReadETC_311(void)
{
	uint8_t _inChar=0;
	uint8_t _inChar2=0;
	uint32_t _myMill;

	while(PIMA_available()>10)
	{
		//WDT_Clear();
		PIMA_read(&_inChar,1);
		if(_inChar==SYNC_BYTE_0)
		{
			PIMA_read(&_inChar2,1);
			if(_inChar2==SYNC_BYTE_1)
			{
				autoBaudFlag=1;
				pima_pacote.sync0 = SYNC_BYTE_0;
				pima_pacote.sync1 = SYNC_BYTE_1;

				if(PIMA_available()<6)
				{
					_myMill=TIM_getMillis();
					while((TIM_getMillis()-_myMill<201)&&(PIMA_available()<6));
				}

				if(PIMA_available()>5)
				{
					PIMA_read(&pima_pacote.raw[2],6);

					if(PIMA_available()<pima_pacote.raw[TAMANHO_START]+2)
					{
						_myMill=TIM_getMillis();
						while((TIM_getMillis()-_myMill<201)&&(PIMA_available()<pima_pacote.raw[TAMANHO_START]+2));
					}
					if(PIMA_available()>pima_pacote.raw[TAMANHO_START]+1)
					{
						PIMA_read(&pima_pacote.raw[ESCOPO_START],pima_pacote.raw[TAMANHO_START]+2);
						if(PIMA_checkCRC()==0)
						{
							pima_pacote.t = TIM_getSecs();
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}
uint16_t PIMA_checkCRC(void)
{
	uint16_t _crc;
	_crc=CRC_16((uint8_t*)&pima_pacote.id,pima_pacote.raw[TAMANHO_START]+5+1+2,0);
	return _crc;
}
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR || MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT || MEDIDOR_TIPO == MEDIDOR_TIPO_4_FAKE_EXT)
#define NBR_SERIAL_clear        SERIAL0_clear
#define NBR_SERIAL_send         SERIAL0_send
#define NBR_SERIAL_receiveUntil SERIAL0_receiveUntil
#define NBR_SERIAL_available	SERIAL0_available
#define NBR_SERIAL_read         SERIAL0_read


uint8_t	nbr_n;	/* indica pacote pronto para ser processado */

NBR14522_Rx  nbr_received;		/* ponteiro para buffer para ser lido */
NBR14522_Tx	nbr_transmit;
uint8_t _nbrstate;
uint8_t _nbr_error;

void NBR14522_Init(uint8_t *mdcID, uint8_t *_nbrMedID)
{
	uint16_t it=0;
	uint8_t _idx=0;

#if (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR)
	_idx=0;
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT)
	_idx=5;
#endif
    nbr_transmit.raw[0]=0x99;
    nbr_transmit.raw[1]=_nbrMedID[0];
    nbr_transmit.raw[2]=_nbrMedID[1];
    nbr_transmit.raw[3]=_nbrMedID[2];
    nbr_transmit.raw[4]=_nbrMedID[3];
    for(it=0;it<66;it++)
    {
        nbr_transmit.raw[_idx+it]=0x00;
    }
    nbr_transmit.raw[_idx+1]=mdcID[0];
    nbr_transmit.raw[_idx+2]=mdcID[1];
    nbr_transmit.raw[_idx+3]=mdcID[2];
}

#if 0
uint8_t NBR14522_TX(uint8_t command, uint8_t idx, uint8_t *_pay, uint8_t _psize)
{
	uint8_t _sendPsize =1;

	nbr_transmit.raw[idx+0]=command;
	if((command!=0x06)&&(command!=0x15))
	{
		nbr_transmit._lastCommand=command;
		memset(&nbr_transmit.raw[idx+4],0,60);
		if(_psize)
		{
			memcpy(&nbr_transmit.raw[idx+4],_pay,_psize);
		}
		NBR14522_CRC16(64);
		_sendPsize=66;
	}
	else
	{
		if(nbr_transmit._reply==0)
		{
			nbr_transmit.raw[idx+0]=0x15; //NAK
		}
	}
	nbr_transmit._reply=0;
	NBR_SERIAL_clear();
	NBR_SERIAL_send(&nbr_transmit.raw[0],idx+_sendPsize);
	return 1;
}
#endif

uint8_t NBR14522_BuildPacketSend(uint8_t command, uint8_t idx, uint8_t* datas, uint8_t datas_size)
{
	uint8_t _sendPsize =1;

	nbr_transmit.raw[idx+0]=command;
	if((command!=0x06)&&(command!=0x15))
	{
		nbr_transmit._lastCommand=command;
		memset(&nbr_transmit.raw[idx+4],0,60);
		if(datas_size)
		{
			memcpy(&nbr_transmit.raw[idx+4],datas,datas_size);
		}
		NBR14522_CRC16(64);
		_sendPsize=66;
	}
	else
	{
		if(nbr_transmit._reply==0)
		{
			nbr_transmit.raw[idx+0]=0x15; //NAK
		}
	}
	nbr_transmit._reply=0;
	NBR_SERIAL_clear();
	NBR_SERIAL_send(&nbr_transmit.raw[0],idx+_sendPsize);
	return 1;
}

uint8_t NBR14522_BuildPacketSendEXT(uint8_t command, uint8_t idx, uint8_t subcommand, uint8_t type, uint8_t* datas, uint8_t datas_size)
{
    uint8_t _sendPsize =1;
    
	memset(&nbr_transmit.raw[idx+4],0,60);
	nbr_transmit.raw[idx+0]=command;
    nbr_transmit._lastCommand=command;
	nbr_transmit.raw[idx+4]=subcommand;
	nbr_transmit.raw[idx+5]=type;
	if(datas_size)
	{
		memcpy(&nbr_transmit.raw[idx+6],datas,datas_size);
	}
	NBR14522_CRC16(64);
    _sendPsize=66;
    
    nbr_transmit._reply=0;
	NBR_SERIAL_clear();
	NBR_SERIAL_send(&nbr_transmit.raw[0],idx+_sendPsize);
	return 1;
}

uint16_t NBR14522_CRC16(uint16_t size)
{
	uint16_t crc=0;

#if (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR)
	crc=CRC_16((uint8_t*)&nbr_transmit.raw[0],size,0);
	nbr_transmit.raw[64]=(crc>>0)&0xFF;
	nbr_transmit.raw[65]=(crc>>8)&0xFF;
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT)
	crc=CRC_16((uint8_t*)&nbr_transmit.raw[5+0],size,0);
	nbr_transmit.raw[5+64]=(crc>>0)&0xFF;
	nbr_transmit.raw[5+65]=(crc>>8)&0xFF;
#endif
	return crc;
}

uint16_t NBR14522_Check_CRC16(uint8_t med, uint16_t size)
{
	uint16_t __crc;

	__crc=CRC_16((uint8_t*)&nbr_received.raw[0],size,0);

	return __crc;
}

#if 0
uint16_t NBR_RecebeOptica(uint8_t* byte, uint16_t n)
{
	if(n == 1) //checa apenas um byte ate encontrar os 2 bytes de sincronismo
	{
#if (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR)
		if((byte[0] != 0x05)&&(byte[0] == nbr_transmit.raw[0]))
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT)
        if((byte[0] != 0x05)&&((byte[0] == nbr_transmit.raw[5])||(byte[0] == 0x40)))
#endif
		{
			nbr_received.raw[0]=byte[0];
			return 257;	//retorna para receber o pacote inteiro
		}
		return 1;
	}
	else
	{
		memcpy(&nbr_received.raw[1],&byte[0],257);
		if(NBR14522_Check_CRC16(0, 258)==0)
		{
			nbr_n=1;
		}
		return 1;
	}
	return 1;
}
#endif

uint8_t NBR14522_read(uint8_t command, uint8_t idx, uint8_t *_pay, uint8_t _psize, uint8_t _ack)
{
    uint8_t _times=0;
    //GPIO_PA14_Set();    //MAX232
    _nbr_error++;
    //LED_01_Clear();
    //UART0_232_EN_Set();
    //MDC._medStatus=0;
	for (_times=0; _times<2; _times++)
    {
        if(!nbr_n)
        {
            NBR14522_BuildPacketSend(command,idx,_pay,_psize);
            if(NBR_SERIAL_receiveUntil(258,1200))
            {
                while(NBR_SERIAL_available())
                {
                    if(NBR14522_ProcessSerial())
                    {
                        break;
                    }
                }
            }
        }
    }
    if(_ack)
    {
        if(nbr_n)
        {
            NBR14522_BuildPacketSend(0x06,idx,NULL,0);
        }
    }
	return nbr_n;
}

uint8_t NBR14522_readEXT(uint8_t command, uint8_t idx, uint8_t subcommand, uint8_t type, uint8_t* _pay, uint8_t _psize, uint8_t _ack)
{
	uint8_t _times=0;
    //GPIO_PA14_Set();    //MAX232
    _nbr_error++;
    //LED_01_Clear();
    //UART0_232_EN_Set();
    //MDC._medStatus=0;
    if(subcommand == 0x06 || subcommand == 0x03)
    {
        nbr_n = 0;
    }
	for (_times=0; _times<2; _times++)
    {
        if(!nbr_n)
        {
            NBR14522_BuildPacketSendEXT(command,idx,subcommand,type,_pay,_psize);
            if(NBR_SERIAL_receiveUntil(258,1200))
            {
                while(NBR_SERIAL_available())
                {
                    if(NBR14522_ProcessSerial())
                    {
                        break;
                    }
                }
            }
        }
    }
    if(_ack)
    {
        if(nbr_n)
        {
            NBR14522_BuildPacketSend(0x06,idx,NULL,0);
        }
    }
	return nbr_n;
}

uint8_t NBR14522_readBypass(uint8_t *_pkt, uint8_t _psize)
{
	uint32_t _timerTO;

	memcpy(&nbr_transmit.raw[0],_pkt,_psize);
	nbr_transmit._reply=0;
	NBR_SERIAL_clear();
	NBR_SERIAL_send(&nbr_transmit.raw[0],_psize);
	//NBR14522_TX(command,idx,_pay,_psize);

	if(NBR_SERIAL_receiveUntil(258,1200))
	{
		_timerTO=TIM_getMillis();
		while((NBR_SERIAL_available())&&(TIM_getMillis()-_timerTO<1000))
		{
			//WDT_Clear();
			if(NBR14522_ProcessSerial())
			{
				break;
			}
		}
	}
	NBR_SERIAL_clear();
	return nbr_n;
}

uint8_t NBR14522_ProcessSerial(void)
{
	switch(_nbrstate)
	{
		case 0:
			NBR_SERIAL_read(&nbr_received.raw[0],1);
#if (MEDIDOR_TIPO == MEDIDOR_TIPO_2_NBR)
			if((nbr_received.raw[0] != 0x05)&&(nbr_received.raw[0] != 0x06)&&((nbr_received.raw[0] == nbr_transmit.raw[0])||(nbr_received.raw[0] == 0x40)||(nbr_received.raw[0] == nbr_transmit._lastCommand)))
#elif (MEDIDOR_TIPO == MEDIDOR_TIPO_3_NBR_EXT)
			//if((nbr_received.raw[0] != 0x05)&&((nbr_received.raw[0] == nbr_transmit.raw[5])||(nbr_received.raw[5] == 0x40)||(nbr_transmit.raw[5] == 0x06)))
            if((nbr_received.raw[0] != 0x05)&&(nbr_received.raw[0] != 0x06)&&((nbr_received.raw[0] == nbr_transmit.raw[5])||(nbr_received.raw[0] == 0x40)||(nbr_received.raw[0] == nbr_transmit._lastCommand)))
#endif
			{
				_nbrstate=1;
			}
			break;
		case 1:
			if(NBR_SERIAL_receiveUntil(257,1200))
			{
				NBR_SERIAL_read((uint8_t*)&nbr_received.raw[1],257);
				if(NBR14522_Check_CRC16(0,258)==0)
				{
					nbr_transmit._reply=1;
					nbr_n=1;
                    _nbr_error=0;
				}
				else
				{
					NBR_SERIAL_clear();
				}
				_nbrstate=0;
			}
			else
			{
                NBR_SERIAL_clear();
                _nbrstate=0;
                break;
			}
			break;
		default:
			_nbrstate=0;
			break;
	}
	return nbr_n;
}

uint8_t NBR14522_getErrors(void)
{
    return _nbr_error;
}
#endif
