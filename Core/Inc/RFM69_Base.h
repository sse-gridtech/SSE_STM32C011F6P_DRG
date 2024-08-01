/*
 * RFM69.c for Renesas RX111
 *	Based on Felix Rusu (2014), felix@lowpowerlab.com
 *  http://lowpowerlab.com/
 * 
 *	Created on: 04/03/2015
 *      Author: Marcelo Sugita
 */
// **********************************************************************************
// Driver definition for HopeRF RFM69W/RFM69HW/RFM69CW/RFM69HCW, Semtech SX1231/1231H
// **********************************************************************************
// Copyright Felix Rusu (2014), felix@lowpowerlab.com
// http://lowpowerlab.com/
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License as published by the Free Software
// Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General
// Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//
// Licence can be viewed at
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************
#ifndef RFM69_BASE_H
#define RFM69_BASE_H


#include "sse_defines.h"
#include "RFM69registers.h"
#include "custom_gpio.h"
#include "custom_spi.h"

#if 0
#include "r_cg_serial.h"
#include "r_cg_port.h"
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "r_cg_intc.h"
#include "r_cg_timer.h"
#endif


#define USE_MINIMUM_RADIO	1

#define RF69_MAX_DATA_LEN       61 	// to take advantage of the built in AES/CRC we want to limit the frame size to the internal FIFO size (66 bytes - 3 bytes overhead - 2 bytes crc)
#define RF69_MAX_PAYLOAD_LEN	58	//AES 64 bytes message + 1 lenght + 1 address
									//6 bytes a mais no header
#define CSMA_LIMIT              -70 // upper RX signal sensitivity threshold in dBm for carrier sense access
#define RF69_MODE_SLEEP         0 // XTAL OFF
#define RF69_MODE_STANDBY       1 // XTAL ON
#define RF69_MODE_SYNTH         2 // PLL ON
#define RF69_MODE_RX            3 // RX MODE
#define RF69_MODE_TX            4 // TX MODE

// available frequency bands
#define RF69_315MHZ            31U // non trivial values to avoid misconfiguration
#define RF69_433MHZ            43U
#define RF69_868MHZ            86U
#define RF69_915MHZ            91U

//time and constansts
#define null                  0
#define COURSE_TEMP_COEF    -90 // puts the temperature reading in the ballpark, user can fine tune the returned value
#define RF69_BROADCAST_ADDR 0xFFFF
#define RF69_CSMA_LIMIT_MS 150U //100U
#define RF69_TX_LIMIT_MS   600U
#define RF69_FSTEP  61.03515625 // == FXOSC / 2^19 = 32MHz / 2^19 (p13 in datasheet)

//FLAGS
#define RFM69_FLAGS_SEND_ACK			0x80
#define RFM69_FLAGS_RECEIVE_ACK			0x40
#define RFM69_FLAGS_SEND_SENDACK		0x80
#define RFM69_FLAGS_SEND_REQUESTACK		0x40
#define RFM69_FLAGS_RCV_ACKRECEIVED		0x80
#define RFM69_FLAGS_RCV_ACKREQUESTED	0x40

	extern volatile uint8_t RFM69_DATA[RF69_MAX_DATA_LEN]; // recv/xmit buf, including header & crc bytes
	extern volatile uint8_t RFM69_DATALEN;
	extern volatile uint16_t RFM69_FINALID;
	extern volatile uint16_t RFM69_SENDERID;
	extern volatile uint16_t RFM69_TARGETID; // should match _address
	extern volatile uint8_t RFM69_PAYLOADLEN;
	extern volatile int16_t RFM69_RSSI; // most accurate RSSI during reception (closest to the reception)
	extern volatile uint8_t _mode; // should be protected?
	extern volatile uint8_t RFM69_FLAGS;
	//extern uint16_t  RFM69_address;
	// =======	FLAGS PARA HOP	========
	extern volatile uint8_t RFM69_CHANNEL; // should be protected?
    // =======	FLAGS PARA INTERRUPCAO	========
	extern volatile uint8_t RFM69_received_flag; //indica pacote recebido
	extern volatile uint8_t RFM69_sent_flag;		// indica pacote enviado
	// ===protected================
