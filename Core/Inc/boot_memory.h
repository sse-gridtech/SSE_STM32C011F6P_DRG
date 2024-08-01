#ifndef BOOTMEM_H
#define BOOTMEM_H

#define CODE_LINE_SIZE 256

#define MEMORY_FIRMWARE_UPLOAD_CONTROL	                    	    (0x00100000)
#define MEMORY_FIRMWARE_UPLOAD_CONTROL_TOTAL_SIZE                   (MEMORY_BLOCK_SIZE)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START                        	(0x00110000)            //256 linhas por pagina
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B1                       	(0x00120000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B2                       	(0x00130000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B3                       	(0x00140000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B4                       	(0x00150000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B5                       	(0x00160000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B6                       	(0x00170000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B7                       	(0x00180000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B8                       	(0x00190000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B9                       	(0x001A0000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B10                       (0x001B0000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_TOTAL_SIZE                   	(MEMORY_BLOCK_SIZE*6)

#endif
