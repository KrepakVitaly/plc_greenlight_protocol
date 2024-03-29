#include "stm32_uid.h"


STM32_UUID Signature;

void Init_UUID(void)
{
  Signature.idPart1 = STM32_UUID_ADDR[0];
  Signature.idPart2 = STM32_UUID_ADDR[1];
  Signature.idPart3 = STM32_UUID_ADDR[2];
  
  Signature.x = (Signature.idPart1 >> 16) & 0xFFFF;  // x-coordinate
  Signature.y = Signature.idPart1 & 0xFFFF;          // y-coordinate
  Signature.Wafer = Signature.idPart2 >> 0;          // Wafer number
  
  Signature.Lot[0] = (Signature.idPart2 >> 8 ) & 0xFF;  // Lot number
  Signature.Lot[1] = (Signature.idPart2 >> 16) & 0xFF;  // Lot number
  Signature.Lot[2] = (Signature.idPart2 >> 24) & 0xFF;  // Lot number
  Signature.Lot[3] = (Signature.idPart3 >> 0 ) & 0xFF;  // Lot number
  Signature.Lot[4] = (Signature.idPart3 >> 8 ) & 0xFF;  // Lot number
  Signature.Lot[5] = (Signature.idPart3 >> 16) & 0xFF;  // Lot number
  Signature.Lot[6] = (Signature.idPart3 >> 24) & 0xFF;  // Lot number
  Signature.flash_size = ((FLASH_SIZE_ADDR[0] >> 8) & 0xFF) + ((FLASH_SIZE_ADDR[0] << 8) & 0xFF00); //size in kbytes
  
  //Signature.Offline = (uint8_t) *((__IO uint32_t*) (FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*OFFLINE_OFFSET));
  //Signature.Offline++;

  Signature.Firmware_Ver = 1230;
  Signature.Rev_Board = 1250;
  
  Signature.Firmware_CRC32 = *((__IO uint32_t*) (FLASH_ADDR_FOR_STORING+4*FW_CRC_OFFSET));

  if ( Signature.Firmware_CRC32 == 0xFFFFFFFFU )
  {
    // Set the starting address
    uint32_t startingAddress = 0x08004000U; 
    uint32_t endingAddress = startingAddress + 0x00004000U;
    
    uint8_t * data = (uint8_t *)((__IO uint8_t*) startingAddress);
    
    __HAL_CRC_DR_RESET(&hcrc);
    uint32_t crcResult = 0;
    crcResult = HAL_CRC_Calculate(&hcrc,  (uint32_t*)data, 1);
    for(uint32_t address = startingAddress; address < endingAddress; address += 1)    
    {
        data = (uint8_t *)((__IO uint8_t*) address);
        crcResult = HAL_CRC_Accumulate(&hcrc, (uint32_t*)data, 1);
     
        if (*data == 0x00)
        {
          if ( ((uint8_t *)((__IO uint8_t*) (address))-2)[0] == 0x12 && 
               ((uint8_t *)((__IO uint8_t*) (address))-1)[0] == 0x7A &&
               ((uint8_t *)((__IO uint8_t*) (address))+1)[0] == 0xFF && 
               ((uint8_t *)((__IO uint8_t*) (address))+2)[0] == 0xFF && 
               ((uint8_t *)((__IO uint8_t*) (address))+3)[0] == 0xFF && 
               ((uint8_t *)((__IO uint8_t*) (address))+4)[0] == 0xFF && 
               ((uint8_t *)((__IO uint8_t*) (address))+5)[0] == 0xFF)
            break;
        }
    }
    
    Signature.Firmware_CRC32 = crcResult;
    
    HAL_FLASH_Unlock();
    uint8_t res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FW_CRC_OFFSET, 
                     crcResult);
    HAL_FLASH_Lock();
  }

  
  uint32_t flash_part_id1 = *((__IO uint32_t*) (FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FLASH_PART_1));
  uint32_t flash_part_id2 = *((__IO uint32_t*) (FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FLASH_PART_2));
  uint32_t flash_part_id3 = *((__IO uint32_t*) (FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FLASH_PART_3));
  
  HAL_FLASH_Unlock();
  if (Signature.idPart1 != flash_part_id1)
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FLASH_PART_1, (uint32_t)STM32_UUID_ADDR[0]);
  
  if (Signature.idPart2 != flash_part_id2)
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FLASH_PART_2, (uint32_t)STM32_UUID_ADDR[1]);
  
  if (Signature.idPart3 != flash_part_id3)
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FLASH_PART_3, (uint32_t)STM32_UUID_ADDR[2]);
  HAL_FLASH_Lock();
  
  // Read IP address from FLASH
  Signature.IP_address   = * ((__IO uint32_t*) (FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FLASH_IP_OFFSET));
  Signature.Host_address = * ((__IO uint32_t*) (FLASH_ADDR_FOR_STORING+sizeof(uint32_t)*FLASH_HOST_OFFSET));
}