#if TO_C_FILE == 1
	static uint16_t _address;
	static bool _promiscuous;
	static uint8_t _powerLevel;
	static bool _isRFM69HW;
	static uint16_t RF69_ACK_LIMIT_MS =400;
#endif
	// ============================

#define RFM_SPI_Master_Send_Receive     SPI1_TransmitReceive
#define RFM69_TIMER_getMillis           TIM_getMillis
#define RFM69_SET_SS                    SPI1_clear
#define RFM69_SET_RESET                 GPIO_PB4_RADIO_RST_Set//SPI0_RADIO_RESET_Set//GPIO_PA02_RADIO_RST_Set
#define RFM69_RESET_RESET               GPIO_PB4_RADIO_RST_Clear//SPI0_RADIO_RESET_Clear//GPIO_PA02_RADIO_RST_Clear

	bool RFM69_initialize(uint8_t freqBand, uint16_t nodeID, uint8_t networkID, uint8_t isHW, uint8_t promiscuous, uint8_t power);
	uint16_t RFM69_getAddress(void);
	void RFM69_setAddress(uint16_t addr);
	void RFM69_setNetwork(uint8_t networkID);
	bool RFM69_canSend(void);
	void RFM69_send(uint16_t nextAddress,uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t _flags);
	void RFM69_sendDelay(uint16_t nextAddress, uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t flags, uint16_t _delay);
	bool RFM69_sendWithRetry(uint16_t nextAddress,uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t retries, uint16_t retryWaitTime); // 40ms roundtrip req for 61byte packets
	bool RFM69_receiveDone(void);
	bool RFM69_receiveDone2(void);
	bool RFM69_ACKReceived(uint16_t fromNodeID);
	bool RFM69_ACKRequested(void);
	void RFM69_sendACK(const void* buffer, uint8_t bufferSize);
	uint32_t RFM69_getFrequency(void);
	void RFM69_setFrequency(uint32_t freqHz);
	void RFM69_encrypt(const char* key);
	int16_t RFM69_readRSSI(bool forceTrigger);
	void RFM69_promiscuous(bool onOff);
	void RFM69_setHighPower(bool onOFF); // has to be called after initialize() for RFM69HW
	void RFM69_setPowerLevel(uint8_t level); // reduce/increase transmit power level
	void RFM69_sleep(void);
	uint8_t RFM69_readTemperature(uint8_t calFactor); // get CMOS temperature (8bit)
	void RFM69_rcCalibration(void); // calibrate the internal RC oscillator for use in wide temperature variations - see datasheet section [4.3.5. RC Timer Accuracy]
	
	// allow hacking registers by making these public
	uint8_t RFM69_readReg(uint8_t addr);
	void RFM69_writeReg(uint8_t addr, uint8_t val);
	void RFM69_readAllRegs(void);
	uint8_t RFM69_checkConfigRegs(void);
	uint8_t RFM69_configRegError(void);
	void RFM69_readConfigRegs(void);
	uint8_t RFM69_getConfigReg(uint8_t _regg);
	void RFM69_autoCheckConfigRegs(void);

	void RFM69_ISR(void);
	void RFM69_sendFrame(uint16_t nextAddress, uint16_t toAddress, const uint8_t* buffer, uint8_t size, uint8_t flags);
	
	void RFM69_receiveBegin(void);
	void RFM69_setMode(uint8_t mode);
	void RFM69_setHighPowerRegs(bool onOff);
	void RFM69_select(void);
	void RFM69_unselect(void);
	
	void RFM69_BroadcastRxEnabled(bool en);
	void RFM69_sleep(void);

	//=== FHSS CHANNEL HOP
	void RFM69_setChannel(uint8_t channel);
	void RFM69_hop(void);

	bool RFM69_reInit(void);
	void RFM69_reset(void);
	void RFM69_setRadio(uint8_t radio_set);
	void RFM69_setACKtime(uint16_t _acktime);
	uint16_t RFM69_getACKtime(void);

	void RFM69_LED_CONTROL (uint8_t _status);

	//uint8_t RFM_SPI_Master_Send_Receive(uint8_t * const rf_tx_buf, uint16_t rf_tx_num, uint8_t * const rf_rx_buf, uint16_t rf_rx_num);

	//DRIVERS
	void RFM69_TIMER_Start(uint16_t _ms);
	void RFM69_TIMER_Stop(void);
	uint8_t RFM69_TIMER_Fired(void);
	uint32_t RFM69_TIMER_getMillis(void);

	void RFM69_SET_SS(void);
	void RFM69_RESET_SS(void);
	void RFM69_INIT_SS(void);

	//void RFM69_SET_RESET(void);
	//void RFM69_RESET_RESET(void);
	void RFM69_INIT_RESET(void);

	void RFM69_WAIT_SPI(void);
	void RFM69_CLEAR_SPI(void);

	void RFM69_DISABLE_ISR(void);
	void RFM69_ENABLE_ISR(void);

	void RFM69_LED_ON(void);
	void RFM69_LED_OFF(void);
	void RFM69_LED_TOGGLE(void);

