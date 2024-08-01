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
#include "RFM69_Base.h"
#include <string.h>

#define SPI0_MINIMAL 1

volatile uint16_t RFM69_FINALID;
volatile uint16_t RFM69_SENDERID;
volatile uint16_t RFM69_TARGETID;     // should match _address
volatile int16_t RFM69_RSSI;          // most accurate RSSI during reception (closest to the reception)
volatile uint8_t RFM69_DATA[RF69_MAX_DATA_LEN];
volatile uint8_t _mode;        // current transceiver state
volatile uint8_t RFM69_DATALEN;
volatile uint8_t RFM69_PAYLOADLEN;
volatile uint8_t RFM69_inISR;
volatile uint8_t RFM69_CHANNEL; // should be protected?
volatile uint8_t RFM69_FLAGS;

// =======  TIMER ============
uint32_t _RFM_Timer_init;
uint32_t _RFM_Timer_TO;
// =======	FLAGS PARA INTERRUPCAO	========
volatile uint8_t RFM69_received_flag; //indica pacote recebido
volatile uint8_t RFM69_sent_flag;		// indica pacote enviado
// =======	BUFFER PARA SPI	========
uint8_t spi_out[RF69_MAX_DATA_LEN + 10];// = {0xAA,0xFF,0x00,0x0A,0x0F};
uint8_t spi_in[5];//[RF69_MAX_DATA_LEN + 10];
static uint8_t RFM69_LED_normallyON;

// ======= CONFIG REGISTERS ======
uint8_t _configRegsBack[sizeof(RFM69_Registers_LookupTable)];
uint8_t RFM69_regError=0;
#define RFM69_reConfig_ErrorTimes 2
// ======= BACKUP =======
uint8_t _netID;
uint16_t  RFM69_address;
uint8_t RFM69_led;
uint8_t __i;
// ======= FROM_H_FILE =======
	static uint16_t _address;
	static bool _promiscuous;
	static uint8_t _powerLevel;
	static bool _isRFM69HW;
	static uint16_t RF69_ACK_LIMIT_MS =400;
//
    
