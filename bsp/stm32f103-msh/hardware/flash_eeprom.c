#include "flash_eeprom.h"

uint16_t flash_eeprom_buffer[FLASH_EEPROM_SIZE1/2];

//FLASH写入数据测试
void Flash_Eeprom_Write(uint16_t NumToWrite)
{
	FLASH_EraseInitTypeDef flash_init;
	//设置PageError
	uint32_t PageError = 0;
	uint32_t writecounter;
	uint32_t p_address;
	//1、解锁FLASH
  HAL_FLASH_Unlock();
	
	//2、擦除FLASH
	//初始化FLASH_EraseInitTypeDef
	flash_init.TypeErase = FLASH_TYPEERASE_PAGES;
	flash_init.PageAddress = FLASH_EEPROM_ADDR1;
	flash_init.NbPages = 1;
	flash_init.Banks = 0;//only used in mass erase mode
		
	//调用擦除函数
	HAL_FLASHEx_Erase(&flash_init, &PageError);
	
	//3、对FLASH烧写
	p_address = FLASH_EEPROM_ADDR1;
	for(writecounter=0;writecounter<NumToWrite;writecounter++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, p_address, flash_eeprom_buffer[writecounter]);
		p_address+=2;
	}
	//4、锁住FLASH
  HAL_FLASH_Lock();
}


void Flash_Eeprom_Read_Word(uint32_t ReadAddr,uint32_t *pBuffer,uint16_t NumToRead)
{
	uint32_t readcounter;
	for(readcounter=0;readcounter<NumToRead;readcounter++,pBuffer++)
	{
		*pBuffer = *(__IO uint32_t*)(ReadAddr);
		ReadAddr+=4;
	}
}

void Flash_Eeprom_Read_HalfWord(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)
{
	uint32_t readcounter;
	for(readcounter=0;readcounter<NumToRead;readcounter++,pBuffer++)
	{
		*pBuffer = *(__IO uint16_t*)(ReadAddr);
		ReadAddr+=2;
	}
}

void Flash_Eeprom_Read_Byte(uint32_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	uint32_t readcounter;
	for(readcounter=0;readcounter<NumToRead;readcounter++,pBuffer++)
	{
		*pBuffer = *(__IO uint8_t*)(ReadAddr);
		ReadAddr++;
	}
}


void Flash_Eeprom_Test(void)
{
	uint32_t i;
	uint16_t readbuf[20];
	uint8_t readbuf2[20];
	for(i=0;i<20;i++)
		flash_eeprom_buffer[i] = i*2,readbuf[i]=0;
	
	Flash_Eeprom_Write(20);
	Flash_Eeprom_Read_HalfWord(FLASH_EEPROM_ADDR1,readbuf,20);
	Flash_Eeprom_Read_Byte(FLASH_EEPROM_ADDR1,readbuf2,20);
	
}
