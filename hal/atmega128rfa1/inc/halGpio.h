/**
 * \file halGpio.h
 *
 * \brief ATmega128rfa1 GPIO interface
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
 * $Id: halGpio.h 5223 2012-09-10 16:47:17Z ataradov $
 *
 */

#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

#include "sysTypes.h"

#define HAL_GPIO_PIN(name, port, bit) \
  INLINE void    HAL_GPIO_##name##_set()      { PORT##port |= (1 << bit); } \
  INLINE void    HAL_GPIO_##name##_clr()      { PORT##port &= ~(1 << bit); } \
  INLINE void    HAL_GPIO_##name##_toggle()   { PORT##port ^= (1 << bit); } \
  INLINE void    HAL_GPIO_##name##_in()       { DDR##port &= ~(1 << bit); PORT##port &= ~(1 << bit); } \
  INLINE void    HAL_GPIO_##name##_out()      { DDR##port |= (1 << bit); } \
  INLINE void    HAL_GPIO_##name##_pullup()   { PORT##port |= (1 << bit); } \
  INLINE uint8_t HAL_GPIO_##name##_read()     { return (PIN##port & (1 << bit)) != 0; } \
  INLINE uint8_t HAL_GPIO_##name##_state()    { return (DDR##port & (1 << bit)) != 0; }

#endif // _HAL_GPIO_H_
