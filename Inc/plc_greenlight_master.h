/**
  ******************************************************************************
  *
  * @file    plc_greenlight_master.h
  * @author  Vitaliy KREPAK
  * @version V1.0.0
  * @date    12.10.2019
  * @brief   
  *          
  *          
  *
  ******************************************************************************
  */
  
#ifndef PLC_GREENLIGHT_MASTER_H_
#define PLC_GREENLIGHT_MASTER_H_


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
  uint32_t tag;  

} PLCMaster_DataTypeDef;

typedef struct
{
  PLCMaster_DataTypeDef * this_master;
  
} PLCMaster_HandleTypeDef;

#ifdef __cplusplus
}
#endif

#endif // PLC_GREENLIGHT_MASTER_H_