bool RFM69_initialize(uint8_t freqBand, uint16_t nodeID, uint8_t networkID, uint8_t isHW, uint8_t p, uint8_t power)
{
  uint8_t i=0;
  const uint8_t CONFIG[][2] =
	{
	  /* 0x01 */ { REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY },
	  /* 0x02 */ { REG_DATAMODUL, rfm69_parameters[0][7]}, // no shaping
	  /* 0x03 */ { REG_BITRATEMSB, rfm69_parameters[0][0]}, // default: 4.8 KBPS
	  /* 0x04 */ { REG_BITRATELSB, rfm69_parameters[0][1]},
	  /* 0x05 */ { REG_FDEVMSB, rfm69_parameters[0][2]}, // default: 5KHz, (FDEV + BitRate / 2 <= 500KHz)
	  /* 0x06 */ { REG_FDEVLSB, rfm69_parameters[0][3]},

	  /* 0x07 */ { REG_FRFMSB, (uint8_t) (freqBand==RF69_315MHZ ? RF_FRFMSB_315 : (freqBand==RF69_433MHZ ? RF_FRFMSB_433 : (freqBand==RF69_868MHZ ? RF_FRFMSB_868 : RF_FRFMSB_915))) },
	  /* 0x08 */ { REG_FRFMID, (uint8_t) (freqBand==RF69_315MHZ ? RF_FRFMID_315 : (freqBand==RF69_433MHZ ? RF_FRFMID_433 : (freqBand==RF69_868MHZ ? RF_FRFMID_868 : RF_FRFMID_915))) },
	  /* 0x09 */ { REG_FRFLSB, (uint8_t) (freqBand==RF69_315MHZ ? RF_FRFLSB_315 : (freqBand==RF69_433MHZ ? RF_FRFLSB_433 : (freqBand==RF69_868MHZ ? RF_FRFLSB_868 : RF_FRFLSB_915))) },

	  // looks like PA1 and PA2 are not implemented on RFM69W, hence the max output power is 13dBm
	  // +17dBm and +20dBm are possible on RFM69HW
	  // +13dBm formula: Pout = -18 + OutputPower (with PA0 or PA1**)
	  // +17dBm formula: Pout = -14 + OutputPower (with PA1 and PA2)**
	  // +20dBm formula: Pout = -11 + OutputPower (with PA1 and PA2)** and high power PA settings (section 3.3.7 in datasheet)

	  //{REG_PARAMP,0x0B},
	///* 0x13 */ { REG_OCP, RF_OCP_ON | RF_OCP_TRIM_95 }, // over current protection (default is 95mA)
	//{REG_LNA,0x88},

	  // RXBW defaults are { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_5} (RxBw: 10.4KHz)
	  /* 0x19 */ { REG_RXBW, rfm69_parameters[0][4]}, // (BitRate < 2 * RxBw)
	  /* 0x1A */ { REG_AFCBW, rfm69_parameters[0][8]}, // Use double the bandwidth during AFC as reception
	  //for BR-19200: /* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_3 },
	  /* 0x25 */ { REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01 }, // DIO0 is the only IRQ we're using
	  /* 0x26 */ { REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF }, // DIO5 ClkOut disable for power saving
	  /* 0x28 */ { REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN }, // writing to this bit ensures that the FIFO & status flags are reset
	  /* 0x29 */ { REG_RSSITHRESH, 224 }, // must be set to dBm = (-Sensitivity / 2), default is 0xE4 = 228 so -114dBm
	  /* 0x2D */ { REG_PREAMBLELSB, RF_PREAMBLESIZE_LSB_VALUE }, // default 3 preamble bytes 0xAAAAAA
	  /* 0x2E */ { REG_SYNCCONFIG, RF_SYNC_ON | RF_SYNC_FIFOFILL_AUTO | RF_SYNC_SIZE_2 | RF_SYNC_TOL_0 },
	  /* 0x2F */ { REG_SYNCVALUE1, 0x2D },      // attempt to make this compatible with sync1 byte of RFM12B lib
	  /* 0x30 */ { REG_SYNCVALUE2, networkID }, // NETWORK ID
	  /* 0x37 */ { REG_PACKETCONFIG1, rfm69_parameters[0][5] },
	  /* 0x38 */ { REG_PAYLOADLENGTH, 66 }, // in variable length mode: the max frame size, not used in TX
	  /* 0x39 */ { REG_NODEADRS, (uint8_t)(nodeID&0xFF) }, // turned off because we're not using address filtering
	  /* 0x3A */ { REG_BROADCASTADRS, (uint8_t)(RF69_BROADCAST_ADDR&0xFF) }, // turned off because we're not using address filtering
	  /* 0x3C */ { REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | RF_FIFOTHRESH_VALUE }, // TX on FIFO not empty
	  /* 0x3D */ //{ REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_2BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
	/* 0x3D */ { REG_PACKETCONFIG2, rfm69_parameters[0][6] }, // RXRESTARTDELAY must match //for BR-19200:
	/* 0x6F */ //{ REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA1 }, // run DAGC continuously in RX mode for Fading Margin Improvement, recommended default for AfcLowBetaOn=0
	/* 0x6F */ { REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA0 }, // run DAGC continuously in RX mode for Fading Margin Improvement, recommended default for AfcLowBetaOn=0
	  {255, 0}
    };

  RFM69_DISABLE_ISR();
  RFM69_INIT_SS();
  RFM69_CLEAR_SPI();

  RFM69_INIT_RESET();
  RFM69_SET_RESET();
  RFM69_TIMER_Start(10);
  while (!RFM69_TIMER_Fired());
  RFM69_RESET_RESET();
  RFM69_TIMER_Start(10);
  while (!RFM69_TIMER_Fired());

  for (i = 0; CONFIG[i][0] != 255; i++)
	  RFM69_writeReg(CONFIG[i][0], CONFIG[i][1]);

  /*
  for (__i = 0; CONFIG[__i][0] != 255; __i++)
  {
	  if(CONFIG[__i][0]!=REG_IRQFLAGS2)
	  {
		  if(CONFIG[__i][1]!=RFM69_readReg(CONFIG[__i][0]))
		  {
			  return false;
		  }
	  }
  }
   */

  _mode=RF69_MODE_STANDBY;

  // Encryption is persistent between resets and can trip you up during debugging.
  // Disable it during initialization so we always start from a known state.
  RFM69_encrypt(0);

  _promiscuous=p;
  _powerLevel=power;
  _isRFM69HW=isHW;

  RFM69_setPowerLevel(_powerLevel);
  RFM69_setHighPower(isHW); // called regardless if it's a RFM69W or RFM69HW
  RFM69_setHighPowerRegs(false);

  RFM69_inISR=0;
  RFM69_setAddress(nodeID);
  _netID= networkID;
  //RFM69_LED_normallyON=0;

  RFM69_readConfigRegs();
  return true;
}

// return the frequency (in Hz)
uint32_t RFM69_getFrequency(void)
{
  return RF69_FSTEP * (((uint32_t) RFM69_readReg(REG_FRFMSB) << 16) + ((uint16_t) RFM69_readReg(REG_FRFMID) << 8) + RFM69_readReg(REG_FRFLSB));
}

// set the frequency (in Hz)
void RFM69_setFrequency(uint32_t freqHz)
{
  uint8_t oldMode = _mode;
  if (oldMode == RF69_MODE_TX) {
	  RFM69_setMode(RF69_MODE_RX);
  }
  freqHz /= RF69_FSTEP; // divide down by FSTEP to get FRF
  RFM69_writeReg(REG_FRFMSB, freqHz >> 16);
  RFM69_writeReg(REG_FRFMID, freqHz >> 8);
  RFM69_writeReg(REG_FRFLSB, freqHz);
  if (oldMode == RF69_MODE_RX) {
	  RFM69_setMode(RF69_MODE_SYNTH);
  }
  RFM69_setMode(oldMode);
}