#define RFM69_FREQ_TABLE_LENGTH 35
//static const uint8_t __attribute__ ((progmem)) FRF[FREQ_TABLE_LENGTH][3] =
static const uint8_t FRF[RFM69_FREQ_TABLE_LENGTH][3] =
{
		{0xE1, 0x98, 0x71},//0 902.35
		{0xE1, 0xB8, 0x9C},//903
		{0xE1, 0xD8, 0xAA},//903.5
		{0xE1, 0xF8, 0xD6},//904
		{0xE2, 0x18, 0xEB},//904.5
		{0xE2, 0x39, 0x00},//5 905
		{0xE2, 0x59, 0x16},//905.5
		{0xE2, 0x79, 0x41},//906
		{0xE2, 0x99, 0x56},//906.5
		{0xE2, 0xB9, 0x7A},//9 907
/*		{0xE2, 0xD9, 0x90},//907.5
		{0xE2, 0xF9, 0xAC},//908
		{0xE3, 0x19, 0xC9},//908.5
		{0xE3, 0x39, 0xE6},//909
		{0xE3, 0x5A, 0x0A},//909.5
		{0xE3, 0x7A, 0x2E},//910
		{0xE3, 0x9A, 0x3C},//910.5
		{0xE3, 0xBA, 0x60},//911
		{0xE3, 0xDA, 0x7C},//911.5
		{0xE3, 0xFA, 0x92},//912
		{0xE4, 0x1A, 0xB6},//912.5
		{0xE4, 0x3A, 0xD2},//913
		{0xE4, 0x5A, 0xE8},//913.5
		{0xE4, 0x7B, 0x0C},//914
		{0xE4, 0x9B, 0x28},//914.5
		{0xE4, 0xBB, 0x4D},//915
*/		{0xE4, 0xDB, 0x62},//10 915.5
		{0xE4, 0xFB, 0x77},//916
		{0xE5, 0x1B, 0xA3},//916.5
		{0xE5, 0x3B, 0xBF},//917
		{0xE5, 0x5B, 0xCD},//917.5
		{0xE5, 0x7B, 0xE2},//15 918
		{0xE5, 0x9C, 0x0E},//918.5
		{0xE5, 0xBC, 0x2B},//919
		{0xE5, 0xDC, 0x40},//919.5
		{0xE5, 0xFC, 0x64},//920
		{0xE6, 0x1C, 0x81},//20 920.5
		{0xE6, 0x3C, 0x9D},//921
		{0xE6, 0x5C, 0xB2},//921.5
		{0xE6, 0x7C, 0xC8},//922
		{0xE6, 0x9C, 0xEC},//922.5
		{0xE6, 0xBD, 0x01},//25 923
		{0xE6, 0xDD, 0x34},//923.5
		{0xE6, 0xFD, 0x42},//924
		{0xE7, 0x1D, 0x5F},//924.5
		{0xE7, 0x3D, 0x74},//925
		{0xE7, 0x5D, 0x98},//30 925.5
		{0xE7, 0x7D, 0xBC},//926
		{0xE7, 0x9D, 0xD9},//926.5
		{0xE7, 0xBD, 0xEE},//927
		{0xE7, 0xDE, 0x12} //34 927.5
};

