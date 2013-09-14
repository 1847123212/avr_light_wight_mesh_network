/**
 * \file phy.c
 *
 * \brief AT86RF230 PHY implementation
 *
 * Copyright (C) 2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 * $Id: phy.c 5223 2012-09-10 16:47:17Z ataradov $
 *
 */

#ifdef PHY_AT86RF230

#include <stdbool.h>
#include "phy.h"
#include "halPhy.h"

/*****************************************************************************
*****************************************************************************/
#define ED_UPDATE_INTERVAL  140 // us

/*****************************************************************************
*****************************************************************************/
enum
{
  PHY_REQ_NONE    = 0,
  PHY_REQ_CHANNEL = (1 << 0),
  PHY_REQ_PANID   = (1 << 1),
  PHY_REQ_ADDR    = (1 << 2),
  PHY_REQ_RX      = (1 << 3),
  PHY_REQ_ED      = (1 << 4),
};

typedef struct PhyIb_t
{
  uint8_t     request;

  uint8_t     channel;
  uint16_t    panId;
  uint16_t    addr;
  bool        rx;
} PhyIb_t;

/*****************************************************************************
*****************************************************************************/
static void phyWriteRegister(uint8_t reg, uint8_t value);
static uint8_t phyReadRegister(uint8_t reg);
static void phyTrxSetState(uint8_t state);
static void phySetRxState(void);

/*****************************************************************************
*****************************************************************************/
static PhyIb_t       phyIb;
volatile PHY_State_t phyState = PHY_STATE_INITIAL;
volatile uint8_t     phyTxStatus;
volatile int8_t      phyRxRssi;
static uint8_t       phyRxBuffer[128];

/*****************************************************************************
*****************************************************************************/
void PHY_Init(void)
{
  HAL_PhyReset();

  phyWriteRegister(TRX_STATE_REG, TRX_CMD_TRX_OFF);
  while (TRX_STATUS_TRX_OFF != (phyReadRegister(TRX_STATUS_REG) & TRX_STATUS_TRX_STATUS_MASK));

  phyWriteRegister(PHY_TX_PWR_REG, (1 << 7)/*TX_AUTO_CRC_ON*/ | 0/* +3 dBm */);

  phyWriteRegister(IRQ_MASK_REG, 0x00);
  phyReadRegister(IRQ_STATUS_REG);
  phyWriteRegister(IRQ_MASK_REG, TRX_END_MASK);

  phyIb.request = PHY_REQ_NONE;
  phyIb.rx = false;
  phyState = PHY_STATE_IDLE;
}

/*****************************************************************************
*****************************************************************************/
void PHY_SetRxState(bool rx)
{
  phyIb.request |= PHY_REQ_RX;
  phyIb.rx = rx;
}

/*****************************************************************************
*****************************************************************************/
void PHY_SetChannel(uint8_t channel)
{
  phyIb.request |= PHY_REQ_CHANNEL;
  phyIb.channel = channel;
}

/*****************************************************************************
*****************************************************************************/
void PHY_SetPanId(uint16_t panId)
{
  phyIb.request |= PHY_REQ_PANID;
  phyIb.panId = panId;
}

/*****************************************************************************
*****************************************************************************/
void PHY_SetShortAddr(uint16_t addr)
{
  phyIb.request |= PHY_REQ_ADDR;
  phyIb.addr = addr;
}

/*****************************************************************************
*****************************************************************************/
bool PHY_Busy(void)
{
  return PHY_STATE_IDLE != phyState || PHY_REQ_NONE != phyIb.request;
}

/*****************************************************************************
*****************************************************************************/
void PHY_Sleep(void)
{
  phyTrxSetState(TRX_CMD_TRX_OFF);
  HAL_PhySlpTrSet();
  phyState = PHY_STATE_SLEEP;
}

/*****************************************************************************
*****************************************************************************/
void PHY_Wakeup(void)
{
  HAL_PhySlpTrClear();
  phySetRxState();
  phyState = PHY_STATE_IDLE;
}

/*****************************************************************************
*****************************************************************************/
void PHY_DataReq(uint8_t *data, uint8_t size)
{
  phyTrxSetState(TRX_CMD_TX_ARET_ON);

  HAL_PhySpiSelect();
  HAL_PhySpiWriteByte(RF_CMD_FRAME_W);
  HAL_PhySpiWriteByte(size + 2/*crc*/);
  for (uint8_t i = 0; i < size; i++)
    HAL_PhySpiWriteByte(data[i]);
  HAL_PhySpiDeselect();

  phyWriteRegister(TRX_STATE_REG, TRX_CMD_TX_START);
  phyState = PHY_STATE_TX_WAIT_END;
}