void RFM69_setMode(uint8_t newMode)
{
  if (newMode == _mode)
    return;

  switch (newMode) {
    case RF69_MODE_TX:
    	if (_isRFM69HW) RFM69_setHighPowerRegs(true);
    	RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_TRANSMITTER);
      break;
    case RF69_MODE_RX:
    	if (_isRFM69HW) RFM69_setHighPowerRegs(false);
    	RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_RECEIVER);
      break;
    case RF69_MODE_SYNTH:
    	RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_SYNTHESIZER);
      break;
    case RF69_MODE_STANDBY:
    	if (_isRFM69HW) RFM69_setHighPowerRegs(false);
    	RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_STANDBY);
      break;
    case RF69_MODE_SLEEP:
    	RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_SLEEP);
      break;
    default:
      return;
  }

  // we are using packet mode, so this check is not really needed
  // but waiting for mode ready is necessary when going from sleep because the FIFO may not be immediately available from previous mode
  while (_mode == RF69_MODE_SLEEP && (RFM69_readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00); // wait for ModeReady

  _mode = newMode;
}

void RFM69_sleep(void) {
	RFM69_setMode(RF69_MODE_SLEEP);
}

uint16_t RFM69_getAddress(void)
{
	return RFM69_address;
}

void RFM69_setAddress(uint16_t addr)
{
	if(addr==0x0000)
	{
		addr=0x000A;
	}
  _address = addr;
  RFM69_address = addr;
  RFM69_writeReg(REG_NODEADRS, (uint8_t)(_address&0xFF));
}

void RFM69_setNetwork(uint8_t networkID)
{
	RFM69_writeReg(REG_SYNCVALUE2, networkID);
	_netID= networkID;
}

// set output power: 0 = min, 31 = max
// this results in a "weaker" transmitted signal, and directly results in a lower RSSI at the receiver
void RFM69_setPowerLevel(uint8_t powerLevel)
{
  _powerLevel = powerLevel;
  RFM69_writeReg(REG_PALEVEL, (RFM69_readReg(REG_PALEVEL) & 0xE0) | (_powerLevel > 31 ? 31 : _powerLevel));
}

bool RFM69_canSend(void)
{
  if(_mode == RF69_MODE_STANDBY)
  {
	return true;
  }
  else if (_mode == RF69_MODE_RX && RFM69_PAYLOADLEN == 0 && (RFM69_readRSSI(0) < CSMA_LIMIT)) // if signal stronger than -100dBm is detected assume channel activity
  {
	  RFM69_setMode(RF69_MODE_STANDBY);
    return true;
  }
  return false;
}

