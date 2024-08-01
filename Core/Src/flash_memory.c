#include "flash_memory.h"
#include <string.h>
//#include "definitions.h"
#include "custom_spi.h"

static uint32_t erase_addr=0;
static uint32_t erase_len=0;
static uint8_t *write_data = 0;
static uint32_t write_addr=0;
static uint32_t write_len=0;
static uint32_t write_size=0;
static uint8_t dump_buffer[10] = {0};

uint32_t MEMORY_LINE_SHIFT(uint32_t ADDRESS, uint16_t LINE)
{
	uint32_t _offset=0;
	_offset=(uint32_t)((uint32_t)LINE*(uint32_t)CODE_LINE_SIZE);

	return (ADDRESS + _offset);
}

void flashmem_select(void) {
	SPI1_select(0);
}

void flashmem_unselect(void) {
	SPI1_clear();
}

void flashmem_select_send_receive(uint8_t* tx_buf, 	//TODO: ver se a sequencia de addr e data ta certo
		                           uint16_t tx_len,
		                           uint8_t *rx_buf,
                                    uint16_t rx_len) {
    flashmem_select(); // SPI Select
    SPI1_TransmitReceive(tx_buf,tx_len, rx_buf,rx_len);
	while(SPI1_IsBusy());;
	flashmem_unselect(); // SPI Unselect
}

void flashmem_single_byte_command(uint8_t cmd,
		                          size_t rx_len,
		                          uint8_t *rx_buf)
{
	uint8_t memory_scratch_buf [260];
    uint8_t buffer_len=0;
    
	memset(memory_scratch_buf, 0, sizeof(memory_scratch_buf));
	memory_scratch_buf[0] = cmd;
	buffer_len = 1 + rx_len;

	flashmem_select_send_receive(memory_scratch_buf,buffer_len , rx_buf,buffer_len);
}

void flashmem_command_with_address(uint8_t cmd, 	//TODO: ver se a sequencia de addr e data ta certo
		                           uint32_t addr,
								   size_t tx_len,
								   uint8_t *tx_buf,
		                           size_t rx_len,
		                           uint8_t *rx_buf)
{
	uint8_t memory_scratch_buf [260];
	uint16_t i = 0;
    
	memory_scratch_buf[i++] = cmd;
#if memory_info_address_bytes >= 3
    memory_scratch_buf[i++] = addr >> 16;
#endif
#if memory_info_address_bytes >= 2
    memory_scratch_buf[i++] = (addr >> 8) & 0xff;
#endif
#if memory_info_address_bytes >= 1
    memory_scratch_buf[i++] = addr & 0xff;
#endif
	memset(&memory_scratch_buf[i], 0, 256);
    
	if(tx_len > 0)
	{
        memcpy(&memory_scratch_buf[i],tx_buf,tx_len);
        i+=tx_len;
	}
	else if(rx_len > 0)
	{
		i += rx_len;
        rx_len = i;
	}

    flashmem_select_send_receive(memory_scratch_buf,i , memory_scratch_buf,i);

	if(rx_len > 4)
		memcpy(rx_buf,&memory_scratch_buf[4], (rx_len - 4));
}

void flashmem_read1(uint32_t addr,size_t len,uint8_t  *buffer)
{
    __NOP();
    __NOP();
	flashmem_command_with_address(CMD_READ_ARRAY_SLOW,addr,0,0,len, buffer);
}

void flashmem_read(uint32_t addr, size_t len, uint8_t  *buffer)
{
    uint16_t read_bytes=0;
    uint32_t new_addr=addr;
    
    while(len>memory_info_program_page_size)
    { 
        WDT_Clear();
        flashmem_read1(new_addr,memory_info_program_page_size, &buffer[read_bytes]);
        
        new_addr+=memory_info_program_page_size;
        read_bytes+=memory_info_program_page_size;
        len-=memory_info_program_page_size;
    }
    flashmem_read1(new_addr,len, &buffer[read_bytes]);
}

void flashmem_erase_commands(void)
{
    uint8_t erase_status_buf[3] = {0};
    
    flashmem_command_with_address(memory_info_erase_info_cmd_1, erase_addr,0, 0,0, 0);
    erase_addr += memory_info_erase_info_size_1;
    erase_len -= memory_info_erase_info_size_1;

	erase_status_buf[2] = memory_info_status_busy_level;

	while((erase_status_buf[2] & memory_info_status_busy_mask) == memory_info_status_busy_level)
	{
		flashmem_single_byte_command(memory_info_read_status_cmd,3,erase_status_buf);
	}
}
uint8_t flashmem_erase(uint32_t addr,uint32_t len)
{
    if (len % MEMORY_SECTOR_SIZE)
    {
        return 0;
    }
    
	erase_addr = addr;
	erase_len = len;

	while(erase_len)
	{
		WDT_Clear();
		flashmem_single_byte_command(CMD_WRITE_ENABLE,0, dump_buffer);
		flashmem_erase_commands();
	}

	return 1;
}

void flashmem_write_completion2(void)
{
    uint8_t write_status_buf[3] = {0};
	write_size = write_len;

	if ((write_addr & ~(memory_info_program_page_size-1)) != ((write_addr + write_size - 1) & ~(memory_info_program_page_size-1)))
    {
		write_size = memory_info_program_page_size - (write_addr & (memory_info_program_page_size-1));
    }

	flashmem_command_with_address(CMD_BYTE_PAGE_PROGRAM,write_addr,write_size, write_data,0,0);

	write_data += write_size;
	write_addr += write_size;
	write_len -= write_size;

	while((write_status_buf[0] & memory_info_status_busy_mask) == memory_info_status_busy_level)
	{// BUSY
		flashmem_single_byte_command(memory_info_write_status_cmd,3,write_status_buf);
	}
}
uint8_t flashmem_write(uint32_t addr,size_t len,uint8_t  *buffer)
{
	write_data = buffer;
	write_addr = addr;
	write_len = len;

	while(write_len)
	{
        __NOP();
		WDT_Clear();
		flashmem_single_byte_command(CMD_WRITE_ENABLE, 0 , dump_buffer);
		flashmem_write_completion2();
	}

	return 1;
}

#if FLASH_CHECK_IF_EMPTY == 1
uint8_t flashmem_check_empty(uint32_t Address, uint16_t length)
{
	uint16_t i;
	uint8_t check_if_empty=0;

	for (i = 0; i < length; ++i)
	{
		//R_WDT_Restart();
        WDT_Clear();
		flashmem_read(Address+i, 1, &check_if_empty);
		if(check_if_empty != 0xFF)
			return 0;
	}
	return true;
}
#endif
