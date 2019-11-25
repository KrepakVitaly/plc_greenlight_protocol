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
#include "crc.h"
#include "stm32_uid.h"
#include "usart.h"
#include "main.h"
#include "tim.h"

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
  uint32_t rxed_ip =  ((uint32_t)packet[REGULAR_PACKET_HEAD_SIZE+0]   << 24) + 
                      ((uint32_t)packet[REGULAR_PACKET_HEAD_SIZE+1]   << 16) +
                      ((uint32_t)packet[REGULAR_PACKET_HEAD_SIZE+2]   << 8);
  
  if ((rxed_ip & 0x00FFFFFF) != (Signature.IP_address & 0x00FFFFFF))
    return 0;
  
  return 1;
}

uint8_t RegularCheckIPaddressMulti (uint8_t * packet)
{
  uint32_t rxed_ip = ((uint32_t)packet[REGULAR_PACKET_HEAD_SIZE+0]  << 24 ) + 
                      ((uint32_t)packet[REGULAR_PACKET_HEAD_SIZE+1] << 16 ) +
                      ((uint32_t)packet[REGULAR_PACKET_HEAD_SIZE+2] << 8);
  
  if ((rxed_ip & 0x00FFFF00) != (Signature.IP_address & 0x00FFFF00))
    return 0;
  
  return 1;
}


uint8_t ProceedMaintenanceCmd(uint8_t * packet)
{
  uint8_t cmd = packet[MAINTENANCE_CMD_POS];
  
  switch(cmd)
  {
    case MAINTENANCE_CMD_SAD:     MaintenanceSetAddress(packet);    break; // Set Address
    case MAINTENANCE_CMD_SGA:     MaintenanceSetGateway(packet);    break; // Set Gateway Address
    case MAINTENANCE_CMD_SAR:     MaintenanceSetAsRoutr(packet);    break; // Set As Router
    case MAINTENANCE_CMD_ARTR:    MaintenanceAddRoutrTb(packet);    break; // Add Route Table Record
    case MAINTENANCE_CMD_UAH:     MaintenanceUpdateFirm(packet);    break; // Update Address HEX
    default: break;
  }

  return 0;
}

uint8_t ProceedRegularCmd(uint8_t * packet)
{
  uint8_t cmd = packet[REGULAR_CMD_POS];

  switch(cmd)
  {
    case REGULAR_CMD_SBA:         RegularSetBrightAddrs(packet);  break; // Set Brightness Address
    case REGULAR_CMD_SBM:         RegularSetBrightMulti(packet);  break; // Set Brightness Multi
    case REGULAR_CMD_SBB:         RegularSetBrightBroad(packet);  break; // Set Brightness Broad
    case REGULAR_CMD_GAS:         RegularGetAddrsStatus(packet);  break; // Get Address Status
    case REGULAR_CMD_GAM:         RegularGetAddressUUID(packet);  break; // Get Address MAC
    case REGULAR_CMD_GAT:         RegularGetAddressTemp(packet);  break; // Get Address Temp
    case REGULAR_CMD_GAV:         RegularGetAddressVolt(packet);  break; // Get Address Volts
    case REGULAR_CMD_GAA:         RegularGetAddressAmps(packet);  break; // Get Address Amperes
    case REGULAR_CMD_GAB:         RegularGetAddrsBright(packet);  break; // Get Address Brightness
    case REGULAR_CMD_GABV:        RegularGetAdrBoardVer(packet);  break; // Get Address Board Version
    case REGULAR_CMD_GAHV:        RegularGetAddrFirmVer(packet);  break; // Get Address HEX Version
    case REGULAR_CMD_GATIME:      RegularGetAddressTime(packet);  break; // Get Address Time
    case REGULAR_CMD_GATIMECRC:   RegularGetAdrsTimeCRC(packet);  break; // Get Address TimeTable CRC
    case REGULAR_CMD_GHA:         RegularGetHostAddress(packet);  break; // Get Host Address
    case REGULAR_CMD_UAH:         RegularUpdateFirmware(packet);  break; // Update Address HEX
    default: break;
  }

  return 0;
}

uint8_t SendRegularPacketToHost(uint8_t * data)
{
  uint8_t packet[REGULAR_PACKET_SIZE] = {  \
            REGULAR_PACKET_SIZE - 1,      //  |
            REGULAR_PACKET_HEAD_BYTE_1,   //  | Head
            REGULAR_PACKET_HEAD_BYTE_2,   //  |
            (uint8_t)(Signature.Host_address >> 24), // |
            (uint8_t)(Signature.Host_address >> 16), // | Host Address
            (uint8_t)(Signature.Host_address >> 8), // |
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


uint8_t MaintenanceSetAddress(uint8_t * packet)
{
  Signature.IP_address = \
    ((uint32_t)packet[MAINTENANCE_PACKET_HEAD_SIZE+MAINTENANCE_PACKET_ADDRESS_SIZE+1] << 16) +
    ((uint32_t)packet[MAINTENANCE_PACKET_HEAD_SIZE+MAINTENANCE_PACKET_ADDRESS_SIZE+2] << 8 );
  
  HAL_FLASH_Unlock();
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FLASH_IP_OFFSET, Signature.IP_address);
  HAL_FLASH_Lock();
  
  uint8_t data[MAINTENANCE_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00};
  
  return SendMaintenancePacketToHost(data); // Send Okay packet
}

uint8_t MaintenanceSetGateway(uint8_t * packet)
{
  Signature.Host_address = \
    ((uint32_t)packet[MAINTENANCE_PACKET_HEAD_SIZE+MAINTENANCE_PACKET_ADDRESS_SIZE+1] << 16) +
    ((uint32_t)packet[MAINTENANCE_PACKET_HEAD_SIZE+MAINTENANCE_PACKET_ADDRESS_SIZE+2] << 8 );

  HAL_FLASH_Unlock();
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FLASH_HOST_OFFSET, Signature.Host_address);
  HAL_FLASH_Lock();
  
  uint8_t data[MAINTENANCE_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00};
  
  return SendMaintenancePacketToHost(data); // Send Okay packet
}

