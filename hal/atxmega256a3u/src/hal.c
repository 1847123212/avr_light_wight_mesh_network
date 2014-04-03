/**
 * \file hal.c
 *
 * \brief ATxmega256a3u HAL implementation
 *
 * Copyright (C) 2012-2014, Atmel Corporation. All rights reserved.
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
 * Modification and other use of this code is subject to Atmel's Limited
 * License Agreement (license.txt).
 *
 * $Id: hal.c 9267 2014-03-18 21:46:19Z ataradov $
 *
 */

/*- Includes ---------------------------------------------------------------*/
#include "hal.h"
#include "halPhy.h"
#include "halTimer.h"

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
static void halSetSystemFrequency(void)
{
  // Assuming operation from the internal RC oscillator (2 MHz)
#if F_CPU == 4000000
  OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | OSC_PLLFAC3_bm;
  OSC.CTRL |= OSC_PLLEN_bm;
  while (0 == (OSC.STATUS & OSC_PLLRDY_bm));
  CCP = CCP_IOREG_gc;
  CLK.PSCTRL = CLK_PSADIV0_bm | CLK_PSADIV1_bm;

#elif F_CPU == 8000000
  OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | OSC_PLLFAC3_bm;
  OSC.CTRL |= OSC_PLLEN_bm;
  while (0 == (OSC.STATUS & OSC_PLLRDY_bm));
  CCP = CCP_IOREG_gc;
  CLK.PSCTRL = CLK_PSADIV0_bm;

#elif F_CPU == 12000000
  OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | OSC_PLLFAC3_bm | OSC_PLLFAC2_bm;
  OSC.CTRL |= OSC_PLLEN_bm;
  while (0 == (OSC.STATUS & OSC_PLLRDY_bm));
  CCP = CCP_IOREG_gc;
  CLK.PSCTRL = CLK_PSADIV0_bm;

#elif F_CPU == 16000000
  OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | OSC_PLLFAC3_bm;
  OSC.CTRL |= OSC_PLLEN_bm;
  while (0 == (OSC.STATUS & OSC_PLLRDY_bm));

#elif F_CPU == 32000000
  OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | OSC_PLLFAC4_bm;
  OSC.CTRL |= OSC_PLLEN_bm;
  while (0 == (OSC.STATUS & OSC_PLLRDY_bm));

#else
  #error Unsuppoerted F_CPU
#endif

  CCP = CCP_IOREG_gc;
  CLK.CTRL = CLK_SCLKSEL2_bm;
}

/*************************************************************************//**
*****************************************************************************/
void HAL_Init(void)
{
  halSetSystemFrequency();
  PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm| PMIC_LOLVLEN_bm;
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
