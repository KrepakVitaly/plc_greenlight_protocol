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

#include "stm32f4xx_hal_plc_uart.h"
#include "circular_buffer.h"
#include "plc_greenlight_protocol.h"
#include "crc.h"
#include "stm32_uid.h"
#include "usart.h"
#include "main.h"
#include "tim.h"
#include "adc.h"


void ResetStoredValues(void)
{
  FLASH_EraseInitTypeDef EraseInitStruct;
  
  // Sector erase:
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;

  // Set the number of sectors to erase
  EraseInitStruct.NbSectors = 1; 

  // Set the initial sector to erase
  EraseInitStruct.Sector = FLASH_ADDR_FOR_STORING;

  // contains the configuration information on faulty page in case of error
  // (0xFFFFFFFF means that all the pages have been correctly erased)
  uint32_t PageError = 0; 

  // perform erase
  HAL_FLASH_Unlock();
  //__HAL_FLASH_CLEAR_FLAG(FLASH_SR_EOP | FLASH_SR_WRPERR | FLASH_SR_PGERR); 
  HAL_FLASHEx_Erase(&EraseInitStruct, &PageError); // TODO check HAL_OK output
  HAL_FLASH_Lock();
  
  Init_UUID();
}

uint8_t IsValidMaintenancePacket(uint8_t * test_packet)
{
  // Test packet length byte
  if (test_packet[0] != MAINTENANCE_PACKET_SIZE - 1)
    return 0;
  // Test packet head 1 byte
  if (test_packet[1] != MAINTENANCE_PACKET_HEAD_BYTE_1)
    return 0;
  // Test packet head 2 byte
  if (test_packet[2] != MAINTENANCE_PACKET_HEAD_BYTE_2)
    return 0;
  
  uint32_t rx_part_id1 = ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+0] << 0) + 
                         ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+1] << 8) +
                         ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+2] << 16 ) +
                         ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+3] << 24 );
  
  uint32_t rx_part_id2 = ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+4] << 0) + 
                         ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+5] << 8) +
                         ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+6] << 16 ) +
                         ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+7] << 24 );
  
  uint32_t rx_part_id3 = ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+8] << 0) + 
                         ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+9] << 8) +
                         ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+10] << 16 ) +
                         ((uint32_t)test_packet[REGULAR_PACKET_HEAD_SIZE+11] << 24 );
  // Test target id of the packet
  if (Signature.idPart1 != rx_part_id1)
    return 0;
  if (Signature.idPart2 != rx_part_id2)
    return 0;
  if (Signature.idPart3 != rx_part_id3)
    return 0;  

  uint32_t calc_crc = HAL_CRC_Calculate(&hcrc, 
                        (uint32_t*)test_packet, 
                        MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE);
  
  uint32_t rxed_crc = ((uint32_t)test_packet[MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE] << 24) + 
                      ((uint32_t)test_packet[MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE+1] << 16) +
                      ((uint32_t)test_packet[MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE+2] << 8 ) +
                      ((uint32_t)test_packet[MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE+3] << 0 );
  
  // Test packet crc sum
  if (rxed_crc != calc_crc)
    return 0;
  
  return 1;
}

uint8_t IsValidRegularPacket(uint8_t * test_packet)
{
  // Test packet length byte
  if (test_packet[0] != REGULAR_PACKET_SIZE - 1)
    return 0;
  
  if (test_packet[1] != REGULAR_PACKET_HEAD_BYTE_1)
    return 0;
  
  if (test_packet[2] != REGULAR_PACKET_HEAD_BYTE_2)
    return 0;

  uint32_t calc_crc = HAL_CRC_Calculate(&hcrc, 
                        (uint32_t*)test_packet, 
                        REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE);
  
  uint32_t rxed_crc = ((uint32_t)test_packet[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE]   << 24) + 
                      ((uint32_t)test_packet[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE+1] << 16) +
                      ((uint32_t)test_packet[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE+2] << 8 ) +
                      ((uint32_t)test_packet[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE+3] << 0 );

  if (rxed_crc != calc_crc)
    return 0;
  
  return 1;
}

