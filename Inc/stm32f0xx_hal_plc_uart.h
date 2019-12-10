/**
  ******************************************************************************
  *
  * @file    stm32f0xx_hal_plc_uart.h
  * @author  Vitaliy KREPAK
  * @version V1.0.0
  * @date    12.10.2019
  * @brief   
  *          
  *          
  *
  @verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
  */

#ifndef STM32F0XX_HAL_PLC_UART_H_
#define STM32F0XX_HAL_PLC_UART_H_


#ifdef STM32F405xx
  #define __bool_true_false_are_defined 1
  #include "stm32f4xx_hal.h"
  #include "stm32f4xx_hal_def.h"
#endif
#if defined(STM32F030x6) || defined(STM32F031x6)
  #include "stm32f0xx_hal.h"
  #include "stm32f0xx_hal_def.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define UART_TIMEOUT 1000
#define BOOT_DELAY 4000

/**
  * @brief PLC UART Init Structure definition
  */
typedef struct
{
  uint8_t Mode;                      /*!< Specifies whether the Transparent or KQ100 proprietary mode is enabled or disabled. */

  UART_HandleTypeDef * huart;        /*!< Specifies the UART channel of STM */
}PLC_UART_InitTypeDef;


typedef enum
{
  HAL_PLC_UART_STATE_RESET             = 0x00U,   /*!< Peripheral is not initialized
                                                   Value is allowed for gState and RxState */
  HAL_PLC_UART_STATE_READY             = 0x20U,   /*!< Peripheral Initialized and ready for use
                                                   Value is allowed for gState and RxState */
  HAL_PLC_UART_STATE_BUSY              = 0x24U,   /*!< an internal process is ongoing 
                                                   Value is allowed for gState only */
  HAL_PLC_UART_STATE_BUSY_TX           = 0x21U,   /*!< Data Transmission process is ongoing
                                                   Value is allowed for gState only */
  HAL_PLC_UART_STATE_BUSY_RX           = 0x22U,   /*!< Data Reception process is ongoing
                                                   Value is allowed for RxState only */
  HAL_PLC_UART_STATE_TIMEOUT           = 0xA0U,   /*!< Timeout state
                                                   Value is allowed for gState only */
  HAL_PLC_UART_STATE_ERROR             = 0xE0U    /*!< Error
                                                   Value is allowed for gState only */
}HAL_PLC_UART_StateTypeDef;


/**
  * @brief  PLC UART handle Structure definition
  */
typedef struct
{

  PLC_UART_InitTypeDef       Init;             /*!< UART communication parameters      */

  uint8_t                    *pTxBuffPtr;      /*!< Pointer to UART Tx transfer Buffer */

  uint16_t                   TxXferSize;       /*!< UART Tx Transfer size              */

  uint16_t                   TxXferCount;      /*!< UART Tx Transfer Counter           */

  uint8_t                    *pRxBuffPtr;      /*!< Pointer to UART Rx transfer Buffer */

  uint16_t                   RxXferSize;       /*!< UART Rx Transfer size              */

  uint16_t                   RxXferCount;      /*!< UART Rx Transfer Counter           */

  uint32_t                   ErrorCode;       /*!< UART Error code                    */

  HAL_PLC_UART_StateTypeDef  gState;      /*!< UART state information related to global Handle management 
											  and also related to Tx operations. */

  HAL_PLC_UART_StateTypeDef  RxState;     /*!< UART state information related to Rx operations. */
  
}PLC_UART_HandleTypeDef;


uint8_t HAL_PLC_UART_Rxed_Slave_Packet(void);
uint8_t HAL_PLC_UART_Rxed_Repeater_Packet(void);


uint8_t HAL_PLC_UART_Is_Packet_Received(void);
uint8_t HAL_PLC_UART_CheckRegularPacket(void);

void ResetStoredValues(void);

uint8_t IsValidMaintenancePacket(uint8_t *);
uint8_t IsValidRegularPacket(uint8_t *);
uint8_t ProceedMaintenanceCmd(uint8_t *);
uint8_t ProceedRegularCmd(uint8_t *);
uint8_t RegularCheckIPaddress (uint8_t * packet);
uint8_t RegularCheckIPaddressMulti (uint8_t * packet);

uint8_t SendRegularPacketToHost(uint8_t *);
uint8_t SendMaintenancePacketToHost(uint8_t *);

uint8_t MaintenanceSetAddress(uint8_t *); // Set Address
uint8_t MaintenanceSetGateway(uint8_t *);// Set Gateway Address
uint8_t MaintenanceSetAsRoutr(uint8_t *);// Set As Router
uint8_t MaintenanceAddRoutrTb(uint8_t *);// Add Route Table Record
uint8_t MaintenanceUpdateFirm(uint8_t *);// Update Address HEX
uint8_t MaintenanceRestStored(uint8_t *);

uint8_t RegularSetBrightAddrs(uint8_t *);
uint8_t RegularSetBrightMulti(uint8_t *);
uint8_t RegularSetBrightBroad(uint8_t *);
uint8_t RegularGetAddrsStatus(uint8_t *);
uint8_t RegularGetAddressUUID(uint8_t *);
uint8_t RegularGetAddressTemp(uint8_t *);
uint8_t RegularGetAddressVolt(uint8_t *);
uint8_t RegularGetAddressAmps(uint8_t *);
uint8_t RegularGetAddrsBright(uint8_t *);
uint8_t RegularGetAdrBoardVer(uint8_t *);
uint8_t RegularGetAddrFirmVer(uint8_t *);
uint8_t RegularGetAddressTime(uint8_t *);
uint8_t RegularGetAdrsTimeCRC(uint8_t *);
uint8_t RegularGetHostAddress(uint8_t *);
uint8_t RegularUpdateFirmware(uint8_t *);


#ifdef __cplusplus
}
#endif

#endif // STM32F0XX_HAL_PLC_UART_H_
