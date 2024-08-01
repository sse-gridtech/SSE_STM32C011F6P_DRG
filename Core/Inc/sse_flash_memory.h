#include "sse_defines.h"

#define USE_FLASH 0

#define MEMORY_SSE_FLASH1	                    	    ((uint32_t)0x00000000)
#define MEMORY_SSE_FLASH2	                    	    ((uint32_t)0x00001000)
#define MEMORY_SSE_FLASH3	                    	    ((uint32_t)0x00002000)
#define MEMORY_SSE_FLASH4	                    	    ((uint32_t)0x00003000)
#define MEMORY_SSE_FLASH5	                    	    ((uint32_t)0x00004000)
#define MEMORY_SSE_FLASH6	                    	    ((uint32_t)0x00005000)
#define MEMORY_SSE_FLASH7	                    	    ((uint32_t)0x00006000)
#define MEMORY_SSE_FLASH8	                    	    ((uint32_t)0x00007000)
#define MEMORY_SSE_FLASH9	                    	    ((uint32_t)0x00008000)

typedef struct{
    uint8_t     MDC_FWVERSION[4];
	uint8_t     MDC_SSEID[6];
	uint8_t     MDC_COPELID[6];
    uint8_t     MDC_DTH[5];
    uint8_t     MDC_MASK[32];
    uint16_t    MDC_RADIOID;
    uint16_t    MDC_DRGID;
    uint8_t     MDC_RADIONET;
    uint8_t     MDC_TEMPERATURE;
}Flash_Page_1;
extern Flash_Page_1		flash1;

#define FLASH1_ERASE_ON_EMPTY       1
#define FLASH1_ERASE_ON_VERSION     2

#define FLASH1_START            MEMORY_SSE_FLASH1
#define FLASH1_FWVERSION        FLASH1_START
#define FLASH1_FWVERSION_SIZE   4
#define FLASH1_SSEID            FLASH1_FWVERSION + FLASH1_FWVERSION_SIZE
#define FLASH1_SSEID_SIZE       6
#define FLASH1_COPELID          FLASH1_SSEID + FLASH1_SSEID_SIZE
#define FLASH1_COPELID_SIZE     6
#define FLASH1_DTH              FLASH1_COPELID + FLASH1_COPELID_SIZE
#define FLASH1_DTH_SIZE         5
#define FLASH1_RADIOID          FLASH1_DTH + FLASH1_DTH_SIZE
#define FLASH1_RADIOID_SIZE     2
#define FLASH1_DRGID            FLASH1_RADIOID + FLASH1_RADIOID_SIZE
#define FLASH1_DRGID_SIZE       2
#define FLASH1_MASK             FLASH1_DRGID + FLASH1_DRGID_SIZE
#define FLASH1_MASK_SIZE        32
#define FLASH1_NET              FLASH1_MASK + FLASH1_MASK_SIZE
#define FLASH1_NET_SIZE         1
#define FLASH1_TEMP             FLASH1_NET + FLASH1_NET_SIZE
#define FLASH1_TEMP_SIZE        1
#define FLASH1_HW_MODEL         FLASH1_TEMP + FLASH1_TEMP_SIZE
#define FLASH1_HW_MODEL_SIZE    20
#define FLASH1_HW_SERIAL        FLASH1_HW_MODEL + FLASH1_HW_MODEL_SIZE
#define FLASH1_HW_SERIAL_SIZE   10
#define FLASH1_MAX_SIZE         FLASH1_HW_SERIAL + FLASH1_HW_SERIAL_SIZE

#define NUM_GRUPOS_MDC  5
typedef struct{
    uint8_t     MDC_FWVERSION[4];
	uint8_t     FU_REMOTE_UPLOAD_CONTROL_STATUS;
    uint8_t     MDC_grupos[NUM_GRUPOS_MDC];
    uint8_t     MDC_gruposFU[NUM_GRUPOS_MDC];
}Flash_Page_2;
extern Flash_Page_2		flash2;

#define FLASH2_ERASE_ON_EMPTY       1
#define FLASH2_ERASE_ON_VERSION     2
#define FLASH2_ERASE_ALWAYS         3

