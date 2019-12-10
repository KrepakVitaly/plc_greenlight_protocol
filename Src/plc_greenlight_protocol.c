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

