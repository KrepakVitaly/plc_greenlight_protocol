/**
  ******************************************************************************
  *
  * @file    plc_greenlight_protocol.h
  * @author  Vitaliy KREPAK
  * @version V1.0.0
  * @date    12.10.2019
  * @brief   
  *          
  *          
  *
  ******************************************************************************
  */
  
#ifndef PLC_GREENLIGHT_PROTOCOL_H_
#define PLC_GREENLIGHT_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32F405xx
  #define __bool_true_false_are_defined 1
  #include "stm32f4xx_hal.h"
#endif
#if defined(STM32F030x6) || defined(STM32F031x6)
  #include "stm32f0xx_hal.h"
#endif

#define PROTOCOL_REVISION 2
#define PROTOCOL_SHORT_REVISION 0  // for use short version of protocol in bootloader and size-restricted appliances

/* UART CONSTANTS */

#define BAUDRATE 9600

/* REGULAR PACKET CONSTANTS */

#define REGULAR_PACKET_SIZE 22
#define REGULAR_PACKET_HEAD_SIZE  3
#define REGULAR_PACKET_CRC_SIZE   4
#define REGULAR_PACKET_ADDRESS_SIZE  3

#define REGULAR_PACKET_DATA_SIZE  (REGULAR_PACKET_SIZE - \
                                   REGULAR_PACKET_HEAD_SIZE - \
                                   REGULAR_PACKET_ADDRESS_SIZE - \
                                   REGULAR_PACKET_CRC_SIZE)

#define REGULAR_PACKET_HEAD_BYTE_1 0x56								   
#define REGULAR_PACKET_HEAD_BYTE_2 0x12

#define REGULAR_CMD_POS (REGULAR_PACKET_HEAD_SIZE+REGULAR_PACKET_ADDRESS_SIZE+0)
//#define REGULAR_CMD_POS_1 (REGULAR_PACKET_HEAD_SIZE+REGULAR_PACKET_ADDRESS_SIZE+1)
//#define REGULAR_CMD_POS_2 (REGULAR_PACKET_HEAD_SIZE+REGULAR_PACKET_ADDRESS_SIZE+2)

#define REGULAR_DATA_START_POS (REGULAR_CMD_POS+1)

#define REGULAR_CMD_SBA       0x00000001 // Set Brightness Address
#define REGULAR_CMD_SBM       0x00000002 // Set Brightness Multi
#define REGULAR_CMD_SBB       0x00000003 // Set Brightness Broad
#define REGULAR_CMD_GAS       0x00000004 // Get Address Status
#define REGULAR_CMD_GAM       0x00000005 // Get Address MAC
#define REGULAR_CMD_GAT       0x00000006 // Get Address Temp
#define REGULAR_CMD_GAV       0x00000007 // Get Address Volts
#define REGULAR_CMD_GAA       0x00000008 // Get Address Amperes
#define REGULAR_CMD_GAB       0x00000009 // Get Address Brightness
#define REGULAR_CMD_GABV      0x0000000A // Get Address Board Version
#define REGULAR_CMD_GAHV      0x0000000B // Get Address HEX Version
#define REGULAR_CMD_GATIME    0x0000000C // Get Address Time
#define REGULAR_CMD_GATIMECRC 0x0000000D // Get Address TimeTable CRC
#define REGULAR_CMD_GHA       0x0000000E // Get Host Address
#define REGULAR_CMD_UAH       0x0000000F // Update Address HEX

#if REGULAR_PACKET_DATA_SIZE <= 0
	#error Wrong size of data sector of regular packet ( < 0 )
#endif

/* Maintenance PACKET CONSTANTS */

#define MAINTENANCE_PACKET_SIZE          REGULAR_PACKET_SIZE
#define MAINTENANCE_PACKET_HEAD_SIZE     REGULAR_PACKET_HEAD_SIZE
#define MAINTENANCE_PACKET_CRC_SIZE      REGULAR_PACKET_CRC_SIZE
#define MAINTENANCE_PACKET_ADDRESS_SIZE  12

