/**
  ******************************************************************************
  *
  * @file    plc_greenlight_protocol.c 
  * @author  Vitaliy KREPAK
  * @version V1.0.0
  * @date    12.10.2019
  * @brief   
  *          
  *          
  *
  ******************************************************************************
  */
  
#include "plc_greenlight_protocol.h"
#include "crc.h"

PacketStatus Get_Regular_Packet     (uint8_t * buf, uint8_t buf_size)
{
  buf[0] =  REGULAR_PACKET_SIZE - 1;     //  |
  buf[1] =  REGULAR_PACKET_HEAD_BYTE_1;  //  | Head
  buf[2] =  REGULAR_PACKET_HEAD_BYTE_2;  //  |
  buf[3] =  0x00; // | 
  buf[4] =  0x00; // | Host Address
  buf[5] =  0x00; // |

  uint32_t calc_crc = HAL_CRC_Calculate(&hcrc, 
                        (uint32_t*)buf, 
                        MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE);
  
  buf[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE]   = (uint8_t)(calc_crc >> 24);
  buf[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE+1] = (uint8_t)(calc_crc >> 16);
  buf[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE+2] = (uint8_t)(calc_crc >> 8);
  buf[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE+3] = (uint8_t)(calc_crc >> 0);
}

PacketStatus Get_Maintenance_Packet (uint8_t * buf, uint8_t buf_size);
PacketStatus Get_Repeater_Packet    (uint8_t * buf, uint8_t buf_size);


PacketStatus Is_Regular_Packet (uint8_t * buf, uint8_t buf_size)
{
    uint32_t calc_crc = HAL_CRC_Calculate(&hcrc, 
                        (uint32_t*)buf, 
                        MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE);
}


PacketStatus Is_Maintenance_Packet (uint8_t * buf, uint8_t buf_size);
PacketStatus Is_Repeater_Packet    (uint8_t * buf, uint8_t buf_size);

