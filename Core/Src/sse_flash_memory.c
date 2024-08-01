#include "sse_flash_memory.h"
#include "flash_memory.h"
#include "string.h"
#include "sse_util.h"

#define FLASH1_PARAM_ID     1
#define PARAM_ID_4  0x04
#define PARAM_ID_5  0x10
//#define FLASH2_ERASE_INIT   FLASH2_ERASE_ALWAYS
//#define FLASH2_ERASE_INIT   FLASH2_ERASE_ON_VERSION
//#define FLASH2_ERASE_INIT   FLASH2_ERASE_ON_EMPTY
#define FLASH1_ERASE_INIT   FLASH2_ERASE_ON_VERSION
#define FLASH2_ERASE_INIT   FLASH2_ERASE_ON_EMPTY
#define FLASH3_ERASE_INIT   FLASH2_ERASE_ON_EMPTY
#define FLASH4_ERASE_INIT   FLASH2_ERASE_ON_EMPTY
#define FLASH5_ERASE_INIT   FLASH2_ERASE_ON_EMPTY
#define ERASE_ON_DIFF_VERSION  1
//SSE_MDC				MDC = {0};
//SSE_Medidor			medidor= {0};
//const uint16_t FLASHX_MAX_SIZE = FLASH3_MAX_SIZE;
Flash_Page_1		flash1 ={0};
Flash_Page_2		flash2 ={0};
Flash_Page_3		flash3 ={0};
Flash_Page_4		flash4 ={0};

static void SSE_Flash2_defines(void)
{
    flash2.MDC_FWVERSION[0] = SSE_FW_VERSION_0;
    flash2.MDC_FWVERSION[1] = SSE_FW_VERSION_1;
    flash2.MDC_FWVERSION[2] = SSE_FW_VERSION_2;
    flash2.MDC_FWVERSION[3] = SSE_FW_VERSION_3;
    flash2.FU_REMOTE_UPLOAD_CONTROL_STATUS=0;
    flash2.MDC_grupos[0]=0x01;
	flash2.MDC_gruposFU[0]=0x01;
}

