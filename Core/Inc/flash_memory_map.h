/*
 * Memory_map.h
 *
 *  Created on: 7 de dez de 2017
 *      Author: Felipe Oikawa
 */

#ifndef MEMORY_MEMORY_MAP_H_
#define MEMORY_MEMORY_MAP_H_

#define MEMORY_W25Q16       1

#define MEMORY_TO_USE       MEMORY_W25Q16

#define MEMORY_BYTE_SIZE	1
#define MEMORY_4BYTE_SIZE	4
#define MEMORY_3BYTE_SIZE	3
#define MEMORY_2BYTE_SIZE	2

//W25Q16
    //64 mbit -> 8 mbyte
    //                  sector 4kbyte -> 2000 sectors
//W25Q64JV has 64 mbit = 8 mbyte each sector has 4 kbytes size then you have 2000 sector or 0x7d0 so the maximum address you can erase is 0x7d0

#if MEMORY_TO_USE == MEMORY_W25Q16
#define MEMORY_CHIP_SIZE		2095152
#define MEMORY_BLOCK_SIZE		65536
#define MEMORY_SECTOR_SIZE		4096

#define MEMORY_NUM_SECTORS      512
#define MEMORY_INIT_SECTOR		0
#define MEMORY_LAST_SECTOR		511
#endif

#define MEMORY_MED_ID_SIZE	6
#define MEMORY_TOPIC_SIZE	20
// Sector Size	---	Sector Count	---	Sector Range	/	Address Range		---		Notes
// 	4 kbyte				512					SA0				000000h-000FFFh				Sector Starting Address
//											SA1				001000h-001FFFh				Sector 1 Address
// 											...
//											SA10			00A000h-00AFFFh				Sector 10 Address
// 											...
//											SA100			064000h-064FFFh				Sector 100 Address
// 											...
//											SA256			100000h-100FFFh				Sector 256 Address
// 											...
//											SA511			1FF000h-1FFFFFh				Sector Ending Address

//#define CODE_LINE_SIZE 256
// 256 -> 16 lines per sector -> 256 lines per block

/**************************************************************//**
 *  shift de linhas
 *****************************************************************/
//#define MEMORY_LINE_SHIFT(ADDRESS,LINE)                                 (ADDRESS + LINE*CODE_LINE_SIZE)


/**************************************************************//**
 * CCD MEDIDORES (SECTOR - 0 to 3)
 *****************************************************************/

/**************************************************************//**
 * FW UPLOAD DATA (BLOCK [16,17,18]) (SECTORS - [256:304])
 *****************************************************************/
#if 0
#define MEMORY_FIRMWARE_UPLOAD_DATA_START                        	(0x00100000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B1                       	(0x00110000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_START_B2                       	(0x00120000)
#define MEMORY_FIRMWARE_UPLOAD_DATA_TOTAL_SIZE                   	(MEMORY_BLOCK_SIZE*3)
#define MEMORY_FIRMWARE_UPLOAD_CONTROL	                    	    (0x00130000)
#define MEMORY_FIRMWARE_UPLOAD_CONTROL_TOTAL_SIZE                  (MEMORY_BLOCK_SIZE)
#endif
#endif /* MEMORY_MEMORY_MAP_H_ */




