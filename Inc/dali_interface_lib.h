/**
  ******************************************************************************
  * @file           : dali_interface_lib.h
  * @brief          : Header for dali_interface_lib.c file.
  *                   This file contains the common defines of the application.
  *****************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __DALI_INTERFACE_LIB_H_
#define __DALI_INTERFACE_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#ifdef STM32F031x6
#include "stm32f0xx_hal.h"
#endif

#define DALI_MAX_CTR 34

/*In accordance with the DIN EN 60929 standard, addresses and commands are
transmitted as numbers with a length of two bytes.

These commands take the form Y AAA AAA S xxxx xxxx. 
Each letter here stands for one bit.

Y: type of address
    0bin:    short address
    1bin:    group address or collective call

A: significant address bit

S: selection bit (specifies the significance of the following eight bits):
    0bin:    the 8 xxxx xxxx bits contain a value for direct control of the lamp power
    1bin:    the 8 xxxx xxxx bits contain a command number.

x: a bit in the lamp power or in the command number*/

extern uint8_t dali_cntr;

extern uint32_t dali_cmd;
extern uint32_t dali_cmd_sh;


uint8_t step_DALI_set_brightness(uint8_t * dali_cntr);

#ifdef __cplusplus
}
#endif

#endif  /* __DALI_INTERFACE_LIB_H_ */
