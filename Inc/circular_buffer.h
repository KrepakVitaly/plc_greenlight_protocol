/**
  ******************************************************************************
  *
  * @file    circular_buffer.h
  * @author  Vitaliy KREPAK
  * @version V1.0.1
  * @date    30.10.2019
  * @brief   Realisation of circular buffer of uint8_t.
  *          This buffer was implemented for
  *          parcing the UART byte flow and finding packets
  *
  ******************************************************************************
  */
  
#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

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

typedef enum
{ 
  False = 0,  
  True = 1 
} Bool;

#define CIRC_BUFFER_LEN   64
#define CIRC_BUFFER_OK     0
#define CIRC_BUFFER_ERR    1

typedef struct
{
  uint8_t data[CIRC_BUFFER_LEN];
  uint32_t max_size;
  uint32_t head;
  uint32_t tail;
  uint32_t size;
  
} CircularBuffer_Typedef;

uint8_t  CircularBuffer_Init (CircularBuffer_Typedef* buf);
uint8_t  CircularBuffer_Clear (CircularBuffer_Typedef* buf);
Bool     CircularBuffer_IsEmpty (CircularBuffer_Typedef* buf);
Bool     CircularBuffer_IsFull (CircularBuffer_Typedef* buf);

uint8_t  CircularBuffer_Put_OW (CircularBuffer_Typedef* buf, uint8_t val);

uint8_t  CircularBuffer_Put (CircularBuffer_Typedef* buf, uint8_t val);

uint16_t CircularBuffer_GetLength (CircularBuffer_Typedef* buf);

uint8_t  CircularBuffer_RemoveLastValue (CircularBuffer_Typedef* buf);

uint8_t  CircularBuffer_RemoveLastNValues (CircularBuffer_Typedef* buf, 
                                           uint16_t num);
										   
uint8_t CircularBuffer_GetLastValue(CircularBuffer_Typedef* buf, 
                                    uint8_t * result);
									
uint16_t CircularBuffer_GetLastNValues(CircularBuffer_Typedef* buf, 
                                       uint8_t * result, uint16_t num);
									   
uint8_t CircularBuffer_RemoveFirstValue(CircularBuffer_Typedef* buf);

uint8_t CircularBuffer_RemoveFirstNValues(CircularBuffer_Typedef* buf, 
                                          uint16_t num);
										  
uint8_t CircularBuffer_GetRandValue(CircularBuffer_Typedef* buf, uint16_t idx);

// uint16_t dist (uint16_t head, uint16_t tail, uint16_t module);

#ifdef __cplusplus
}
#endif

#endif // CIRCULAR_BUFFER_H_
