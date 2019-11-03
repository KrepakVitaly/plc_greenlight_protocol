/**
  ******************************************************************************
  *
  * @file    plc_greenlight_slave.h
  * @author  Vitaliy KREPAK
  * @version V1.0.0
  * @date    12.10.2019
  * @brief   
  *          
  *          
  *
  ******************************************************************************
  */
  
#ifndef PLC_GREENLIGHT_SLAVE_H_
#define PLC_GREENLIGHT_SLAVE_H_


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
  uint32_t MAC0;
  uint32_t MAC1;
  uint32_t MAC2;
  
  uint32_t Address;
  uint32_t Host_Address;
  
  uint8_t Temp0;
  uint8_t Temp1;
  uint8_t Temp2;
  
  uint8_t Led_Driver_Status;
  uint8_t Brightness;
  
  //uint32_t Time_Table_CRC32;
  //uint32_t Encryption_Private_Key;
  
  uint8_t Currrent_Sense;
  
  //uint8_t On_Off_without_cmd;

} PLCSlave_DataTypeDef;

typedef struct
{
  PLCSlave_DataTypeDef * this_module;
  //uart handler
  //adc handler
  //timers handler
  //circular buffer
  
} PLCSlave_HandleTypeDef;


#ifdef __cplusplus
}
#endif

#endif // PLC_GREENLIGHT_SLAVE_H_
