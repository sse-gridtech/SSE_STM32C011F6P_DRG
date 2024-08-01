/****************************************************************************//**
 * @file memory.h
 * @brief Adesto Serial Flash Demo
 * @author Embedded Masters
 * @version 1.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Embedded Masters LLC, http://www.embeddedmasters.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Embedded Masters has no
 * obligation to support this Software. Embedded Masters is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Embedded Masters will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdio.h>
#include <stdint.h>
#include "flash_memory_map.h"
#include "boot_memory.h"

#define FLASH_CHECK_IF_EMPTY 0

#define WDT_Clear __NOP
#define ADESTO64K		1
#define SPANSION512K	2
#define W25Q16          3

//W25Q64
#define MEMORY_TYPE		W25Q16

#if MEMORY_TYPE	== W25Q16
//#define memory_name "S25FL116K"
#define memory_info_address_bytes       3
#define memory_info_read_slow           true
#define memory_info_erase_info_count    3
#define memory_info_status_busy_level   0x03
#define memory_info_status_busy_mask    0xF
#define memory_info_read_status_cmd     CMD_READ_STATUS
#define memory_info_write_status_cmd    CMD_WRITE_STATUS_REG_BYTE_1
#define memory_info_program_page_size   256

#define memory_info_erase_info_size_1         MEMORY_SECTOR_SIZE //4096
#define memory_info_erase_info_cmd_1          CMD_BLOCK_OR_SECTOR_ERASE
#define memory_info_erase_info_addrneeded_1   true
#if 0
#define memory_info_erase_info_size_2         65536
#define memory_info_erase_info_cmd_2          CMD_BLOCK_ERASE
#define memory_info_erase_info_addrneeded_2   true
#define memory_info_erase_info_size_3         2095152
#define memory_info_erase_info_cmd_3          CMD_CHIP_ERASE
#define memory_info_erase_info_addrneeded_3   false

#define flashmem_erase_page     memory_info_erase_info_size_1
#define flashmem_erase_block    memory_info_erase_info_size_2
#define flashmem_erase_chip     memory_info_erase_info_size_3
#endif
#endif

/* Adesto Command Hex Values */
#define CMD_READ_ARRAY                  0x0b
#define CMD_READ_ARRAY_SLOW             0x03  /* no dummy byte, but limited to lower clock rate */
#define CMD_READ_ARRAY_DUAL             0x3b  /* not used, as we don't have dual-SPI hardware */
#define CMD_PAGE_ERASE                  0x81
#define CMD_RM25C_PAGE_ERASE            0x42  /* RM26C256DS only */
#define CMD_BLOCK_OR_SECTOR_ERASE       0x20
#define CMD_BLOCK_ERASE          		0xDB
#define CMD_BLOCK_ERASE_LARGER          0xd8
#define CMD_CHIP_ERASE                  0x60
#define CMD_CHIP_ERASE2                 0xc7  /* not used, exactly the same as CMD_CHIP_ERASE */
#define CMD_BYTE_PAGE_PROGRAM           0x02
#define CMD_SEQUENTIAL_PROGRAM          0xad  /* not used, page program is more efficient */
#define CMD_SEQUENTIAL_PROGRAM2         0xaf  /* not used, page program is more efficient */
#define CMD_BYTE_PAGE_PROGRAM_DUAL      0xa2  /* not used, as we don't have dual-SPI hardware */
#define CMD_WRITE_ENABLE                0x06
#define CMD_WRITE_DISABLE               0x04
#define CMD_PROTECT_SECTOR              0x36
#define CMD_UNPROTECT_SECTOR            0x39
#define CMD_READ_SECTOR_PROTECTION      0x3c
#define CMD_PROGRAM_OTP                 0x9b
#define CMD_READ_OTP                    0x77
#define CMD_READ_STATUS                 0x05
#define CMD_DATAFLASH_READ_STATUS       0xd7
#define CMD_ACTIVE_STATUS_INTERRUPT     0x25
#define CMD_WRITE_STATUS_REG_BYTE_1     0x01
#define CMD_WRITE_STATUS_REG_BYTE_2     0x31
#define CMD_RESET                       0xf0
#define CMD_READ_ID                     0x90
#define CMD_READ_MANUFACTURER_ID		0x90
#define CMD_DEEP_POWER_DOWN             0xb9
#define CMD_RESUME_FROM_DEEP_POWER_DOWN 0xab
#define CMD_ULTRA_DEEP_POWER_DOWN       0x79

#define ARG_RESET                       0xd0  /* second byte required for reset command */

// commands only for DataFlash, e.g., AT45DB081E and AT45DB641E
#define CMD_DATAFLASH_BLOCK_ERASE       0x50  /* 8 pages, 2KB */
#define CMD_DATAFLASH_SECTOR_ERASE      0x7c

#define CMD_DATAFLASH_RMW_BUF1          0x58
#define CMD_DATAFLASH_RMW_BUF2          0x59
/***************************************************************************//**
 * @addtogroup Adesto_FlashDrivers
 * @{
 ******************************************************************************/

uint32_t MEMORY_LINE_SHIFT(uint32_t ADDRESS, uint16_t LINE);

void flashmem_select(void);

void flashmem_unselect(void);

void flashmem_select_send_receive(uint8_t* tx_buf, uint16_t tx_len,uint8_t *rx_buf,uint16_t rx_len);

void flashmem_single_byte_command(uint8_t cmd,size_t rx_len,uint8_t *rx_buf);

//void flashmem_multiple_byte_command(size_t tx_len,uint8_t * const tx_buf,size_t rx_len,uint8_t * const rx_buf);

void flashmem_command_with_address(uint8_t cmd, 	//TODO: ver se a sequencia de addr e data ta certo
		                           uint32_t addr,
								   size_t tx_len,
								   uint8_t *tx_buf,
		                           size_t rx_len,
		                           uint8_t *rx_buf);
void flashmem_read(uint32_t addr,size_t len, uint8_t  *buffer);

uint8_t flashmem_erase(uint32_t addr,uint32_t len);

//void memory_erase_completion1(void);

//void memory_erase_completion2(void);

//void flashmem_erase_commands(void);

uint8_t flashmem_write(uint32_t addr,size_t len,uint8_t  *buffer);

//void flashmem_write_completion1(void);

//void flashmem_write_completion2(void);

//void memory_read_id(size_t len, uint8_t *buffer);

//void memory_reset(void);

//bool memory_init(void);

uint8_t flashmem_check_empty(uint32_t Address, uint16_t length);
#endif /* MEMORY_H_ */
