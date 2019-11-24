/**
 * A simple header for reading the STM32 device UUID
 * Tested with STM32F4 and STM32F0 families
 * 
 * Version 1.0
 * Written by Uli Koehler
 * Published on http://techoverflow.net
 * Licensed under CC0 (public domain):
 * https://creativecommons.org/publicdomain/zero/1.0/
 * http://blog.gorski.pm/stm32-unique-id
 * https://stackoverflow.com/questions/29323699/how-to-find-the-device-id-of-a-stm32f103ret
 * https://gist.github.com/ElectronicaXAB3
 * https://www.st.com/content/ccc/resource/technical/document/reference_manual/4a/19/6e/18/9d/92/43/32/DM00043574.pdf/files/DM00043574.pdf/jcr:content/translations/en.DM00043574.pdf
 */
#ifndef __UUID_H
#define __UUID_H
#include <stdint.h>

/**
 * The STM32 factory-programmed UUID memory.
 * Three values of 32 bits each starting at this address
 * Use like this: STM32_UUID[0], STM32_UUID[1], STM32_UUID[2]
 */
 
#ifdef STM32F031x6
#include "stm32f0xx_hal.h"
#define STM32_UUID_ADDR ((uint32_t *)0x1FFFF7AC)
#define FLASH_SIZE_ADDR  ((uint32_t *)0x1FFFF7CC)

#define FLASH_ADDR_FOR_STORING 0x08003C00
#endif

#define FLASH_PART_1 0x00
#define FLASH_PART_2 0x01
#define FLASH_PART_3 0x02
#define FLASH_IP_OFFSET 0x03
#define FLASH_HOST_OFFSET 0x04

typedef struct {
    uint16_t x;  // x-coordinate
    uint16_t y;  // y-coordinate
    uint8_t Wafer;  // Wafer number
    char Lot[7];  // Lot number
    
    uint32_t idPart1;
    uint32_t idPart2 ;
    uint32_t idPart3;
    uint32_t IP_address;
    uint32_t Host_address;
    uint32_t Rev_Board;
    uint32_t Firmware_Ver;
    uint16_t flash_size;
} STM32_UUID;

extern STM32_UUID Signature;

void Init_UUID(void); 

#endif //__UUID_H
