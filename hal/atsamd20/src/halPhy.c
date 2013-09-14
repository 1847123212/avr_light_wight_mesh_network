/**
 * \file halPhy.c
 *
 * \brief ATSAMD20 PHY interface implementation
 *
 * Copyright (C) 2012-2013, Atmel Corporation. All rights reserved.
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
 * $Id: halPhy.c 8367 2013-07-25 17:18:50Z ataradov $
 *
 */

/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "halPhy.h"
#include "phy.h"

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
uint8_t HAL_PhySpiWriteByte(uint8_t value)
{
  return HAL_PhySpiWriteByteInline(value);
}

/*************************************************************************//**
*****************************************************************************/
void HAL_PhyReset(void)
{
  HAL_GPIO_PHY_RST_clr();
  HAL_Delay(10);
  HAL_GPIO_PHY_RST_set();
}

/*************************************************************************//**
*****************************************************************************/
static void halPhySpiSync(void)
{
  while (SC2_SPI_STATUS & SC1_SPI_STATUS_SYNCBUSY);
}

/*************************************************************************//**
*****************************************************************************/
static void halPhyEicSync(void)
{
  while (EIC_STATUS & EIC_STATUS_SYNCBUSY);
}

/*************************************************************************//**
*****************************************************************************/
void halPhyInit(void)
{
  // Configure IO pins
  HAL_GPIO_PHY_SLP_TR_out();
  HAL_GPIO_PHY_SLP_TR_clr();
  HAL_GPIO_PHY_RST_out();
  HAL_GPIO_PHY_IRQ_in();
  HAL_GPIO_PHY_IRQ_pmuxen();
  HAL_GPIO_PHY_CS_out();
  HAL_GPIO_PHY_MISO_in();
  HAL_GPIO_PHY_MISO_pmuxen();
  HAL_GPIO_PHY_MOSI_out();
  HAL_GPIO_PHY_MOSI_pmuxen();
  HAL_GPIO_PHY_SCK_out();
  HAL_GPIO_PHY_SCK_pmuxen();

  // Configure SPI
  PORTA_PMUX8 = PORTA_PMUX8_PMUXE(2/*C*/);
  PORTA_PMUX9 = PORTA_PMUX9_PMUXE(2/*C*/) | PORTA_PMUX9_PMUXO(2/*C*/);

  PM_APBCMASK |= PM_APBCMASK_SERCOM1;

  GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID(0x0e/*SERCOM1_CORE*/) | GCLK_CLKCTRL_GEN(0);

  SC1_SPI_CTRLB = SC1_SPI_CTRLB_RXEN;
  halPhySpiSync();

  SC1_SPI_CTRLA = SC1_SPI_CTRLA_MODE(3/*SPI master*/) | SC1_SPI_CTRLA_DOPO | SC1_SPI_CTRLA_ENABLE;
  halPhySpiSync();

  // Configure EIC
  PORTB_PMUX6 = PORTB_PMUX6_PMUXO(0/*A*/);

  GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID(0x03/*EIC*/) | GCLK_CLKCTRL_GEN(0);

  EIC_CTRL = EIC_CTRL_ENABLE;
  halPhyEicSync();

  EIC_CONFIG1 |= EIC_CONFIG1_SENSE13(1/*rise*/);

  EIC_INTENSET = 1 << HAL_PHY_IRQ_INDEX;
  NVIC_ISER = NVIC_ISER_EIC;
}

/*************************************************************************//**
*****************************************************************************/
void HAL_IrqHandlerEic(void)
{
  if (EIC_INTFLAG & (1 << HAL_PHY_IRQ_INDEX))
  {
    phyInterruptHandler();
    EIC_INTFLAG = 1 << HAL_PHY_IRQ_INDEX;
  }
}
