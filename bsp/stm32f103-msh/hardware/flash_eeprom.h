#ifndef __FLASH_EEPROM_H
#define __FLASH_EEPROM_H
#include "stm32f1xx_hal.h"

//stm32f103rct6	flash:256k 	ram:48k
#define FLASH_PAGESIZE		0x800				//2048
#define FLASH_SIZE				0x40000			//256K
#define FLASH_START_ADDR	0x08000000
#define FLASH_END_ADDR		(FLASH_START_ADDR + FLASH_SIZE)

#define FLASH_EEPROM_START_ADDR	(FLASH_END_ADDR - FLASH_PAGESIZE)//0x803f800	totol:256K ,start at the last 2K ,last page
#define FLASH_EEPROM_SIZE	FLASH_PAGESIZE	//2048

#define FLASH_EEPROM_SIZE1 512
#define FLASH_EEPROM_ADDR1 FLASH_EEPROM_START_ADDR		
#define FLASH_EEPROM_ADDR2 FLASH_EEPROM_START_ADDR+512		
#define FLASH_EEPROM_ADDR3 FLASH_EEPROM_START_ADDR+1024		
#define FLASH_EEPROM_ADDR4 FLASH_EEPROM_START_ADDR+1536		


extern uint16_t flash_eeprom_buffer[];

void Flash_Eeprom_Test(void);

void Flash_Eeprom_Write(uint16_t NumToWrite);
void Flash_Eeprom_Read_Word(uint32_t ReadAddr,uint32_t *pBuffer,uint16_t NumToRead);
void Flash_Eeprom_Read_HalfWord(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);
void Flash_Eeprom_Read_Byte(uint32_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);

#endif
















