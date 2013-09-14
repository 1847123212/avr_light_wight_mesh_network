/**
 * \file halTimer.c
 *
 * \brief ATSAMD20 timer implementation
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
 * $Id: halTimer.c 8367 2013-07-25 17:18:50Z ataradov $
 *
 */

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include "hal.h"
#include "halTimer.h"

/*- Definitions ------------------------------------------------------------*/
#define TIMER_PRESCALER  8
#define TIMER_TOP        (((F_CPU / 1000ul) / TIMER_PRESCALER) * HAL_TIMER_INTERVAL)

/*- Variables --------------------------------------------------------------*/
volatile uint8_t halTimerIrqCount;
static volatile uint8_t halTimerDelayInt;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
static inline void halTimerSync(void)
{
  while (TC4_16_STATUS & TC4_16_STATUS_SYNCBUSY);
}

/*************************************************************************//**
*****************************************************************************/
void HAL_TimerInit(void)
{
  halTimerIrqCount = 0;

  PM_APBCMASK |= PM_APBCMASK_TC4;

  GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID(0x15/*TC4,TC5*/) | GCLK_CLKCTRL_GEN(0);

  TC4_16_CTRLA = TC4_16_CTRLA_MODE(0/*16 bit*/) | TC4_16_CTRLA_WAVEGEN(1/*MFRQ*/) |
      TC4_16_CTRLA_PRESCALER(3/*DIV8*/) | TC4_16_CTRLA_PRESCSYNC(0x1/*PRESC*/);
  halTimerSync();

  TC4_16_COUNT = 0;
  halTimerSync();

  TC4_16_CC0 = TIMER_TOP;
  halTimerSync();

  TC4_16_CTRLA |= TC4_16_CTRLA_ENABLE;
  halTimerSync();

  TC4_16_INTENSET = TC4_16_INTENSET_MC0;
  NVIC_ISER = NVIC_ISER_TC4;
}

/*************************************************************************//**
*****************************************************************************/
void HAL_TimerDelay(uint16_t us)
{
  uint16_t target = TC4_16_COUNT + us;

  if (target > TIMER_TOP)
    target -= TIMER_TOP;

  TC4_16_CC1 = target;
  halTimerSync();

  halTimerDelayInt = 0;
  TC4_16_INTENSET = TC4_16_INTENSET_MC1;
  while (0 == halTimerDelayInt);
  TC4_16_INTENCLR = TC4_16_INTENCLR_MC1;
}

/*************************************************************************//**
*****************************************************************************/
void HAL_IrqHandlerTc4(void)
{
  if (TC4_16_INTFLAG & TC4_16_INTFLAG_MC0)
  {
    halTimerIrqCount++;
    TC4_16_INTFLAG = TC4_16_INTFLAG_MC0;
  }
  else
  {
    halTimerDelayInt = 1;
    TC4_16_INTFLAG = TC4_16_INTFLAG_MC1;
  }
}
