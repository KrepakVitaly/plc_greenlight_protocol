/**
  ******************************************************************************
  *
  * @file    plc_greenlight_repeater.h
  * @author  Vitaliy KREPAK
  * @version V1.0.0
  * @date    12.10.2019
  * @brief   
  *          
  *          
  *
  ******************************************************************************
  */
  
#ifndef PLC_GREENLIGHT_REPEATER_H_
#define PLC_GREENLIGHT_REPEATER_H_


#ifdef STM32F405xx
  #define __bool_true_false_are_defined 1
  #include "stm32f4xx_hal.h"
#endif
#if defined(STM32F030x6) || defined(STM32F031x6)
  #include "stm32f0xx_hal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  uint8_t Enabled;
  uint8_t Status; // WAIT_FOR_ANSWER, READY
  
  uint32_t Next_Domain_Repeater_Address;
  //uint32_t Routing_Table;
  // uint32_t * data_buffer;
  
} PLCRepeater_DataTypeDef;

typedef struct
{
  PLCRepeater_DataTypeDef * this_repeater;
  
} PLCRepeater_HandleTypeDef;



#ifdef __cplusplus
}
#endif

#endif // PLC_GREENLIGHT_REPEATER_H_
