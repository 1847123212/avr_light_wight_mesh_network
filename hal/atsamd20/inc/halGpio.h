/**
 * \file halGpio.h
 *
 * \brief ATSAMD20 GPIO interface
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
 * $Id: halGpio.h 8410 2013-08-08 16:59:55Z ataradov $
 *
 */

#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

/*- Includes ---------------------------------------------------------------*/
#include "sysTypes.h"

/*- Definitions ------------------------------------------------------------*/
#define HAL_GPIO_PIN(name, port, bit) \
  INLINE void    HAL_GPIO_##name##_set(void)      { PORT##port##_OUTSET = (1 << bit); } \
  INLINE void    HAL_GPIO_##name##_clr(void)      { PORT##port##_OUTCLR = (1 << bit); } \
  INLINE void    HAL_GPIO_##name##_toggle(void)   { PORT##port##_OUTTGL = (1 << bit); } \
  INLINE void    HAL_GPIO_##name##_in(void)       { PORT##port##_DIRCLR = (1 << bit);   \
                                                    PORT##port##_PINCFG##bit |= PORTA_PINCFG##bit##_INEN; \
                                                    PORT##port##_PINCFG##bit &= ~PORTA_PINCFG##bit##_PULLEN; } \
  INLINE void    HAL_GPIO_##name##_out(void)      { PORT##port##_DIRSET = (1 << bit);   \
                                                    PORT##port##_PINCFG##bit |= PORTA_PINCFG##bit##_INEN; } \
  INLINE void    HAL_GPIO_##name##_pullup(void)   { PORT##port##_OUTSET = (1 << bit); \
                                                    PORT##port##_PINCFG##bit |= PORTA_PINCFG##bit##_PULLEN; } \
  INLINE uint8_t HAL_GPIO_##name##_read(void)     { return (PORT##port##_IN & (1 << bit)) != 0; } \
  INLINE uint8_t HAL_GPIO_##name##_state(void)    { return (PORT##port##_DIR & (1 << bit)) != 0; } \
  INLINE void    HAL_GPIO_##name##_pmuxen(void)   { PORT##port##_PINCFG##bit |= PORTA_PINCFG##bit##_PMUXEN; } \
  INLINE void    HAL_GPIO_##name##_pmuxdis(void)  { PORT##port##_PINCFG##bit &= ~PORTA_PINCFG##bit##_PMUXEN; }

#endif // _HAL_GPIO_H_