static const uint8_t rfm69_parameters[1][9] =
{
#if 0
		{RF_BITRATEMSB_1200, RF_BITRATELSB_1200, RF_FDEVMSB_90000, RF_FDEVLSB_90000,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_2,
			RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_NODEBROADCAST,
			RF_PACKET2_RXRESTARTDELAY_2BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF,
			RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_2},
		{RF_BITRATEMSB_55555, RF_BITRATELSB_55555, RF_FDEVMSB_50000, RF_FDEVLSB_50000,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_2,
			RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_OFF,
			RF_PACKET2_RXRESTARTDELAY_2BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF,
			RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00,
			RF_RXBW_DCCFREQ_100 | RF_RXBW_100k0},
		{RF_BITRATEMSB_19200, RF_BITRATELSB_19200, 0x02, 0x75,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_3,
			RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_WHITENING | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_NODEBROADCAST,
			RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF,
			RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_3},
		/*{RF_BITRATEMSB_9600, RF_BITRATELSB_9600, RF_FDEVMSB_20000, RF_FDEVLSB_20000,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_3,
			RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_NODEBROADCAST,
			RF_PACKET2_RXRESTARTDELAY_2BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF,
			RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00},*/
		{RF_BITRATEMSB_9600, RF_BITRATELSB_9600, RF_FDEVMSB_20000, RF_FDEVLSB_20000,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_4,
			RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_NODEBROADCAST,
			RF_PACKET2_RXRESTARTDELAY_2BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF,
			RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20 | RF_RXBW_EXP_3},
		{RF_BITRATEMSB_4800, RF_BITRATELSB_4800, RF_FDEVMSB_10000, RF_FDEVLSB_10000,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_15k6,
			RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_WHITENING | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_OFF,
			RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF,
			RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_41k7},
		{RF_BITRATEMSB_9600, RF_BITRATELSB_9600, RF_FDEVMSB_20000, RF_FDEVLSB_20000,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_31k3,
			RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_WHITENING | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_NODEBROADCAST,
			RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF,
			RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20 | RF_RXBW_EXP_3}
#endif
		{RF_BITRATEMSB_9600,
			RF_BITRATELSB_9600,
			RF_FDEVMSB_20000,
			RF_FDEVLSB_20000,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_31k3,
			RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_WHITENING | RF_PACKET1_CRC_ON | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_NODEBROADCAST,
			RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF,
			RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00,
			RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_20 | RF_RXBW_EXP_3}
};

static const uint8_t RFM69_Registers_LookupTable [] =
{
		REG_DATAMODUL, //0
		REG_BITRATEMSB,//01
		REG_BITRATELSB,
		REG_FDEVMSB,
		REG_FDEVLSB,
		REG_FRFMSB,//05
		REG_FRFMID,
		REG_FRFLSB,
		//REG_AFCCTRL,
		//REG_LISTEN1,//09
		//REG_LISTEN2,//0A
		//REG_LISTEN3,
		//REG_PALEVEL,
		//REG_PARAMP,
		//REG_OCP,
		REG_RXBW,//0F
		REG_AFCBW,//10
		//REG_DIOMAPPING2,
		//REG_RSSITHRESH,
		//REG_RXTIMEOUT1,
		//REG_RXTIMEOUT2,
		REG_PREAMBLEMSB,//15
		REG_PREAMBLELSB,
		REG_SYNCCONFIG,
		REG_SYNCVALUE1,
		REG_SYNCVALUE2,//19
		//REG_SYNCVALUE3,//1A ---- 18 19 1A 14
		//REG_SYNCVALUE4,
		REG_PACKETCONFIG1,
		//REG_PAYLOADLENGTH,
		REG_NODEADRS,
		//REG_BROADCASTADRS,//1F
		//REG_AUTOMODES,//20
		//REG_FIFOTHRESH,
		//REG_PACKETCONFIG2,
		//REG_AESKEY1,
		//REG_TESTLNA,
		//REG_TESTDAGC
};
#endif
