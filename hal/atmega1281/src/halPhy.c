/**
 * \file halPhy.c
 *
 * \brief ATmega1281 PHY interface implementation
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
 * $Id: halPhy.c 5242 2012-09-10 18:37:05Z ataradov $
 *
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "halPhy.h"
#include "phy.h"

/*****************************************************************************
*****************************************************************************/
uint8_t HAL_PhySpiWriteByte(uint8_t value)
{
  return HAL_PhySpiWriteByteInline(value);
}

/*****************************************************************************
*****************************************************************************/
void HAL_PhyReset(void)
{
  HAL_GPIO_PHY_RST_clr();
  HAL_Delay(10);
  HAL_GPIO_PHY_RST_set();
}

/*****************************************************************************
*****************************************************************************/
void halPhyInit(void)
{
  HAL_GPIO_PHY_SLP_TR_out();
  HAL_GPIO_PHY_SLP_TR_clr();
  HAL_GPIO_PHY_RST_out();
  HAL_GPIO_PHY_IRQ_in();
  HAL_GPIO_PHY_CS_out();
  HAL_GPIO_PHY_MISO_in();
  HAL_GPIO_PHY_MOSI_out();
  HAL_GPIO_PHY_SCK_out();

  SPCR = ((1 << SPE) | (1 << MSTR));
  SPSR = (1 << SPI2X);

#if defined(PLATFORM_ZIGBIT)
  EICRB |= (1 << ISC51) | (1 << ISC50);
  EIMSK |= (1 << INT5);
#elif defined(PLATFORM_RCB231)
  EICRA |= (1 << ISC01) | (1 << ISC00);
  EIMSK |= (1 << INT0);
#endif
}

/*****************************************************************************
*****************************************************************************/
#if defined(PLATFORM_ZIGBIT)
ISR(INT5_vect)
#elif defined(PLATFORM_RCB231)
ISR(INT0_vect)
#endif
{
  phyInterruptHandler();
}
