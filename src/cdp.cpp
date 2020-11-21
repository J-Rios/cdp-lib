
/**
 * @file    cdp.cpp
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    21-11-2020
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * Conditional DePhase (CDP), also known as Differential Manchester
 * Code (DMC), library to ease binary data encoding/decoding.
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

/* Libraries */

#include "cdp.h"

/*****************************************************************************/

/* Constants */

#define LOGIC_LEVEL_LOW  0
#define LOGIC_LEVEL_HIGH 1
#define INITIAL_SIGNAL_LEVEL LOGIC_LEVEL_HIGH

/*****************************************************************************/

/* In-Scope inline Functions */

/**
  * @brief  Return Bit value from given byte.
  * i.e.
  *   GET_BIT(0bABCDEFGH, 0): 0b0000000H
  *   GET_BIT(0bABCDEFGH, 3): 0b0000000E
  *   GET_BIT(0bABCDEFGH, 7): 0b0000000A
  * @param  data Data byte to get the bit value.
  * @param  bit_n Bit number (from LSB to MSB) of data byte to get the value.
  * @return Byte data bit value (0 or 1).
  */
static inline uint8_t GET_BIT(const uint8_t data, const uint8_t bit_n)
{   return ((data >> bit_n) & 0x01);   }
static inline uint8_t GET_BIT(const uint16_t data, const uint8_t bit_n)
{   return ((data >> bit_n) & 0x01);   }
static inline uint8_t GET_BIT(const uint32_t data, const uint8_t bit_n)
{   return ((data >> bit_n) & 0x01);   }

/*****************************************************************************/

/* Constructor & Destructor */

/* CDP constructor */
CDP::CDP()
{}

/* CDP destructor */
CDP::~CDP()
{}

/*****************************************************************************/

/* Encode Methods */

/**
  * @brief  Encode input data with Conditional DePhase (aka
  * Differential Manchester) code.
  * It uses same endian as Ethernet:
  *   Byte order: Big Endian (MSB byte sent first)
  *   Bit order: Little Endian (LSb bit of a byte sent first)
  * Example:
  *   Raw data: 11100101
  *   Encoding bits in order: 10100111
  *   Encoded data: 01011010 10011001
  * @param  data_in Pointer to input data to be encode.
  * @param  data_in_len Number of bytes to encode from input data.
  * @param  data_out Pointer to output data array to store the encoded data.
  * @param  data_out_len Number of bytes that can be stored in the output
  * data array.
  * @return Encode result ok (true/false).
  */
bool CDP::encode(const uint8_t* data_in, const size_t data_in_len,
                 uint8_t* data_out, const size_t data_out_len)
{
    uint8_t current_signal_level = INITIAL_SIGNAL_LEVEL;
    uint16_t encoded_byte = 0x0000;
    size_t encode_byte_i = 0;

    // Check if number of bytes to be encoded doesn't fit in output array
    if(data_in_len*2 > data_out_len)
        return false;

    // For each byte of data
    for(size_t i = 0; i < data_in_len; i++)
    {
        // Encode the byte
        encoded_byte = this->encode_byte(data_in[i], &current_signal_level);

        data_out[encode_byte_i] = (uint8_t)((encoded_byte >> 8) & 0x00ff);
        data_out[encode_byte_i+1] = (uint8_t)(encoded_byte & 0x00ff);

        encode_byte_i = encode_byte_i + 2;
    }

    return true;
}

/**
  * @brief  Encode byte value with Conditional DePhase (aka
  * Differential Manchester) code.
  * @param  data_byte Byte value to be encoded.
  * @param  current_signal_level Pointer to current logic signal level 
  * value (LOW or HIGH) needed to encode each bit.
  * @return Encoded byte.
  */
uint16_t CDP::encode_byte(const uint8_t data_byte,
        uint8_t* current_signal_level)
{
    uint16_t encoded_byte = 0x0000;
    uint8_t encode_bit_i = 0x00;
    uint8_t data_bit = 0x00;

    // For each bit in the byte
    encode_bit_i = 8;
    for(uint8_t i = 0; i < 8; i++)
    {
        // Get and encode the bit
        data_bit = GET_BIT(data_byte, i);
        data_bit = this->encode_bit(data_bit, current_signal_level);

        // Add encoded bit data
        encoded_byte = encoded_byte | (GET_BIT(data_bit, 1) <<
                                       encode_bit_i);
        encoded_byte = encoded_byte | (GET_BIT(data_bit, 0) <<
                                       (encode_bit_i+1));

        encode_bit_i = encode_bit_i + 2;
        if(encode_bit_i >= 16)
            encode_bit_i = 0;
    }

    return encoded_byte;
}

/**
  * @brief  Encode bit value with Conditional DePhase (aka 
  * Differential Manchester) code as IEEE 802.5.
  * Truth table:
  *     cd|00|01|10|11
  *      o|01|10|10|01 ("01" -> signal goes '0' and then go '1')
  *   c - current_signal_level; d - data_bit; o - encoded output
  * @param  data_bit Bit value to be encoded.
  * @param  current_signal_level Pointer to current logic signal level 
  * value (LOW or HIGH) needed to encode each bit.
  * @return Encoded bit in 2 bytes (0x0000, 0x0001, 0x0100 or 0x0101).
  */
