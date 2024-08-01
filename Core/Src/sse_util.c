/*
 * util.c
 *
 *  Created on: 13 de set de 2016
 *      Author: Marcelo
 */
#include "sse_util.h"
#include <string.h>

#ifdef USE_CRC16
uint16_t CRC_16(uint8_t *Buffer, uint16_t  Len, uint16_t crc_seed)
{
   unsigned int crc = crc_seed;
   unsigned char bufdat;
   int i;

   while(Len--)
      {
         bufdat = *Buffer++;
         crc = crc ^ bufdat;
        for (i = 0;i < 8; i++)
         {
           if ((crc & 0x0001) !=0)
           {
   // Expected poly should be bits reflected so 0xA001 used instead of 0x8005;
              crc = (crc >> 1) ^ 0xA001;
           }
          else
           {
              crc = crc >> 1;
           }
         }
      }
   //   Swapping final CRC nibbles
      //crc = (crc >> 8) | (crc << 8);
      return crc;
}
#endif

#ifdef USE_IntToHexU8
uint8_t IntToHexU8(uint32_t u)
{
	uint8_t val = 0;

	val = u % 10;
	u /= 10;
	val |= (u % 10) << 4;
	return val;
}
#endif

#ifdef USE_SignedIntToHexU8
uint8_t SignedIntToHexU8(int32_t u)
{
	uint8_t val = 0;
	if(u<0)
	{
		u=-u;
	}
	val = u % 10;
	u /= 10;
	val |= (u % 10) << 4;
	return val;
}
#endif

#ifdef USE_HexU8ToINT
uint32_t HexU8ToINT(uint8_t* b, uint8_t size)
{
	uint32_t ret=0;
	while (size--)
	{
	        ret = ret*100 + (*b>>4)*10 +(*b&0x0F);
	        ++b;
	}
	return ret;
}
#endif

#ifdef USE_HexU8ToASCII
void HexU8ToASCII(uint8_t hex, char* buffer)
{
	uint8_t aux;

	/* Parte alta de h */
	aux = hex >> 4;

	if(aux <= 9)
	{
		buffer[0] = (aux + 0x30);
	}
	else
	{
		buffer[0] = (aux + 0x37);
	}

	aux = hex & 0x0F;

	if(aux <= 9)
	{
		buffer[1] = (aux + 0x30);
	}
	else
	{
		buffer[1] = (aux + 0x37);
	}
}
#endif

#ifdef USE_HexU8ToString
void HexU8ToString(char* buffer,uint8_t* h, uint16_t _s)    //size of HEX , 1 -> 2 ASCII
{
	uint16_t _aux=0;
	uint16_t _aux2=0;
	for(_aux=0;_aux<_s;_aux++)
	{
		HexU8ToASCII(h[_aux],&buffer[_aux2]);
		_aux2+=2;
	}
}
#endif

#ifdef USE_IntToString
//tamanho de ASCII que deve ter
void IntToString(char* buffer,uint32_t _number, uint8_t _tamanho)
{
	uint8_t _aux=0;
	for(_aux=_tamanho;_aux>0;_aux--)
	{
		buffer[_aux-1]=(char)((_number%10)+48);
		_number/=10;
	}
	buffer[_tamanho]=0;
}
#endif

#ifdef USE_IntToASCII
void IntToASCII(char* buffer,uint16_t _number)
{
	buffer[1]=((_number%10)+'0');
	_number/=10;
	buffer[0]=((_number%10)+'0');
}
#endif

#ifdef USE_ASCII2ToHexU8
uint8_t ASCII2ToHexU8(char* ascii)
{
	uint8_t val = 0;

	if(ascii[0] >= 0x30 && ascii[0] <= 0x39)
	{
		val = ascii[0] - 0x30;
	}
	else if(ascii[0] >= 0x41 && ascii[0] <= 0x46)
	{
		val = ascii[0] - 0x37;
	}
	val = val << 4;

	if(ascii[1] >= 0x30 && ascii[1] <= 0x39)
	{
		val |= ascii[1] - 0x30;
	}
	else if(ascii[1] >= 0x41 && ascii[1] <= 0x46)
	{
		val |= ascii[1] - 0x37;
	}

	return val;
}
#endif

#ifdef USE_ASCII2ToIntU8
uint8_t ASCII2ToIntU8(char* ascii, uint8_t size)
{
	uint8_t i;
	uint8_t _myreturn=0;

	for(i=0;i<size;i++)
	{
		_myreturn = (_myreturn * 10) + ((ascii[i]) - '0');
	}
	return _myreturn;
}
#endif

#ifdef USE_ASCII2ToIntU16
uint16_t ASCII2ToIntU16(char* ascii, uint16_t size)
{
	uint8_t i;
	uint16_t _myreturn=0;

	for(i=0;i<size;i++)
	{
		_myreturn = (_myreturn * 10) + ((ascii[i]) - '0');
	}
	return _myreturn;
}
#endif

#ifdef USE_ASCII2ToIntU32
uint32_t ASCII2ToIntU32(char* ascii, uint16_t size)
{
	uint8_t i;
	uint32_t _myreturn=0;

	for(i=0;i<size;i++)
	{
		_myreturn = (_myreturn * 10) + ((ascii[i]) - '0');
	}
	return _myreturn;
}
#endif

