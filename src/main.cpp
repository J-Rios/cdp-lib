
/**
 * @file    main.h
 * @author  Jose Miguel Rios Rubio <jrios.github@gmail.com>
 * @date    21-11-2020
 * @version 1.0.0
 *
 * @section DESCRIPTION
 *
 * CDP library main test code that check if encode-decode data is ok.
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

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <time.h> 

#include "cdp.h"

/*****************************************************************************/

/* Inline Functions */

/* printf a byte value as binary bits */
static inline void printfb(const uint8_t b)
{
    printf("%c%c%c%c%c%c%c%c", 
            (b & 0x80 ? '1' : '0'), (b & 0x40 ? '1' : '0'),
            (b & 0x20 ? '1' : '0'), (b & 0x10 ? '1' : '0'),
            (b & 0x08 ? '1' : '0'), (b & 0x04 ? '1' : '0'),
            (b & 0x02 ? '1' : '0'), (b & 0x01 ? '1' : '0'));
}

/*****************************************************************************/

/* Functions Prototypes */

uint8_t gen_random_byte(void);
bool test0(void);
bool test1(void);

/*****************************************************************************/

/* Main Function */

/**
  * @brief  Launch the tests and shows results.
  * @return Program execution return code.
  */
int main(int argc, char *argv[])
{
    test0() ? printf("TEST 0 Result - OK") : printf("TEST 0 Result - FAIL");
    test1() ? printf("TEST 1 Result - OK") : printf("TEST 1 Result - FAIL");

    printf("\n\n--------------------------------\n\n");

    return 0;
}

/**
  * @brief  Test encode-decode for 4096 bytes and compare decode result
  * with initial data to check if anything goes wrong.
  * @return Test result.
  */
bool test1(void)
{
    const uint16_t DATA_SIZE = 4096;
    uint8_t data[DATA_SIZE] = { 0 };
    uint8_t encoded_data[DATA_SIZE*2] = { 0 };
    uint8_t decoded_data[DATA_SIZE] = { 0 };
    CDP Cdp;
    bool any_fail = false;

    printf("\n\n--------------------------------\n\n");
    printf("TEST 1:\n\n");

    // Feed input data with pseudo-random data
    for(uint32_t i = 0; i < DATA_SIZE; i++)
        data[i] = gen_random_byte();

    // Show input data
    #if 0
        printf("\nInput data:\n");
        for(uint32_t i = 0; i < DATA_SIZE; i++)
            printfb(data[i]);
        printf("\n\n");
    #endif

    // Encode data
    if(Cdp.encode(data, DATA_SIZE, encoded_data, DATA_SIZE*2) == false)
    {
        printf("Error encoding data.\n");
        return false;
    }

    // Show encoded data
    #if 0
        printf("Encoded data:\n");
        for(uint32_t i = 0; i < DATA_SIZE*2; i++)
            printfb(encoded_data[i]);
        printf("\n\n");
    #endif

    // Decode previous encoded data
    if(Cdp.decode(encoded_data, DATA_SIZE*2, decoded_data, DATA_SIZE) == false)
    {
        printf("Error decoding data.\n");
        return false;
    }

    // Show encoded data
    #if 0
        printf("Decoded data:\n");
        for(uint32_t i = 0; i < DATA_SIZE; i++)
            printfb(decoded_data[i]);
        printf("\n\n");
    #endif

    // Compare decoded data with original input data
    printf("Comparing decoded bytes with original input bytes...\n");
    for(uint32_t i = 0; i < DATA_SIZE; i++)
    {
        if(decoded_data[i] != data[i])
        {
            printf("Byte %d - FAIL!\n", i);
            printf("    Input byte != Decoded byte\n");
            printfb(data[i]); printf(" != "); printfb(decoded_data[i]);
            printf("\n");
            any_fail = true;
        }
    }
    if(any_fail)
    {
        printf("Error, decoded data != input data.\n\n");
        return false;
    }
    else
        printf("Ok, decoded data == input data.\n\n");

    return true;
}

/**
  * @brief  Test encode-decode for just one known byte value.
  * @return Test result.
  */
bool test0(void)
{
    const uint8_t DATA_SIZE = 1;
    uint8_t data[DATA_SIZE] = { 0b01110100 };
    uint8_t encoded_data[DATA_SIZE*2] = { 0 };
    CDP Cdp;

    printf("\n--------------------------------\n\n");
    printf("TEST 0:\n\n");
    printf("Input data:   "); printfb(data[0]); printf("\n");
    if(Cdp.encode(data, DATA_SIZE, encoded_data, DATA_SIZE*2) == false)
    {
        printf("Error encoding data.\n");
        return false;
    }
    printf("Encoded data: "); printfb(encoded_data[0]); printf(", ");
    printfb(encoded_data[1]); printf("\n");
    data[0] = 0x00;
    if(Cdp.decode(encoded_data, DATA_SIZE*2, data, DATA_SIZE) == false)
    {
        printf("Error decoding data.\n");
        return false;
    }
    printf("Decoded data: "); printfb(data[0]); printf("\n\n");

    return true;
}

/*****************************************************************************/

/* Auxiliar Functions */

/**
  * @brief  Generate a pseudo-random byte value.
  * @return Generated random byte value.
  */
uint8_t gen_random_byte(void)
{
    // Set random seed based on actual time, just once
    static uint8_t once = 1;
    if(once)
    {
        once = 0;
        srand(time(0));
    }

    // Return the generated byte value
    return ((uint8_t)(rand()));
}