#define FLASH2_START                    MEMORY_SSE_FLASH2
#define FLASH2_FWVERSION                FLASH2_START
#define FLASH2_FWVERSION_SIZE           4
#define FLASH2_FU_CONTROL               FLASH2_FWVERSION + FLASH2_FWVERSION_SIZE
#define FLASH2_FU_CONTROL_SIZE          1
#define FLASH2_MAX_SIZE                 FLASH2_FU_CONTROL + FLASH2_FU_CONTROL_SIZE

typedef struct{
    uint8_t     MDC_FWVERSION[4];
    uint8_t     SSE_Pin1_ON;
    uint8_t     SSE_Pin1_NormalStatus;
    uint8_t     LAST_STATE_ALARMADO_SENSOR1;
    uint32_t    old_ABS_medidoresMask;
    uint8_t     ABS_LIBERAR_PORTA;
    uint8_t     ABS_MED_IDX_STATUS[12][2];
    uint8_t     ABS_REARMAR_PORTAS;
    uint8_t     ABS_medidoresMask[5];
}Flash_Page_3;
extern Flash_Page_3		flash3;

#define ML_SIZE                 240
#define ML_SIZE_PAGE_SIZE		33
typedef struct{
    uint8_t     MDC_FWVERSION[4];
	uint8_t     FU_REMOTE_UPLOAD_CONTROL_STATUS;
    uint8_t     FU_maskMemLinesReceived[ML_SIZE];
}Flash_Page_4;
extern Flash_Page_4		flash4;

#define FLASH4_START                    MEMORY_SSE_FLASH4
#define FLASH4_FWVERSION                FLASH4_START
#define FLASH4_FWVERSION_SIZE           4
#define FLASH4_FU_CONTROL               FLASH4_FWVERSION + FLASH4_FWVERSION_SIZE
#define FLASH4_FU_CONTROL_SIZE          1
#define FLASH4_FU_ML                    FLASH4_FU_CONTROL + FLASH4_FU_CONTROL_SIZE
#define FLASH4_FU_ML_SIZE               ML_SIZE
#define FLASH4_MAX_SIZE                 FLASH4_FU_ML + FLASH4_FU_ML_SIZE

#define FLASHX_MAX_SIZE     FLASH4_MAX_SIZE

#define FLASH_2BYTE_SIZE                            2U
//#define FLASH_MESH_TABLE_START                      (3072U)
#define FLASH_MESH_CONFIG_START                     MEMORY_SSE_FLASH6
#define FLASH_MESH_CONFIG_SIZE                      32
#define FLASH_MESH_TABLE_START                      FLASH_MESH_CONFIG_START + FLASH_MESH_CONFIG_SIZE
#define FLASH_MESH_TABLE_PARAMETER(N,SELECTION)     (FLASH_MESH_TABLE_START + N*FLASH_MESH_PARAMETER_SIZE + SELECTION)
#define FLASH_MESH_PARAMETER_FINAL_ID               (0U)
#define FLASH_MESH_PARAMETER_NEXT_HOP               (FLASH_MESH_PARAMETER_FINAL_ID + FLASH_2BYTE_SIZE)
#define FLASH_MESH_PARAMETER_GATEWAY_ID             (FLASH_MESH_PARAMETER_NEXT_HOP + FLASH_2BYTE_SIZE)
#define FLASH_MESH_PARAMETER_SIZE                   (FLASH_MESH_PARAMETER_GATEWAY_ID + FLASH_2BYTE_SIZE)
//#define FLASH_MESH_TABLE_SIZE                       (2048U)
#define FLASH_MESH_TABLE_SIZE                       ((100U)*(6U))//(1536U)
#define FLASH_MESH_FULL_SIZE                       FLASH_MESH_TABLE_SIZE + FLASH_MESH_CONFIG_SIZE

#define FLASH_MESHROW_SIZE                           6U

void SSE_writeFlashData(uint32_t sector_towrite, uint32_t address_towrite, uint8_t* data_towrite, uint16_t size_towrite);
void SSE_readFlashDataASCII(uint32_t address_toread, char* buffer_toreturn, uint16_t size_toread);
void SSE_Flash_Initialize (void );
void SSE_reWriteFlash1(void);
void SSE_reWriteFlash2(void);
void SSE_reWriteFlash3(void);
void SSE_reWriteFlash4(void);

void SSE_readFlash1(void);