#define MAINTENANCE_PACKET_DATA_SIZE  (MAINTENANCE_PACKET_SIZE - \
                                       MAINTENANCE_PACKET_HEAD_SIZE - \
                                       MAINTENANCE_PACKET_ADDRESS_SIZE - \
                                       MAINTENANCE_PACKET_CRC_SIZE)

#define MAINTENANCE_PACKET_HEAD_BYTE_1 0x26									   
#define MAINTENANCE_PACKET_HEAD_BYTE_2 0x24

// TODO change 3 pos to 1 pos
#define MAINTENANCE_CMD_POS  (MAINTENANCE_PACKET_HEAD_SIZE+MAINTENANCE_PACKET_ADDRESS_SIZE+0)
//#define MAINTENANCE_CMD_POS_1  (MAINTENANCE_PACKET_HEAD_SIZE+MAINTENANCE_PACKET_ADDRESS_SIZE+1)
//#define MAINTENANCE_CMD_POS_2  (MAINTENANCE_PACKET_HEAD_SIZE+MAINTENANCE_PACKET_ADDRESS_SIZE+2)

#define MAINTENANCE_DATA_START_POS (MAINTENANCE_CMD_POS+1)

#define MAINTENANCE_CMD_SAD       0x00000001 // Set Address
#define MAINTENANCE_CMD_SGA       0x00000002 // Set Gateway Address
#define MAINTENANCE_CMD_SAR       0x00000003 // Set As Router
#define MAINTENANCE_CMD_ARTR      0x00000004 // Add Route Table Record
#define MAINTENANCE_CMD_UAH       0x00000005 // Update Address HEX


#if MAINTENANCE_PACKET_DATA_SIZE <= 0
	#error Wrong size of data sector of Maintenance packet ( < 0 )
#endif

/* Repeater PACKET CONSTANTS */

#define REPEATER_PACKET_HEAD_SIZE     REGULAR_PACKET_HEAD_SIZE
#define REPEATER_PACKET_CRC_SIZE      REGULAR_PACKET_CRC_SIZE
#define REPEATER_PACKET_ADDRESS_SIZE  REGULAR_PACKET_ADDRESS_SIZE

#define REPEATER_PACKET_DATA_SIZE     (REGULAR_PACKET_DATA_SIZE + \
                                       REGULAR_PACKET_ADDRESS_SIZE)

#define REPEATER_PACKET_TOTAL_SIZE    (REPEATER_PACKET_HEAD_SIZE + \
                                       REPEATER_PACKET_CRC_SIZE + \
									   REPEATER_PACKET_ADDRESS_SIZE + \
									   REPEATER_PACKET_DATA_SIZE)

#define REPEATER_PACKET_HEAD_BYTE_1 0x16									   
#define REPEATER_PACKET_HEAD_BYTE_2 0x66									   
									   
#if REPEATER_PACKET_TOTAL_SIZE != (REGULAR_PACKET_SIZE+REPEATER_PACKET_ADDRESS_SIZE)
	#error Wrong size of data sector of Repeater packet
#endif

/* Command codes for Regular Packet */

/* Command codes for Maintenance Packet */

/* Command codes for Repeater Packet */

/* Return values of functions */

typedef enum
{
  PACKET_OK       = 0,
  PACKET_ERROR    = 1,
  PACKET_CRC_ERR  = 2,
  PACKET_LEN_ERR  = 3,
  PACKET_HEAD_ERR = 4,
  PACKET_CMD_ERR  = 5,
} PacketStatus;

PacketStatus Get_Regular_Packet     (uint8_t * buf, uint8_t buf_size);
PacketStatus Get_Maintenance_Packet (uint8_t * buf, uint8_t buf_size);
PacketStatus Get_Repeater_Packet    (uint8_t * buf, uint8_t buf_size);

PacketStatus Is_Regular_Packet     (uint8_t * buf, uint8_t buf_size);
PacketStatus Is_Maintenance_Packet (uint8_t * buf, uint8_t buf_size);
PacketStatus Is_Repeater_Packet    (uint8_t * buf, uint8_t buf_size);




#ifdef __cplusplus
}
#endif

#endif // PLC_GREENLIGHT_PROTOCOL_H_
