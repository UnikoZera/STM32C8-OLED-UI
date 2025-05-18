/*
 * flash.h
 *
 *  Created on: May 18, 2025
 *      Author: UnikoZera
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "stm32f1xx_hal.h" // For uintx_t types and HAL types
#include <stdint.h>

// W25Q64 Function Prototypes
uint8_t W25Q64_Init(void);
uint16_t W25Q64_Read_ID(void);
uint32_t W25Q64_Read_JEDEC_ID(void);

void W25Q64_Erase_Chip(void);
void W25Q64_Erase_Sector(uint32_t SectorAddr);

void W25Q64_Page_Program(uint32_t WriteAddr, uint8_t *pBuffer, uint16_t NumByteToWrite);
void W25Q64_Write(uint32_t WriteAddr, uint8_t *pBuffer, uint32_t NumByteToWrite);
void W25Q64_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead);
void W25Q64_Fast_Read(uint32_t ReadAddr, uint8_t *pBuffer, uint32_t NumByteToRead);

// Helper functions (if you want to expose them, otherwise keep static in .c)
// void W25Q64_Write_Enable(void);
// void W25Q64_Write_Disable(void);
// uint8_t W25Q64_Read_StatusRegister1(void);
// void W25Q64_Wait_Busy(void);

#endif /* INC_FLASH_H_ */