void RFM69_send(uint16_t nextAddress, uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t flags)
{
  RFM69_TIMER_Start(RF69_CSMA_LIMIT_MS);//CMT0_SetTimeout(RF69_CSMA_LIMIT_MS);
  RFM69_writeReg(REG_PACKETCONFIG2, (RFM69_readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
  while (!RFM69_canSend() && !RFM69_TIMER_Fired()) RFM69_receiveDone();
  //while (!RFM69_TIMER_Fired());
  RFM69_TIMER_Stop();
  RFM69_sendFrame(nextAddress, toAddress, buffer, bufferSize, flags); //requestACK, false
  //RFM69_receiveBegin();
}

void RFM69_sendDelay(uint16_t nextAddress, uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t flags, uint16_t _delay)
{
	RFM69_writeReg(REG_PACKETCONFIG2, (RFM69_readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
	if(_delay)
	{
	  RFM69_TIMER_Start(_delay);//CMT0_SetTimeout(RF69_CSMA_LIMIT_MS);
	  while (!RFM69_TIMER_Fired());
	}
	RFM69_TIMER_Start(RF69_CSMA_LIMIT_MS);//CMT0_SetTimeout(RF69_CSMA_LIMIT_MS);
	while (!RFM69_canSend() && !RFM69_TIMER_Fired()) RFM69_receiveDone();
	RFM69_TIMER_Stop();
	RFM69_sendFrame(nextAddress, toAddress, buffer, bufferSize, flags); //requestACK, false
}

// to increase the chance of getting a packet across, call this function instead of send
// and it handles all the ACK requesting/retrying for you :)
// The only twist is that you have to manually listen to ACK requests on the other side and send back the ACKs
// The reason for the semi-automaton is that the lib is interrupt driven and
// requires user action to read the received data and decide what to do with it
// replies usually take only 5..8ms at 50kbps@915MHz
bool RFM69_sendWithRetry(uint16_t nextAddress,uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t retries, uint16_t retryWaitTime) {
  uint8_t iterator=0;
  for (; iterator <= retries; iterator++)
  {
    RFM69_send(nextAddress, toAddress, buffer, bufferSize, RFM69_FLAGS_RECEIVE_ACK);
    RFM69_receiveBegin();
    RFM69_TIMER_Start(retryWaitTime);
    while (!RFM69_TIMER_Fired())
    {
      if (RFM69_ACKReceived(toAddress))
      {
    	RFM69_TIMER_Stop();
        return true;
      }
    }
  }
  return false;
}

// should be polled immediately after sending a packet with ACK request
bool RFM69_ACKReceived(uint16_t fromNodeID) {
  if (RFM69_receiveDone2())
    return (RFM69_SENDERID == fromNodeID || fromNodeID == RF69_BROADCAST_ADDR) && (RFM69_FLAGS&RFM69_FLAGS_RCV_ACKRECEIVED);
  return false;
}

// check whether an ACK was requested in the last received packet (non-broadcasted packet)
bool RFM69_ACKRequested(void) {
  //return (RFM69_FLAGS&RFM69_FLAGS_RCV_ACKREQUESTED) && (RFM69_TARGETID != RF69_BROADCAST_ADDR);
  return (RFM69_FLAGS&RFM69_FLAGS_RCV_ACKREQUESTED);
}

// should be called immediately after reception in case sender wants ACK
void RFM69_sendACK(const void* buffer, uint8_t bufferSize) {

  int16_t _RSSI = RFM69_RSSI; // save payload received RSSI value

  RFM69_TIMER_Start(RF69_ACK_LIMIT_MS);
  while(!RFM69_TIMER_Fired());
  RFM69_sendFrame(RFM69_SENDERID, RFM69_SENDERID, buffer, bufferSize, RFM69_FLAGS_SEND_ACK);
  RFM69_RSSI = _RSSI; // restore payload RSSI
}

void RFM69_sendFrame(uint16_t nextAddress, uint16_t toAddress, const uint8_t* buffer, uint8_t bufferSize, uint8_t flags)
{
  uint8_t i=0;
  uint8_t dummybytes[RF69_MAX_DATA_LEN + 10];

  RFM69_setMode(RF69_MODE_STANDBY); // turn off receiver to prevent reception while filling fifo
  while ((RFM69_readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00); // wait for ModeReady
  
  RFM69_writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_00); // DIO0 is "Packet Sent"

  if (bufferSize > RF69_MAX_DATA_LEN) bufferSize = RF69_MAX_DATA_LEN;

  if(toAddress==0x0000)
  {
	  toAddress=0x000A;
  }
	/* escrita no buffer FIFO */
	spi_out[0] = (uint32_t)(REG_FIFO | 0x80);
	spi_out[1] = (uint32_t)(bufferSize + 7); //ADD +2
	//REPEAT CHANGES
	spi_out[2] = (uint8_t)(nextAddress&0xFF);
	spi_out[3] = (uint8_t)(nextAddress>>8);
	//~REPEAT CHANGES
	spi_out[4] = (uint8_t)(toAddress&0xFF);
	spi_out[5] = (uint8_t)(toAddress>>8);
	spi_out[6] = (uint8_t)(_address&0xFF);
	spi_out[7] = (uint8_t)(_address>>8);

	spi_out[8] = flags;

	for(i = 0;i < bufferSize; i++)
	{
		spi_out[i+9] = (buffer[i]);
	}
	//~REPEAT CHANGES +2 em tudo

	// reseta flag
	RFM69_sent_flag = 0;

	if(RFM69_LED_normallyON)
		RFM69_LED_OFF();
	else
		RFM69_LED_ON();

  // write to FIFO
  RFM69_select();
  //RFM_SPI_Master_Send_Receive(spi_out,(uint16_t)(9+bufferSize),0,0);// antes tamanho definido para bufferSize+5
																			// porém último byte não era enviado.
																			// Tamanho aumentado em 1.
  //RFM_SPI_Master_Send_Receive(spi_out,(uint16_t)(10+bufferSize),0,0);// normal, para encrypt usar 10
  RFM_SPI_Master_Send_Receive(spi_out,(uint16_t)(10+bufferSize),dummybytes,0);// normal, para encrypt usar 10
  
  RFM69_WAIT_SPI();
  RFM69_unselect();

  // no need to wait for transmit mode to be ready since its handled by the radio
  RFM69_TIMER_Start(RF69_TX_LIMIT_MS);//CMT0_SetTimeout(RF69_TX_LIMIT_MS);
  RFM69_setMode(RF69_MODE_TX);
  while (RFM69_sent_flag == 0)
  {
	if(RFM69_TIMER_Fired())//if(CMT0_Timeout())
	{
		RFM69_reset();
		RFM69_sent_flag=1;
	}
  } // wait for DIO0 to turn HIGH signalling transmission finish

  if(RFM69_LED_normallyON)
  		RFM69_LED_ON();
  	else
  		RFM69_LED_OFF();

  RFM69_TIMER_Stop();//R_CMT0_Stop();
  RFM69_setMode(RF69_MODE_STANDBY);
}

void RFM69_ISR(void) {

  if(_mode == RF69_MODE_TX) //ADICIONADO INTERRUPCAO DE TX
  {
	  //RFM69_LED_OFF();
	  RFM69_sent_flag = 1;
	  return;
  }

  if (_mode == RF69_MODE_RX && (RFM69_readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY))
  {
	uint8_t i=0;

	RFM69_inISR=1;
	RFM69_RSSI = RFM69_readRSSI(0);
	RFM69_setMode(RF69_MODE_STANDBY);
	RFM69_select();
	// determina o tamanho da payload
	spi_out[0] = (uint32_t)(REG_FIFO & 0x7f);
	spi_out[1] = 0x00U;
	RFM_SPI_Master_Send_Receive(spi_out,2,spi_in,2);
	RFM69_WAIT_SPI();
	RFM69_PAYLOADLEN = (uint8_t)spi_in[1];
	//RFM69_PAYLOADLEN = RFM69_PAYLOADLEN > 66 ? 66 : RFM69_PAYLOADLEN; //precaution
	if(RFM69_PAYLOADLEN>66)
	{
		RFM69_PAYLOADLEN = 0;
		RFM69_inISR=0;
		RFM69_unselect();
		RFM69_receiveBegin();
		return;
	}

	spi_in[0] = 0x00U;
	spi_out[0] = 0x00U;
	RFM_SPI_Master_Send_Receive(spi_out,1,spi_in,1);
	RFM69_WAIT_SPI();

	//REPEAT CHANGES
	RFM69_TARGETID = (uint8_t)spi_in[0];
	spi_in[0] = 0x00U;
	spi_out[0] = 0x00U;
	RFM_SPI_Master_Send_Receive(spi_out,1,spi_in,1);
	RFM69_WAIT_SPI();
	RFM69_TARGETID |= (uint16_t)(spi_in[0]<<8);
	//~REPEAT CHANGES

    if(!(_promiscuous || RFM69_TARGETID == _address || RFM69_TARGETID == RF69_BROADCAST_ADDR)|| RFM69_PAYLOADLEN < 7) // address situation could receive packets that are malformed and don't fit this libraries extra fields
	{
      RFM69_PAYLOADLEN = 0;
      RFM69_inISR=0;
      RFM69_unselect();
      RFM69_receiveBegin();
      return;
    }

    RFM69_DATALEN = RFM69_PAYLOADLEN - 3 - 2 -2;//ADD REPEAT -2

    spi_in[0] = 0x00U;
	spi_out[0] = 0x00U;
	RFM_SPI_Master_Send_Receive(spi_out,1,spi_in,1);
	RFM69_WAIT_SPI();
	RFM69_FINALID = (uint8_t)spi_in[0];
	spi_in[0] = 0x00U;
	spi_out[0] = 0x00U;
	RFM_SPI_Master_Send_Receive(spi_out,1,spi_in,1);
	RFM69_WAIT_SPI();
	RFM69_FINALID |= (uint16_t)(spi_in[0]<<8);

    spi_out[0] = 0x00U;
    RFM_SPI_Master_Send_Receive(spi_out,1,spi_in,1);
    RFM69_WAIT_SPI();
    RFM69_SENDERID = (uint8_t)spi_in[0];
    spi_in[0] = 0x00U;
	spi_out[0] = 0x00U;
	RFM_SPI_Master_Send_Receive(spi_out,1,spi_in,1);
	RFM69_WAIT_SPI();
	RFM69_SENDERID |= (uint16_t)(spi_in[0]<<8);

	if(RFM69_SENDERID == 0)
	{
	  RFM69_PAYLOADLEN = 0;
	  RFM69_inISR=0;
	  RFM69_unselect();
	  RFM69_receiveBegin();
	  return;
	}

    spi_out[0] = 0x00U;
    RFM_SPI_Master_Send_Receive(spi_out,1,spi_in,1);
    RFM69_WAIT_SPI();
    RFM69_FLAGS = (uint8_t)spi_in[0];

    //RFM69_ACK_RECEIVED = (uint8_t)(RFM69_FLAGS & RFM69_FLAGS_SEND_ACK); //extract ACK-requested flag
    //RFM69_ACK_REQUESTED = (uint8_t)(RFM69_FLAGS & RFM69_FLAGS_RECEIVE_ACK); //extract ACK-received flag

    for (i= 0; i < RFM69_DATALEN; i++)
    {
    	spi_out[0] = 0x00U;
    	RFM_SPI_Master_Send_Receive(spi_out,1,spi_in,1);
    	RFM69_WAIT_SPI();
    	RFM69_DATA[i] = (uint8_t)spi_in[0];
    }
    if (RFM69_DATALEN < RF69_MAX_DATA_LEN) RFM69_DATA[RFM69_DATALEN] = 0; // add null at end of string
    if((RFM69_DATALEN>0)&&(!(RFM69_FLAGS & RFM69_FLAGS_SEND_ACK))) RFM69_received_flag=1; //#IGUAL ORIGINAL

    if(RFM69_TARGETID == 0x000A)
	{
		RFM69_TARGETID = 0x0000;
	}
    if(RFM69_SENDERID == 0x000A)
	{
		RFM69_SENDERID = 0x0000;
	}

    RFM69_inISR=0;
    RFM69_unselect();
    //RFM69_setMode(RF69_MODE_RX);
    RFM69_setMode(RF69_MODE_STANDBY);
  }
}

void RFM69_receiveBegin(void) {
	RFM69_DATALEN = 0;
	RFM69_SENDERID = 0;
	RFM69_TARGETID = 0;
	RFM69_PAYLOADLEN = 0;
	RFM69_RSSI = 0;
	RFM69_FLAGS=0;
	RFM69_received_flag=0;
  	if (RFM69_readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY)
  		RFM69_writeReg(REG_PACKETCONFIG2, (RFM69_readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
  	RFM69_writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01); // set DIO0 to "PAYLOADREADY" in receive mode
    RFM69_setMode(RF69_MODE_RX);
}

bool RFM69_receiveDone(void) {

  if (RFM69_received_flag)
    {
	  RFM69_setMode(RF69_MODE_STANDBY); // enables interrupts
	  RFM69_received_flag=0;
		 return true;
    }
    else if (_mode == RF69_MODE_RX) // already in RX no payload yet
    {
      return false;
    }
    RFM69_receiveBegin();
    return false;

}

// To enable encryption: radio.encrypt("ABCDEFGHIJKLMNOP");
// To disable encryption: radio.encrypt(null) or radio.encrypt(0)
// KEY HAS TO BE 16 bytes !!!
void RFM69_encrypt(const char* key) {
    uint8_t validKey = key != 0 && strlen(key)!=0;
    uint8_t dummybytes[RF69_MAX_DATA_LEN + 10];
	RFM69_setMode(RF69_MODE_STANDBY);
  if (key != 0)
  {
	uint8_t i=0;
	RFM69_select();
    spi_out[0] = (uint32_t)(REG_AESKEY1 | 0x80);
    for(i = 0;i < 16; i++)
	{
		spi_out[i+1] = (key[i]);
	}
    //RFM_SPI_Master_Send_Receive(spi_out,(uint16_t)(1+16),0,0);
    RFM_SPI_Master_Send_Receive(spi_out,(uint16_t)(1+16),dummybytes,0);
    RFM69_WAIT_SPI();
    RFM69_unselect();
  }
  RFM69_writeReg(REG_PACKETCONFIG2, (RFM69_readReg(REG_PACKETCONFIG2) & 0xFE) | (validKey ? 1 : 0));
}

int16_t RFM69_readRSSI(bool forceTrigger) {
  int16_t rssi = 0;
  if (forceTrigger)
  {
    // RSSI trigger not needed if DAGC is in continuous mode
	  RFM69_writeReg(REG_RSSICONFIG, RF_RSSI_START);
    while ((RFM69_readReg(REG_RSSICONFIG) & RF_RSSI_DONE) == 0x00); // wait for RSSI_Ready
  }
  rssi = -RFM69_readReg(REG_RSSIVALUE);
  rssi >>= 1;
  return rssi;
}

uint8_t RFM69_readReg(uint8_t addr)
{
  spi_out[0] = (uint32_t)(addr & 0x7F);
  spi_out[1] = 0x00U;
  spi_out[2] = 0x00U;
  spi_in[0] = 0x00U;
  spi_in[1] = 0x00U;
 	// espera pela liberação da SPI
  RFM69_select();
  RFM_SPI_Master_Send_Receive(spi_out,2,spi_in,2);
  RFM69_WAIT_SPI();
  RFM69_unselect();
  return (uint8_t)spi_in[1];
}

void RFM69_writeReg(uint8_t addr, uint8_t value)
{
  spi_out[0] = (addr | 0x80);
  spi_out[1] = (value);
  	// espera pela liberação da SPI
  RFM69_select();
  RFM_SPI_Master_Send_Receive(spi_out,2,spi_in,0);
  RFM69_WAIT_SPI();
  RFM69_unselect();
}

// select the transceiver
void RFM69_select(void) {
	RFM69_DISABLE_ISR();
	// espera pela liberação da SPI
	//RFM69_WAIT_SPI();
	RFM69_RESET_SS();
}

// UNselect the transceiver chip
void RFM69_unselect(void) {
	//RFM69_WAIT_SPI();
	RFM69_SET_SS();
	if(!RFM69_inISR)
	{
		RFM69_ENABLE_ISR();
	}
}

// ON  = disable filtering to capture all frames on network
// OFF = enable node/broadcast filtering to capture only frames sent to this/broadcast address
void RFM69_promiscuous(bool onOff) {
  _promiscuous = onOff;
  RFM69_writeReg(REG_PACKETCONFIG1, (RFM69_readReg(REG_PACKETCONFIG1) & 0xF9) | (onOff ? RF_PACKET1_ADRSFILTERING_OFF : RF_PACKET1_ADRSFILTERING_NODEBROADCAST));
}

void RFM69_setHighPower(bool onOff) {
  _isRFM69HW = onOff;
  //RFM69_writeReg(REG_OCP, _isRFM69HW ? RF_OCP_OFF : RF_OCP_ON);
  if (_isRFM69HW) // turning ON
	  RFM69_writeReg(REG_PALEVEL, (RFM69_readReg(REG_PALEVEL) & 0x1F) | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON); // enable P1 & P2 amplifier stages
  else
	  RFM69_writeReg(REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | _powerLevel); // enable P0 only
}

void RFM69_setHighPowerRegs(bool onOff) {
	RFM69_writeReg(REG_TESTPA1, onOff ? 0x5D : 0x55);
	RFM69_writeReg(REG_TESTPA2, onOff ? 0x7C : 0x70);
	RFM69_writeReg(REG_OCP, onOff ? RF_OCP_OFF : RF_OCP_ON);
}

// for debugging
#if 0
void RFM69_readAllRegs(void)
{
  uint8_t regAddr=1;
  uint8_t regVal[0x4f];
  for (regAddr = 1; regAddr <= 0x4F; regAddr++)
  {
	  	 regVal[regAddr-1]=RFM69_readReg(regAddr);
  }
  regAddr=0x4F;
  return;
}
#endif

void RFM69_readConfigRegs(void)
{
  uint8_t _numRegs;
  uint8_t _itRegs;

  _numRegs=sizeof(_configRegsBack);
  for (_itRegs = 0; _itRegs < _numRegs; _itRegs++)
  {
	  _configRegsBack[_itRegs]=RFM69_readReg(RFM69_Registers_LookupTable[_itRegs]);;
  }
  return;
}

uint8_t RFM69_getConfigReg(uint8_t _regg)
{
	return _configRegsBack[_regg];
}

uint8_t RFM69_checkConfigRegs(void)
{
  uint8_t _flag=0;
  uint8_t _numRegs;
  uint8_t _itRegs;

  _numRegs=sizeof(_configRegsBack);
  for (_itRegs = 0; _itRegs < _numRegs; _itRegs++)
  {
	  if(_configRegsBack[_itRegs]!=RFM69_readReg(RFM69_Registers_LookupTable[_itRegs]))
	  {
		  _flag=1;
		  break;
	  }
  }
  if(_flag==1)
  {
	  RFM69_regError++;
	  if(RFM69_regError>RFM69_reConfig_ErrorTimes)
	  {
		  RFM69_regError=0;
		  RFM69_reset();
		  return _itRegs;
	  }
	  return 0xFF;
  }
  else
  {
	  RFM69_regError=0;
  }
  return 0;
}
uint8_t RFM69_configRegError(void)
{
	if(RFM69_regError>RFM69_reConfig_ErrorTimes)
	{
		RFM69_regError=0;
		return true;
	}
	return false;
}

//Call Function Sometimes in loop to check and reinit
void RFM69_autoCheckConfigRegs(void)
{
	RFM69_checkConfigRegs();
	if(RFM69_configRegError())
	{
		RFM69_reset();
	}
}

uint8_t RFM69_readTemperature(uint8_t calFactor) // returns centigrade
{
  RFM69_setMode(RF69_MODE_STANDBY);
  RFM69_writeReg(REG_TEMP1, RF_TEMP1_MEAS_START);
  while ((RFM69_readReg(REG_TEMP1) & RF_TEMP1_MEAS_RUNNING));
  return ~RFM69_readReg(REG_TEMP2) + COURSE_TEMP_COEF + calFactor; // 'complement' corrects the slope, rising temp = rising val
  //return 166-RFM69_readReg(REG_TEMP2);
} // COURSE_TEMP_COEF puts reading in the ballpark, user can add additional correction

void RFM69_rcCalibration(void)
{
	RFM69_writeReg(REG_OSC1, RF_OSC1_RCCAL_START);
  while ((RFM69_readReg(REG_OSC1) & RF_OSC1_RCCAL_DONE) == 0x00);
}

void RFM69_setChannel(uint8_t channel)
{
	RFM69_CHANNEL = channel;
  if (RFM69_CHANNEL > RFM69_FREQ_TABLE_LENGTH - 1) RFM69_CHANNEL = 0;
  RFM69_writeReg(REG_FRFMSB, FRF[RFM69_CHANNEL][0]);
  RFM69_writeReg(REG_FRFMID, FRF[RFM69_CHANNEL][1]);
  RFM69_writeReg(REG_FRFLSB, FRF[RFM69_CHANNEL][2]);
  RFM69_receiveBegin();
}

void RFM69_hop(void)
{
	RFM69_setChannel(++RFM69_CHANNEL);
}

void RFM69_BroadcastRxEnabled(bool en)
{
	uint8_t valor_atual;

	valor_atual = RFM69_readReg(REG_PACKETCONFIG1);

	valor_atual &= 0xF9; // zeramos a parte correspondente a filtragem de endereço

	if(en == true)
	{
		valor_atual |= RF_PACKET1_ADRSFILTERING_NODEBROADCAST;
	}
	else
	{
		valor_atual |= RF_PACKET1_ADRSFILTERING_NODE;
	}

	RFM69_writeReg(REG_PACKETCONFIG1,valor_atual);
}

bool RFM69_receiveDone2(void) {
  if (RFM69_FLAGS&RFM69_FLAGS_RCV_ACKRECEIVED)
    {
	  RFM69_setMode(RF69_MODE_STANDBY); // enables interrupts
      return true;
    }
    else if (_mode == RF69_MODE_RX) // already in RX no payload yet
    {
      return false;
    }
    RFM69_receiveBegin();
    return false;
}

bool RFM69_reInit(void) {

	RFM69_setMode(RF69_MODE_STANDBY);
	RFM69_DISABLE_ISR();
	RFM69_INIT_SS();
	//RFM69_SET_SS();

	RFM69_initialize(RF69_915MHZ, _address, _netID, _isRFM69HW, _promiscuous, _powerLevel);
    //RFM69_encrypt("ABCDEFGHIJKLMNOP");
	RFM69_ENABLE_ISR();

	return true;
}

void RFM69_setRadio(uint8_t radio_set)
{
  uint8_t oldMode = _mode;

  	RFM69_setMode(RF69_MODE_STANDBY);

    RFM69_writeReg(REG_DATAMODUL, rfm69_parameters[radio_set][7]);
  	RFM69_writeReg( REG_BITRATEMSB, rfm69_parameters[radio_set][0]); // default: 4.8 KBPS
  	RFM69_writeReg( REG_BITRATELSB, rfm69_parameters[radio_set][1]);
	RFM69_writeReg( REG_FDEVMSB, rfm69_parameters[radio_set][2]); // default: 5KHz, (FDEV + BitRate / 2 <= 500KHz)
	RFM69_writeReg( REG_FDEVLSB, rfm69_parameters[radio_set][3]);
	RFM69_writeReg( REG_RXBW, rfm69_parameters[radio_set][4]); // (BitRate < 2 * RxBw)
	RFM69_writeReg( REG_AFCBW, rfm69_parameters[radio_set][4]); // Use double the bandwidth during AFC as reception
	RFM69_writeReg(REG_PACKETCONFIG1, rfm69_parameters[radio_set][5]);
	RFM69_writeReg(REG_PACKETCONFIG2, rfm69_parameters[radio_set][6]);

	RFM69_setMode(oldMode);
	while ((RFM69_readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00); // wait for ModeReady
	//RFM69_readAllRegs();
}

void RFM69_setACKtime(uint16_t _acktime)
{
	RF69_ACK_LIMIT_MS=_acktime;
}
uint16_t RFM69_getACKtime(void)
{
	return RF69_ACK_LIMIT_MS;
}

void RFM69_reset(void)
{
	RFM69_INIT_RESET();
	RFM69_SET_RESET();
 	RFM69_TIMER_Start(150);
	while (!RFM69_TIMER_Fired());
	RFM69_RESET_RESET();
	RFM69_TIMER_Start(100);
	while (!RFM69_TIMER_Fired());
	RFM69_reInit();
	RFM69_TIMER_Start(20);
	while (!RFM69_TIMER_Fired());
}

void RFM69_LED_CONTROL (uint8_t _status)
{
	if(_status)
	{
		RFM69_LED_ON();
	}
	else
	{
		RFM69_LED_OFF();
	}
	RFM69_LED_normallyON=_status;
}

void RFM69_TIMER_Start(uint16_t _ms)
{
    _RFM_Timer_init=RFM69_TIMER_getMillis();
    _RFM_Timer_TO=_ms;
}
void RFM69_TIMER_Stop(void)
{
    _RFM_Timer_init=0;
    _RFM_Timer_TO=0;
}
uint8_t RFM69_TIMER_Fired(void)
{
#if 0
    if(TIM_Fired(RFM69_TIMER_getMillis(),&_RFM_Timer_init,_RFM_Timer_TO))
    {
        return 1;
    }
#else
    uint32_t _current = RFM69_TIMER_getMillis();
    if(_RFM_Timer_init>_current)
	{
		_RFM_Timer_init=_current;
		return 0;
	}
	if((_current-_RFM_Timer_init)>_RFM_Timer_TO)
	{
		_RFM_Timer_init=_current;
		return 1;
	}
#endif
    return 0;
}

void RFM69_RESET_SS(void)
{
    SPI1_select(1);
}
void RFM69_INIT_SS(void)
{
}
void RFM69_INIT_RESET(void)
{
}
void RFM69_WAIT_SPI(void)
{
#if SPI0_MINIMAL == 0
    while(!SPI0_isTransferDone());
#else
    while(SPI1_IsBusy());;
#endif
}
void RFM69_CLEAR_SPI(void)
{
    // clear SPI0_isTransferDone
}
void RFM69_DISABLE_ISR(void)
{
    //EIC_InterruptDisable(EIC_PIN_1);
}
void RFM69_ENABLE_ISR(void)
{
    //EIC_InterruptEnable(EIC_PIN_1);
}
void RFM69_LED_ON(void)
{
	RFM69_led=1;
	GPIO_PA0_LED0_Set();   //LED2
}
void RFM69_LED_OFF(void)
{
	RFM69_led=0;
	GPIO_PA0_LED0_Clear(); // LED2
}
void RFM69_LED_TOGGLE(void)
{
	if(RFM69_led)
	{
		RFM69_LED_OFF();
	}
	else
	{
		RFM69_LED_ON();
	}
}