uint8_t MaintenanceSetAsRoutr(uint8_t * packet)
{  
  uint8_t data[MAINTENANCE_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00};
  
  return SendMaintenancePacketToHost(data); // Send Okay packet
}

uint8_t MaintenanceAddRoutrTb(uint8_t * packet)
{
  uint8_t data[MAINTENANCE_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00};
  
  return SendMaintenancePacketToHost(data); // Send Okay packet
}

uint8_t MaintenanceUpdateFirm(uint8_t * packet)
{
  uint8_t data[MAINTENANCE_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00};
  
  SendMaintenancePacketToHost(data); // Send Okay packet
  JumpToBootloader();
  return 0;
}


uint8_t RegularSetBrightAddrs(uint8_t * packet)
{
  // TODO insert PWM update func
  
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  
  brightness_g = packet[REGULAR_DATA_START_POS+2];
  
  user_pwm_setvalue(brightness_g);
  
  RegularGetAddrsStatus(packet);
  return 0;
}

uint8_t RegularSetBrightMulti(uint8_t * packet)
{
  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
  // TODO insert PWM update func
  if (RegularCheckIPaddressMulti (packet) != 1)
    return 1;
  
  brightness_g = packet[REGULAR_DATA_START_POS+2];
  return 0;
}

uint8_t RegularSetBrightBroad(uint8_t * packet)
{
  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
  // TODO insert PWM update func
  brightness_g = packet[REGULAR_DATA_START_POS+2];
  return 0;
}

uint8_t RegularGetAddrsStatus(uint8_t * packet)
{
  
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  data[0] = status_g >> 8;
  data[1] = status_g >> 0;
  
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularGetAddressUUID(uint8_t * packet)
{
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  data[0] = Signature.idPart1 >> 0;
  data[1] = Signature.idPart1 >> 8;
  data[2] = Signature.idPart1 >> 16;
  data[3] = Signature.idPart1 >> 24;

  data[4] = Signature.idPart2 >> 0;
  data[5] = Signature.idPart2 >> 8;
  data[6] = Signature.idPart2 >> 16;
  data[7] = Signature.idPart2 >> 24;
                                            
  data[8] = Signature.idPart3 >> 0;
  data[9] = Signature.idPart3 >> 8;
  data[10] = Signature.idPart3 >> 16;
  data[11] = Signature.idPart3 >> 24;
                                            
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularGetAddressTemp(uint8_t * packet)
{
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  data[0] = temp_g;
  
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularGetAddressVolt(uint8_t * packet)
{
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  data[0] = volt_g;
  
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularGetAddressAmps(uint8_t * packet)
{
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  data[0] = amps_g;
  
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularGetAddrsBright(uint8_t * packet)
{
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  data[0] = brightness_g;
  
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularGetAdrBoardVer(uint8_t * packet)
{
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  data[0] = Signature.Rev_Board >> 24;
  data[1] = Signature.Rev_Board >> 16;
  data[2] = Signature.Rev_Board >> 8;
  data[3] = Signature.Rev_Board >> 0;
  
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularGetAddrFirmVer(uint8_t * packet)
{
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  data[0] = Signature.Firmware_Ver >> 24;
  data[1] = Signature.Firmware_Ver >> 16;
  data[2] = Signature.Firmware_Ver >> 8;
  data[3] = Signature.Firmware_Ver >> 0;
  
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularGetAddressTime(uint8_t * packet)
{
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularGetAdrsTimeCRC(uint8_t * packet)
{
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularGetHostAddress(uint8_t * packet)
{
  if (RegularCheckIPaddress (packet) != 1)
    return 1;
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  data[0] = Signature.Host_address >> 24;
  data[1] = Signature.Host_address >> 16;
  data[2] = Signature.Host_address >> 8;
  data[3] = Signature.Host_address >> 0;
  
  SendRegularPacketToHost(data); // Send Okay packet
  return 0;
}

uint8_t RegularUpdateFirmware(uint8_t * packet)
{
  uint8_t data[REGULAR_PACKET_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};
  
  SendRegularPacketToHost(data); // Send Okay packet
  HAL_Delay(BOOT_DELAY);
  JumpToBootloader();
  return 0;
}