#ifdef USE_StringToHex
void StringToHex (uint8_t* returnHex, char* ascii, uint16_t size)
{
	uint16_t it = 0;
	for (it=0;it<size/2;it++)
	{
		returnHex[it]=ASCII2ToHexU8((char*)&ascii[it*2]);
	}
}
#endif

#ifdef USE_createHexU8toDouble
float createHexU8toDouble(uint8_t* _buf)
{
	iee_float myfloat;

	myfloat.b[0]=_buf[3];
	myfloat.b[1]=_buf[2];
	myfloat.b[2]=_buf[1];
	myfloat.b[3]=_buf[0];
	return myfloat.f;
}
#endif

#ifdef USE_createHexU8toFloat
float createHexU8toFloat(uint8_t* _buf)
{
	float _temp=0;
	memcpy(&_temp,&_buf[0],4);
	return _temp;
}
#endif

#ifdef USE_fabs
float fabs(float _num)
{
	if(_num<0)
	{
		return -_num;
	}
	return _num;
}
#endif

#ifdef USE_reset
void reset(void)
{
    while(1);
}
#endif

#ifdef USE_checksum8
uint8_t SSE_checksum8(uint8_t *Buffer, uint16_t  Len)
{
	uint8_t _iter=0;
	uint8_t _chk8=0;

    for(_iter=0;_iter <Len; _iter++)
    {
    	_chk8+=Buffer[_iter];
    }
    return _chk8;
}
#endif

#ifdef USE_U8H
uint8_t U8H (uint16_t _param)
{
	return ((_param>>8)&0xFF);
}
#endif
#ifdef USE_U8L
uint8_t U8L (uint16_t _param)
{
	return (_param&0xFF);
}
#endif

#ifdef USE_COPEL_REM
void SSE_Hex32ToStringCat(char* _toBeCat, uint32_t _HexToCat)
{
	uint32_t _newHex = _HexToCat;
	uint8_t _newCountHex[8];
	char _tempASCIIbuf[20]={0};

	memcpy(&_newCountHex[4],&_newHex,4);
	_newCountHex[0]=_newCountHex[7];
	_newCountHex[1]=_newCountHex[6];
	_newCountHex[2]=_newCountHex[5];
	_newCountHex[3]=_newCountHex[4];
	HexU8ToString((char*)_tempASCIIbuf,(uint8_t*)&_newCountHex[0],4);
	_tempASCIIbuf[8]=0;
	strcat((char*)_toBeCat,(char*)_tempASCIIbuf);
}

#if 0
//#define COPEL_BASE_DATE_HEX		0x4E559080		// 01/09/2021 00h00m00
#define COPEL_BASE_DATE_HEX		0x4E601C80
#define COPEL_BASE_DATE_DAY		1
#define COPEL_BASE_DATE_MON		9
#define COPEL_BASE_DATE_YEAR	2021
#define COPEL_BASE_DATE_HOUR	0
#define COPEL_BASE_DATE_MIN		0
#define COPEL_BASE_DATE_SEC		0

#define COPEL_BASE_DATE_DAYS	243

#define SECS_IN_YEAR	31536000U
#define SECS_IN_DAY		86400U
#define SECS_IN_HOUR	3600U
#define SECS_IN_MIN		60U

const uint8_t DAYS_IN_MONTH[] = {31,28,31,30,31,30,31,31,30,31,30,31};

uint8_t _toConvDays;
uint32_t _SecsTotal;
uint32_t _SecsTotal1;
uint32_t _SecsTotal2;
uint32_t _SecsTotal3;

// COPEL USA SEGUNDOS DESDE 1980
// de 1970 a 1980 12CE97F0
// na real 315532800 ou 0x12CEA600
uint32_t SSE_DateToSecs32 (uint8_t dDia,uint8_t dMes,uint16_t dAno,uint8_t dHora,uint8_t dMin,uint8_t dSec)
{
    if (dAno == COPEL_BASE_DATE_YEAR)
    {
         if (dMes<COPEL_BASE_DATE_MON)
              return 0;
         else if (dMes>COPEL_BASE_DATE_MON)
              return 1;
         else if (dDia<COPEL_BASE_DATE_DAY)
              return 0;
         else if(dDia>COPEL_BASE_DATE_DAY)
         {
        	 _toConvDays=dDia-COPEL_BASE_DATE_DAY;
         }
         else
         {

         }
    }
    _SecsTotal=(uint32_t)COPEL_BASE_DATE_HEX+((uint32_t)_toConvDays*SECS_IN_DAY)+((uint32_t)dHora*SECS_IN_HOUR)+((uint32_t)dMin*SECS_IN_MIN)+((uint32_t)dSec);
    //_SecsTotal1=(uint32_t)COPEL_BASE_DATE_HEX+(_toConvDays*SECS_IN_DAY);
    //_SecsTotal2=(uint32_t)((uint32_t)dHora*SECS_IN_HOUR);
    //_SecsTotal3=(uint32_t)(dMin*SECS_IN_MIN)+(dSec);
    //_SecsTotal=_SecsTotal1+_SecsTotal2+_SecsTotal3;
	return _SecsTotal;
}
#endif
#endif