#ifdef PHY_ENABLE_ENERGY_DETECTION
/*****************************************************************************
*****************************************************************************/
void PHY_EdReq(void)
{
  phyIb.request |= PHY_REQ_ED;
}
#endif

/*****************************************************************************
*****************************************************************************/
static void phyWriteRegister(uint8_t reg, uint8_t value)
{
  phyWriteRegisterInline(reg, value);
}

/*****************************************************************************
*****************************************************************************/
static uint8_t phyReadRegister(uint8_t reg)
{
  return phyReadRegisterInline(reg);
}

/*****************************************************************************
*****************************************************************************/
static void phySetRxState(void)
{
  if (phyIb.rx)
    phyTrxSetState(TRX_CMD_RX_AACK_ON);
  else
    phyTrxSetState(TRX_CMD_TRX_OFF);
}

/*****************************************************************************
*****************************************************************************/
static void phyHandleSetRequests(void)
{
  phyTrxSetState(TRX_CMD_TRX_OFF);

  if (phyIb.request & PHY_REQ_CHANNEL)
  {
    uint8_t v = phyReadRegister(PHY_CC_CCA_REG) &  ~0x1f; 
    phyWriteRegister(PHY_CC_CCA_REG, v | phyIb.channel);
  }

  if (phyIb.request & PHY_REQ_PANID)
  {
    uint8_t *d = (uint8_t *)&phyIb.panId;
    phyWriteRegister(PAN_ID_0_REG, d[0]);
    phyWriteRegister(PAN_ID_1_REG, d[1]);
  }

  if (phyIb.request & PHY_REQ_ADDR)
  {
    uint8_t *d = (uint8_t *)&phyIb.addr;
    phyWriteRegister(SHORT_ADDR_0_REG, d[0]);
    phyWriteRegister(SHORT_ADDR_1_REG, d[1]);
    phyWriteRegister(CSMA_SEED_0_REG, d[0] + d[1]);
  }

#ifdef PHY_ENABLE_ENERGY_DETECTION
  if (phyIb.request & PHY_REQ_ED)
  {
    phyWriteRegister(IRQ_MASK_REG, 0);
    phyTrxSetState(TRX_CMD_RX_ON);

    phyWriteRegister(PHY_ED_LEVEL_REG, 0);
    HAL_Delay(ED_UPDATE_INTERVAL);
    phyRxRssi = (int8_t)phyReadRegister(PHY_ED_LEVEL_REG);

    PHY_EdConf(phyRxRssi + PHY_RSSI_BASE_VAL);

    phyReadRegister(IRQ_STATUS_REG);
    phyWriteRegister(IRQ_MASK_REG, TRX_END_MASK);
  }
#endif

  phySetRxState();

  phyIb.request = PHY_REQ_NONE;
}

/*****************************************************************************
*****************************************************************************/
static void phyTrxSetState(uint8_t state)
{
  phyWriteRegister(TRX_STATE_REG, TRX_CMD_FORCE_TRX_OFF);
  phyWriteRegister(TRX_STATE_REG, state);
  while (state != (phyReadRegister(TRX_STATUS_REG) & TRX_STATUS_TRX_STATUS_MASK));
}

/*****************************************************************************
*****************************************************************************/
void PHY_TaskHandler(void)
{
  switch (phyState)
  {
    case PHY_STATE_IDLE:
    {
      if (phyIb.request)
        phyHandleSetRequests();
    } break;

    case PHY_STATE_TX_CONFIRM:
    {
      PHY_DataConf(phyTxStatus);

      while (TRX_CMD_PLL_ON != (phyReadRegister(TRX_STATUS_REG) & TRX_STATUS_TRX_STATUS_MASK));
      phySetRxState();
      phyState = PHY_STATE_IDLE;
    } break;

    case PHY_STATE_RX_IND:
    {
      PHY_DataInd_t ind;
      uint8_t size;

      HAL_PhySpiSelect();
      HAL_PhySpiWriteByte(RF_CMD_FRAME_R);
      size = HAL_PhySpiWriteByte(0);
      for (uint8_t i = 0; i < size + 1/*lqi*/; i++)
        phyRxBuffer[i] = HAL_PhySpiWriteByte(0);
      HAL_PhySpiDeselect();

      ind.data = phyRxBuffer;
      ind.size = size - 2/*crc*/;
      ind.lqi  = phyRxBuffer[size];
      ind.rssi = phyRxRssi + PHY_RSSI_BASE_VAL;
      PHY_DataInd(&ind);

      while (TRX_CMD_PLL_ON != (phyReadRegister(TRX_STATUS_REG) & TRX_STATUS_TRX_STATUS_MASK));
      phySetRxState();
      phyState = PHY_STATE_IDLE;
    } break;

    default:
      break;
  }
}

#endif // PHY_AT86RF230
