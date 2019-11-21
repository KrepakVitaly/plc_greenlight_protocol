/**
  ******************************************************************************
  *
  * @file    stm32f0xx_hal_plc_uart.c
  * @author  Vitaliy KREPAK
  * @version V1.0.0
  * @date    12.10.2019
  * @brief   PLC HAL module driver over HAL UART STM CUBE Driver.
  *          
  *          
  *
  @verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
  */

#include "stm32f0xx_hal_plc_uart.h"
#include "circular_buffer.h"
#include "plc_greenlight_protocol.h"
  

uint8_t CheckPacket(void)
{
  CircularBuffer_Typedef kq130_buf;
  uint8_t new_byte_received = 0;
  
  // if there is a chance to contain full packet in circular buffer
  // must be greater or equal than PACKET_SIZE
  uint16_t len = CircularBuffer_GetLength(&kq130_buf);
  
  if (len >= REGULAR_PACKET_SIZE) 
  {
    new_byte_received = 0;
    uint8_t packet_analyze_buf[REGULAR_PACKET_SIZE];
    CircularBuffer_GetLastNValues(&kq130_buf, packet_analyze_buf, REGULAR_PACKET_SIZE);
    
    //if packet was found
    if ( packet_analyze_buf[1] == 0x56 && // byte No1
         packet_analyze_buf[2] == 0x12 && // byte No2
         packet_analyze_buf[3] == 0x54  ) // byte No3
    {
      //if address is valid
    }
    //if broadcast packet was found
    else if ( packet_analyze_buf[1] == 0x32 && // byte No1
              packet_analyze_buf[2] == 0x02 && // byte No2
              packet_analyze_buf[3] == 0x77  ) // byte No3
    {
      CircularBuffer_RemoveLastNValues(&kq130_buf, REGULAR_PACKET_SIZE); //packet was read and throwed away
    }
    //if multicast packet was found
    else if ( packet_analyze_buf[1] == 0x19 && // byte No1
              packet_analyze_buf[2] == 0xB3 && // byte No2
              packet_analyze_buf[3] == 0xEE  ) // byte No3
    {
      CircularBuffer_RemoveLastNValues(&kq130_buf, REGULAR_PACKET_SIZE); //packet was read and throwed away
    }
    else //if packet was not found
    {
    }
  }
  
  return 0;
}
	