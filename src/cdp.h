
/**
 * @file    cdp.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    21-11-2020
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * Conditional DePhase (CDP), also known as Differential Manchester
 * Code (DMC), library to ease binary data encoding/decoding as IEEE 802.5.
 *
 * @section LICENSE
 *
 * Copyright (c) 2020 Jose Miguel Rios Rubio. All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*****************************************************************************/

/* Include Guard */

#ifndef CONDITIONAL_DEPHASE_CODE_H_
#define CONDITIONAL_DEPHASE_CODE_H_

/*****************************************************************************/

/* Libraries */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*****************************************************************************/

/* Class Interface */

class CDP
{
    public:

        CDP();
        ~CDP();

        bool encode(const uint8_t* data_in, const size_t data_in_len,
                uint8_t* data_out, const size_t data_out_len);
        bool decode(const uint8_t* data_in, const size_t data_in_len,
                uint8_t* data_out, const size_t data_out_len);

    private:

        uint16_t encode_byte(const uint8_t data_byte,
                uint8_t* current_signal_level);
        uint8_t encode_bit(const uint8_t data_bit,
                uint8_t* current_signal_level);

        uint8_t decode_byte(uint16_t data_byte,
                uint8_t* current_signal_level);
        uint8_t decode_bit(const uint8_t data_bit,
                uint8_t* current_signal_level);
};

/*****************************************************************************/

#endif /* CONDITIONAL_DEPHASE_CODE_H_ */
