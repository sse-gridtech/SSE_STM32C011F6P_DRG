/*
 * util.h
 *
 *  Created on: 13 de set de 2016
 *      Author: Marcelo
 */
//#include <stdlib.h>
//#include "device.h"
//#include <stddef.h>
#include <stdint.h>

#ifndef SSE_UTIL_H_
#define SSE_UTIL_H_

#ifndef NULL
#define NULL    ((void *)0)
#endif  /* !NULL */

typedef union
{
  uint8_t b[4];
  double f;
}iee_float;

typedef union
{
  uint8_t  b[4];
  uint32_t ui32;
}uint_abcd;

#define USE_CRC16
#define USE_IntToHexU8
//#define USE_SignedIntToHexU8
#define USE_HexU8ToINT
#define USE_HexU8ToASCII
#define USE_HexU8ToString
#define USE_IntToString
#define USE_IntToASCII
#define USE_ASCII2ToHexU8
#define USE_ASCII2ToIntU8
#define USE_ASCII2ToIntU16
#define USE_ASCII2ToIntU32
#define USE_StringToHex
#define USE_createHexU8toDouble
#define USE_createHexU8toFloat
//#define USE_fabs
//#define USE_reset
//#define USE_checksum8
//#define USE_U8H
//#define USE_U8L

#define USE_COPEL_REM

uint16_t CRC_16(uint8_t *Buffer, uint16_t  Len, uint16_t crc_seed);
uint8_t IntToHexU8(uint32_t u);
uint8_t SignedIntToHexU8(int32_t u);
uint32_t HexU8ToINT(uint8_t* b, uint8_t size);
void HexU8ToASCII(uint8_t hex, char* buffer);
void HexU8ToString(char* buffer,uint8_t* h, uint16_t _s);
void IntToString(char* buffer,uint32_t _number, uint8_t _tamanho);
void IntToASCII(char* buffer,uint16_t _number);
uint8_t ASCII2ToHexU8(char* ascii);
uint8_t ASCII2ToIntU8(char* ascii, uint8_t size);
uint16_t ASCII2ToIntU16(char* ascii, uint16_t size);
uint32_t ASCII2ToIntU32(char* ascii, uint16_t size);
void StringToHex (uint8_t* returnHex, char* ascii, uint16_t size);
float createHexU8toDouble(uint8_t* _buf);
float createHexU8toFloat(uint8_t* _buf);
//float fabs(float _num);
void reset(void);
uint8_t SSE_checksum8(uint8_t *Buffer, uint16_t  Len);
uint8_t U8H (uint16_t _param);
uint8_t U8L (uint16_t _param);

void SSE_Hex32ToStringCat(char* _toBeCat, uint32_t _HexToCat);
//uint32_t SSE_DateToSecs32 (uint8_t dDia,uint8_t dMes,uint16_t dAno,uint8_t dHora,uint8_t dMin,uint8_t dSec);

#endif /* SSE_UTIL_H_ */
