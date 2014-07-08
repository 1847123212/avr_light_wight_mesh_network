/**
 * \file halBoard.h
 *
 * \brief Board interface
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
 * $Id: halBoard.h 9267 2014-03-18 21:46:19Z ataradov $
 *
 */

#ifndef _HAL_BOARD_H_
#define _HAL_BOARD_H_

/*- Includes ---------------------------------------------------------------*/
#include "halGpio.h"

/*- Definitions ------------------------------------------------------------*/
#if defined(PLATFORM_RCB128RFA1)
  #define HAL_BOARD_RCB_BB
  HAL_GPIO_PIN(nEN,       D, 4);
  HAL_GPIO_PIN(FORCEON,   D, 6);
  HAL_GPIO_PIN(nFORCEOFF, D, 7);

#elif defined(PLATFORM_RCB231)
  HAL_GPIO_PIN(nEN,       C, 4);
  HAL_GPIO_PIN(FORCEON,   C, 6);
  HAL_GPIO_PIN(nFORCEOFF, C, 7);

#endif

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
INLINE void HAL_BoardInit(void)
{
#if defined(HAL_BOARD_RCB_BB)
  HAL_GPIO_nEN_out();
  HAL_GPIO_nEN_clr();

  HAL_GPIO_FORCEON_out();
  HAL_GPIO_FORCEON_set();

  HAL_GPIO_nFORCEOFF_out();
  HAL_GPIO_nFORCEOFF_set();
#endif
}

#endif // _HAL_BOARD_H_