uint8_t RegularCheckIPaddress (uint8_t * packet)
{
  if (packet[REGULAR_PACKET_HEAD_SIZE+0] != (uint8_t)(Signature.IP_address >> 16) &&
      packet[REGULAR_PACKET_HEAD_SIZE+1] != (uint8_t)(Signature.IP_address >> 8)  &&
      packet[REGULAR_PACKET_HEAD_SIZE+2] != (uint8_t)(Signature.IP_address >> 0)    )
    return 0;
  
  return 1;
}

uint8_t RegularCheckIPaddressMulti (uint8_t * packet)
{
  if (packet[REGULAR_PACKET_HEAD_SIZE+0] != (uint8_t)(Signature.IP_address >> 16) &&
      packet[REGULAR_PACKET_HEAD_SIZE+1] != (uint8_t)(Signature.IP_address >> 8) )
    return 0;
  
  return 1;
}


uint8_t SendRegularPacketToHost(uint8_t * data)
{
  uint8_t packet[REGULAR_PACKET_SIZE] = {  \
            REGULAR_PACKET_SIZE - 1,      //  |
            REGULAR_PACKET_HEAD_BYTE_1,   //  | Head
            REGULAR_PACKET_HEAD_BYTE_2,   //  |
            (uint8_t)(Signature.Host_address >> 16), // |
            (uint8_t)(Signature.Host_address >> 8), // | Host Address
            (uint8_t)(Signature.Host_address >> 0), // |
            data[0], data[1], data [2], data [3], 
            data[4], data[5], data [6], data [7], 
            data[8], data[9], data[10], data[11], 
            0x00, 0x00, 0x00, 0x00
  };

  uint32_t calc_crc = HAL_CRC_Calculate(&hcrc, 
                        (uint32_t*)packet, 
                        MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE);
  
  packet[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE]   = (uint8_t)(calc_crc >> 24);
  packet[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE+1] = (uint8_t)(calc_crc >> 16);
  packet[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE+2] = (uint8_t)(calc_crc >> 8);
  packet[REGULAR_PACKET_SIZE-REGULAR_PACKET_CRC_SIZE+3] = (uint8_t)(calc_crc >> 0);
  
  return HAL_UART_Transmit(&huart1, packet, REGULAR_PACKET_SIZE, UART_TIMEOUT);
}

uint8_t SendMaintenancePacketToHost(uint8_t * data)
{
  uint8_t packet[MAINTENANCE_PACKET_SIZE] = {  \
            MAINTENANCE_PACKET_SIZE - 1,      //  |
            MAINTENANCE_PACKET_HEAD_BYTE_1,   //  | Head
            MAINTENANCE_PACKET_HEAD_BYTE_2,   //  |
            (uint8_t)(Signature.idPart1 >> 0) & 0xFF, // |
            (uint8_t)(Signature.idPart1 >> 8) & 0xFF, // |
            (uint8_t)(Signature.idPart1 >> 16)  & 0xFF, // | Host Address
            (uint8_t)(Signature.idPart1 >> 24)  & 0xFF, // |
            (uint8_t)(Signature.idPart2 >> 0) & 0xFF, // |
            (uint8_t)(Signature.idPart2 >> 8) & 0xFF, // |
            (uint8_t)(Signature.idPart2 >> 16)  & 0xFF, // | Host Address
            (uint8_t)(Signature.idPart2 >> 24)  & 0xFF, // |
            (uint8_t)(Signature.idPart3 >> 0) & 0xFF, // |
            (uint8_t)(Signature.idPart3 >> 8) & 0xFF, // |
            (uint8_t)(Signature.idPart3 >> 16)  & 0xFF, // | Host Address
            (uint8_t)(Signature.idPart3 >> 24)  & 0xFF, // |
            data[0], data[1], data [2],
            0x00, 0x00, 0x00, 0x00
  };
  
  uint32_t calc_crc = HAL_CRC_Calculate(&hcrc, 
                        (uint32_t*)packet, 
                        MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE);
  
  packet[MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE]   = (uint8_t)(calc_crc >> 24);
  packet[MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE+1] = (uint8_t)(calc_crc >> 16);
  packet[MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE+2] = (uint8_t)(calc_crc >> 8);
  packet[MAINTENANCE_PACKET_SIZE-MAINTENANCE_PACKET_CRC_SIZE+3] = (uint8_t)(calc_crc >> 0);
  
  return HAL_UART_Transmit(&huart1, packet, MAINTENANCE_PACKET_SIZE, UART_TIMEOUT);
}