static void SSE_Flash1_defines(void)
{
	uint8_t buff[] = {0xE0, 0x00, 0x03, 0x02, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //uint8_t temp_data[8];
    //memcpy(&temp_data[4],&FW_VERSION,4);
    flash1.MDC_FWVERSION[0] = SSE_FW_VERSION_0;
    flash1.MDC_FWVERSION[1] = SSE_FW_VERSION_1;
    flash1.MDC_FWVERSION[2] = SSE_FW_VERSION_2;
    flash1.MDC_FWVERSION[3] = SSE_FW_VERSION_3;
    memcpy(&flash1.MDC_DTH[0],(uint8_t*)&DTHL,4);
	flash1.MDC_DTH[4]=DTHM;

#if    FLASH1_PARAM_ID == 0
	flash1.MDC_SSEID[0] = SSE_SNID0_VTC_ABS;
	flash1.MDC_SSEID[1] = 0x01;
	flash1.MDC_SSEID[2] = SSE_SNID2_DEFAULT;
	flash1.MDC_SSEID[3] = SSE_SNID3_DEFAULT;
	flash1.MDC_SSEID[4] = SSE_SNID4_DEFAULT;
	flash1.MDC_SSEID[5] = SSE_SNID5_DEFAULT;
    
    flash1.MDC_COPELID[0] = SSE_SNID0_DEFAULT;
	flash1.MDC_COPELID[1] = SSE_SNID1_DEFAULT;
	flash1.MDC_COPELID[2] = SSE_SNID2_DEFAULT;
	flash1.MDC_COPELID[3] = SSE_SNID3_DEFAULT;
	flash1.MDC_COPELID[4] = SSE_SNID4_DEFAULT;
	flash1.MDC_COPELID[5] = SSE_SNID5_DEFAULT;
    
    flash1.MDC_RADIOID = flash1.MDC_SSEID[4] << 8 | flash1.MDC_SSEID[5];
	//flash1.MDC_RADIONET = RFM69_DEFAULT_VTC_NETWORK_ID;
    flash1.MDC_RADIONET = RFM69_DEFAULT_FLASH1_RESET_PARAM;
    //flash1.MDC_RADIONET = RFM69_VTC_NETWORK_ID_CS6;
#else
    flash1.MDC_SSEID[0] = SSE_SNID0_MDC_NANO;
	flash1.MDC_SSEID[1] = SSE_SNID1_DEFAULT;
	flash1.MDC_SSEID[2] = SSE_SNID2_DEFAULT;
	flash1.MDC_SSEID[3] = SSE_SNID3_DEFAULT;
	flash1.MDC_SSEID[4] = PARAM_ID_4;
	flash1.MDC_SSEID[5] = PARAM_ID_5;
    
    flash1.MDC_COPELID[0] = SSE_SNID0_DEFAULT;
	flash1.MDC_COPELID[1] = SSE_SNID1_DEFAULT;
	flash1.MDC_COPELID[2] = SSE_SNID2_DEFAULT;
	flash1.MDC_COPELID[3] = SSE_SNID3_DEFAULT;
	flash1.MDC_COPELID[4] = PARAM_ID_4;
	flash1.MDC_COPELID[5] = PARAM_ID_5;

	flash1.MDC_DRGID = 0x0001;

	memcpy(&flash1.MDC_MASK,&buff,32);

    flash1.MDC_RADIOID = flash1.MDC_SSEID[4] << 8 | flash1.MDC_SSEID[5];
	//flash1.MDC_RADIONET = RFM69_DEFAULT_VTC_NETWORK_ID;
    flash1.MDC_RADIONET = RFM69_DEFAULT_FLASH1_RESET_PARAM;
#endif
    
    flash1.MDC_TEMPERATURE = 0;
}

static void SSE_Flash3_defines(void)
{
    flash3.MDC_FWVERSION[0] = SSE_FW_VERSION_0;
    flash3.MDC_FWVERSION[1] = SSE_FW_VERSION_1;
    flash3.MDC_FWVERSION[2] = SSE_FW_VERSION_2;
    flash3.MDC_FWVERSION[3] = SSE_FW_VERSION_3;
    flash3.SSE_Pin1_NormalStatus=1;
    flash3.SSE_Pin1_ON=1;
    flash3.LAST_STATE_ALARMADO_SENSOR1=0;
    flash3.old_ABS_medidoresMask=0;
    flash3.ABS_LIBERAR_PORTA=0;
    memset(flash3.ABS_MED_IDX_STATUS,0xFF,12*2);
    flash3.ABS_REARMAR_PORTAS=0;
    memset(flash3.ABS_medidoresMask,0x00,5);
}

static void SSE_Flash4_defines(void)
{
    flash4.MDC_FWVERSION[0] = SSE_FW_VERSION_0;
    flash4.MDC_FWVERSION[1] = SSE_FW_VERSION_1;
    flash4.MDC_FWVERSION[2] = SSE_FW_VERSION_2;
    flash4.MDC_FWVERSION[3] = SSE_FW_VERSION_3;
    flash4.FU_REMOTE_UPLOAD_CONTROL_STATUS=0;
    memset(flash4.FU_maskMemLinesReceived,0,ML_SIZE);
}

void SSE_reWriteFlash1(void)
{
	uint8_t _dataToWrite[256];
	uint8_t _bytestoWrite;

	//prepara dados
	memcpy(&_dataToWrite[0],(uint8_t*)&flash1,sizeof(Flash_Page_1));
	_bytestoWrite=sizeof(Flash_Page_1);

#if FLASH_CHECK_IF_EMPTY == 1
	//check se limpo
	if(!flashmem_check_empty(MEMORY_SSE_FLASH1,_bytestoWrite))
	{
        flashmem_erase(MEMORY_SSE_FLASH1,MEMORY_SECTOR_SIZE);
	}
#else
    flashmem_erase(MEMORY_SSE_FLASH1,MEMORY_SECTOR_SIZE);
#endif
    
	//escreve
	flashmem_write(MEMORY_SSE_FLASH1,_bytestoWrite,_dataToWrite);
}

void SSE_reWriteFlash2(void)
{
	uint8_t _dataToWrite[512]={0};
	uint16_t _bytestoWrite;

	//prepara dados
	memcpy(&_dataToWrite[0],(uint8_t*)&flash2,sizeof(Flash_Page_2));
	_bytestoWrite=sizeof(Flash_Page_2);

#if FLASH_CHECK_IF_EMPTY == 1
	//check se limpo
	if(!flashmem_check_empty(MEMORY_SSE_FLASH2,_bytestoWrite))
	{
        flashmem_erase(MEMORY_SSE_FLASH2,MEMORY_SECTOR_SIZE);
	}
#else
    flashmem_erase(MEMORY_SSE_FLASH2,MEMORY_SECTOR_SIZE);
#endif
	//escreve
	flashmem_write(MEMORY_SSE_FLASH2,_bytestoWrite,_dataToWrite);
}

void SSE_reWriteFlash3(void)
{
	uint8_t _dataToWrite[512]={0};
	uint16_t _bytestoWrite;

	//prepara dados
	memcpy(&_dataToWrite[0],(uint8_t*)&flash3,sizeof(Flash_Page_3));
	_bytestoWrite=sizeof(Flash_Page_3);

#if FLASH_CHECK_IF_EMPTY == 1
	//check se limpo
	if(!flashmem_check_empty(MEMORY_SSE_FLASH3,_bytestoWrite))
	{
        flashmem_erase(MEMORY_SSE_FLASH3,MEMORY_SECTOR_SIZE);
	}
#else
    flashmem_erase(MEMORY_SSE_FLASH3,MEMORY_SECTOR_SIZE);
#endif
    
	//escreve
	flashmem_write(MEMORY_SSE_FLASH3,_bytestoWrite,_dataToWrite);
}

void SSE_reWriteFlash4(void)
{
	uint8_t _dataToWrite[256];
	uint8_t _bytestoWrite;

	//prepara dados
	memcpy(&_dataToWrite[0],(uint8_t*)&flash4,sizeof(Flash_Page_4));
	_bytestoWrite=sizeof(Flash_Page_4);

#if FLASH_CHECK_IF_EMPTY == 1
	//check se limpo
	if(!flashmem_check_empty(MEMORY_SSE_FLASH4,_bytestoWrite))
	{
        flashmem_erase(MEMORY_SSE_FLASH4,MEMORY_SECTOR_SIZE);
	}
#else
    flashmem_erase(MEMORY_SSE_FLASH4,MEMORY_SECTOR_SIZE);
#endif
    
	//escreve
	flashmem_write(MEMORY_SSE_FLASH4,_bytestoWrite,_dataToWrite);
}

void SSE_readReWriteFlashData(uint32_t sector_towrite, uint16_t idx_towrite, uint8_t* data_towrite, uint16_t size_towrite)
{
	uint8_t _dataToWrite[FLASHX_MAX_SIZE];
	uint16_t _bytestoWrite;

	//prepara dados
    flashmem_read(sector_towrite,FLASHX_MAX_SIZE,&_dataToWrite[0]);
	memcpy(&_dataToWrite[idx_towrite],data_towrite,size_towrite);
	_bytestoWrite=FLASHX_MAX_SIZE;

#if FLASH_CHECK_IF_EMPTY == 1
	//check se limpo
	if(!flashmem_check_empty(sector_towrite,_bytestoWrite))
	{
        flashmem_erase(sector_towrite,MEMORY_SECTOR_SIZE);
	}
#else
    flashmem_erase(sector_towrite,MEMORY_SECTOR_SIZE);
#endif
    
	//escreve
	flashmem_write(sector_towrite,_bytestoWrite,_dataToWrite);
}

void SSE_writeFlashData(uint32_t sector_towrite, uint32_t address_towrite, uint8_t* data_towrite, uint16_t size_towrite)
{
    uint32_t relative_add_to_write = address_towrite - sector_towrite;
    
    if(flashmem_check_empty(address_towrite,size_towrite))
    {
        flashmem_write(address_towrite,size_towrite,data_towrite);
    }
    else
    {
        SSE_readReWriteFlashData(sector_towrite,(uint16_t)relative_add_to_write,data_towrite,size_towrite);
    }
}

void SSE_readFlashDataASCII(uint32_t address_toread, char* buffer_toreturn, uint16_t size_toread)
{
    if(flashmem_check_empty(address_toread,size_toread))
    {
        //memset((uint8_t*)buffer_toreturn,0,size_toread);
        buffer_toreturn[0]='\0';
    }
    else
    {
        flashmem_read(address_toread,size_toread,(uint8_t*)&buffer_toreturn[0]);
    }
}

void SSE_readFlash1(void)
{
	uint8_t     _tempReadData[256];
    uint32_t    _FLASH_FWVERSION=0;
    
	flashmem_read(MEMORY_SSE_FLASH1,256,&_tempReadData[0]);
    
    memcpy(&_FLASH_FWVERSION,&_tempReadData[0],4);
    
#if FLASH1_ERASE_INIT == FLASH2_ERASE_ON_EMPTY
    //if((_tempReadData[0]==0xFF)||(_tempReadData[1]==0xFF))
    if((_FLASH_FWVERSION==0xFFFFFFFF)||(_tempReadData[0]=='T'))
#elif FLASH1_ERASE_INIT == FLASH2_ERASE_ON_VERSION
    if((_tempReadData[0]!=SSE_FW_VERSION_0)||(_tempReadData[1]!=SSE_FW_VERSION_1)||(_tempReadData[2]!=SSE_FW_VERSION_2)||(_tempReadData[3]!=SSE_FW_VERSION_3))
#elif FLASH1_ERASE_INIT == FLASH2_ERASE_ALWAYS
    if(1)
#endif
    {
        SSE_reWriteFlash1();
    }
    else
    {
        //restaura dados
        memcpy((uint8_t*)&flash1,&_tempReadData[0],sizeof(flash1));
        if((_tempReadData[0]!=SSE_FW_VERSION_0)||(_tempReadData[1]!=SSE_FW_VERSION_1)||(_tempReadData[2]!=SSE_FW_VERSION_2)||(_tempReadData[3]!=SSE_FW_VERSION_3))
        {
            flash1.MDC_FWVERSION[0] = SSE_FW_VERSION_0;
            flash1.MDC_FWVERSION[1] = SSE_FW_VERSION_1;
            flash1.MDC_FWVERSION[2] = SSE_FW_VERSION_2;
            flash1.MDC_FWVERSION[3] = SSE_FW_VERSION_3;
            SSE_reWriteFlash1();
        }
    }
}

void SSE_readFlash2(void)
{
	uint8_t     _tempReadData[256];
    
	flashmem_read(MEMORY_SSE_FLASH2,256,&_tempReadData[0]);
    
#if FLASH2_ERASE_INIT == FLASH2_ERASE_ON_EMPTY
    if((_tempReadData[0]==0xFF)||(_tempReadData[1]==0xFF))
#elif FLASH2_ERASE_INIT == FLASH2_ERASE_ON_VERSION
    if((_tempReadData[0]!=SSE_FW_VERSION_0)||(_tempReadData[1]!=SSE_FW_VERSION_1)||(_tempReadData[2]!=SSE_FW_VERSION_2)||(_tempReadData[3]!=SSE_FW_VERSION_3))
#elif FLASH2_ERASE_INIT == FLASH2_ERASE_ALWAYS
    if(1)
#endif
    {
#if     ERASE_ON_DIFF_VERSION == 1
        SSE_Flash2_defines();
#endif
        SSE_reWriteFlash2();
    }
    else
    {
        //restaura dados
        memcpy((uint8_t*)&flash2,&_tempReadData[0],sizeof(flash2));
    }
}

void SSE_readFlash3(void)
{
	uint8_t     _tempReadData[256];
    
	flashmem_read(MEMORY_SSE_FLASH3,256,&_tempReadData[0]);
    
#if FLASH3_ERASE_INIT == FLASH2_ERASE_ON_EMPTY
    if((_tempReadData[0]==0xFF)||(_tempReadData[1]==0xFF))
#elif FLASH3_ERASE_INIT == FLASH2_ERASE_ON_VERSION
    if((_tempReadData[0]!=SSE_FW_VERSION_0)||(_tempReadData[1]!=SSE_FW_VERSION_1)||(_tempReadData[2]!=SSE_FW_VERSION_2)||(_tempReadData[3]!=SSE_FW_VERSION_3))
#elif FLASH3_ERASE_INIT == FLASH2_ERASE_ALWAYS
    if(1)
#endif
    {
#if     ERASE_ON_DIFF_VERSION == 1
        SSE_Flash3_defines();
#endif
        SSE_reWriteFlash3();
    }
    else
    {
        //restaura dados
        memcpy((uint8_t*)&flash3,&_tempReadData[0],sizeof(flash3));
        if(flash3.ABS_REARMAR_PORTAS!=1)
        {
            flash3.ABS_REARMAR_PORTAS=0;
        }
    }
}

void SSE_readFlash4(void)
{
	uint8_t     _tempReadData[256];
    
	flashmem_read(MEMORY_SSE_FLASH4,256,&_tempReadData[0]);
    
#if FLASH4_ERASE_INIT == FLASH2_ERASE_ON_EMPTY
    if((_tempReadData[0]==0xFF)||(_tempReadData[1]==0xFF))
#elif FLASH4_ERASE_INIT == FLASH2_ERASE_ON_VERSION
    if((_tempReadData[0]!=SSE_FW_VERSION_0)||(_tempReadData[1]!=SSE_FW_VERSION_1)||(_tempReadData[2]!=SSE_FW_VERSION_2)||(_tempReadData[3]!=SSE_FW_VERSION_3))
#elif FLASH4_ERASE_INIT == FLASH2_ERASE_ALWAYS
#endif
    {
        SSE_reWriteFlash4();
    }
    else
    {
        //restaura dados
        memcpy((uint8_t*)&flash4,&_tempReadData[0],sizeof(flash4));
    }
}

void SSE_Flash_Initialize ( void )
{
    SSE_Flash1_defines();
    SSE_Flash2_defines();
    SSE_Flash3_defines();
    SSE_Flash4_defines();
#if USE_FLASH == 1
    SSE_readFlash1();
    SSE_readFlash2();
    SSE_readFlash3();
    SSE_readFlash4();
#endif
}

