/**
 * \file hal.c
 *
 * \brief ATSAMD20 HAL implementation
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
 * $Id: hal.c 8367 2013-07-25 17:18:50Z ataradov $
 *
 */

/*- Includes ---------------------------------------------------------------*/
#include "hal.h"
#include "halPhy.h"
#include "halTimer.h"

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
void HAL_Init(void)
{
  // Switch to 8MHz clock (disable prescaler)
  SYSCTRL_OSC8M = (SYSCTRL_OSC8M & ~SYSCTRL_OSC8M_PRESC_MSK) | SYSCTRL_OSC8M_PRESC(0);

  SYS_EnableInterrupts();

  HAL_TimerInit();
  halPhyInit();
}

/*************************************************************************//**
*****************************************************************************/
void HAL_Delay(uint8_t us)
{
  HAL_TimerDelay(us);
}