uint8_t CDP::encode_bit(const uint8_t data_bit, uint8_t* current_signal_level)
{
    // If current signal and new bit value are the same, signal goes "10";
    // else it goes "01"
    if(data_bit == *current_signal_level)
    {
        *current_signal_level = LOGIC_LEVEL_LOW;
        return 0b10;
    }
    else
    {
        *current_signal_level = LOGIC_LEVEL_HIGH;
        return 0b01;
    }
}

/*****************************************************************************/

/* Decode Methods */

/**
  * @brief  Decode input data with Conditional DePhase (aka
  * Differential Manchester) code.
  * It uses same endian as Ethernet:
  *   Byte order: Big Endian (MSB byte sent first)
  *   Bit order: Little Endian (LSb bit of a byte sent first)
  * Example:
  *   Encoded data: 01011010 10011001
  *   Encoding bits in order: 01011010 10011001
  *   Decoded data: 11100101
  * @param  data_in Pointer to encoded input data to be decoded.
  * @param  data_in_len Number of bytes to decode from input data.
  * @param  data_out Pointer to output data array to store the decoded data.
  * @param  data_out_len Number of bytes that can be stored in the output
  * data array.
  * @return Decode result ok (true/false).
  */
bool CDP::decode(const uint8_t* data_in, const size_t data_in_len,
            uint8_t* data_out, const size_t data_out_len)
{
    uint8_t current_signal_level = INITIAL_SIGNAL_LEVEL;
    uint16_t encoded_byte = 0x0000;
    size_t decode_byte_i = 0;

    // Check if number of bytes to be decoded doesn't fit in output array
    if(data_out_len*2 < data_in_len)
        return false;

    // For each byte of data
    for(size_t i = 0; i < data_in_len; i = i + 2)
    {
        // Create the uint16_t encoded byte data
        encoded_byte = 0x0000;
        encoded_byte = encoded_byte | (data_in[i] << 8);
        encoded_byte = encoded_byte | data_in[i+1];

        // Decode the byte
        data_out[decode_byte_i] =
                this->decode_byte(encoded_byte, &current_signal_level);

        // Increase decoded byte index
        decode_byte_i = decode_byte_i + 1;
    }

    return true;
}

/**
  * @brief  Decode byte value with Conditional DePhase (aka
  * Differential Manchester) code.
  * @param  data_byte Encoded byte value in uint16_t element to be decoded.
  * @param  current_signal_level Pointer to current logic signal level 
  * value (LOW or HIGH) needed to decode each bit.
  * @return Decoded byte.
  */
uint8_t CDP::decode_byte(const uint16_t data_byte,
        uint8_t* current_signal_level)
{
    uint16_t decoded_byte = 0x0000;
    uint8_t data_bit = 0x00;
    uint8_t decode_bit_i = 0;

    // For each bit in the MSB byte
    for(uint8_t i = 8; i < 16; i = i + 2)
    {
        // Get and decode the bit
        data_bit = GET_BIT(data_byte, i+1);
        data_bit = data_bit | (GET_BIT(data_byte, i) << 1);
        data_bit = this->decode_bit(data_bit, current_signal_level);

        // Add decoded bit data
        decoded_byte = decoded_byte | (data_bit << decode_bit_i);

        // Increase decoded bit index
        decode_bit_i = decode_bit_i + 1;
    }

    // For each bit in the LSB byte
    for(uint8_t i = 0; i < 8; i = i + 2)
    {
        // Get and decode the bit
        data_bit = GET_BIT(data_byte, i+1);
        data_bit = data_bit | (GET_BIT(data_byte, i) << 1);
        data_bit = this->decode_bit(data_bit, current_signal_level);

        // Add decoded bit data
        decoded_byte = decoded_byte | (data_bit << decode_bit_i);

        // Increase decoded bit index
        decode_bit_i = decode_bit_i + 1;
    }

    return decoded_byte;
}

/**
  * @brief  Decode bit value with Conditional DePhase (aka 
  * Differential Manchester) code as IEEE 802.5.
  * Truth table:
  *     cd|00|01|10|11
  *      o|01|10|10|01 ("01" -> signal goes '0' and then go '1')
  *   c - current_signal_level_decode; d - data_bit; o - encoded output
  * @param  data_bit Encoded bit value to be decoded.
  * @param  current_signal_level Pointer to current logic signal level 
  * value (LOW or HIGH) needed to decode each bit.
  * @return Decoded bit.
  */
uint8_t CDP::decode_bit(const uint8_t data_bit, uint8_t* current_signal_level)
{
    uint8_t bit_value = 1;

    // If encoded bit == "10"
    if(data_bit == 0b10)
    {
        // Decoded bit value is equal to current signal level
        bit_value = *current_signal_level;
        *current_signal_level = LOGIC_LEVEL_LOW;
        return bit_value;
    }
    // If encoded bit == "01"
    else
    {
        // Decoded bit value is equal to not(current signal level)
        bit_value = !(*current_signal_level);
        *current_signal_level = LOGIC_LEVEL_HIGH;
        return bit_value;
    }
}
