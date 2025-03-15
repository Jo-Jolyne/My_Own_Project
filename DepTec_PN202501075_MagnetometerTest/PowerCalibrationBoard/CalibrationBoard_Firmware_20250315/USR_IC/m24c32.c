#include "m24c32.h"

#define EP24LC32 1
// #define EP24LC16 1
// #define EP24LC08 1

#ifdef EP24LC16
	#define MEMMACRO 0x0700
#elif EP24LC08
	#define MEMMACRO 0x0300
#endif

#ifdef EP24LC32

uint8_t EEPROM_Write(I2C_HandleTypeDef *hi2c, uint16_t SlaveAdd, uint16_t MemAdd, uint8_t *pData, uint16_t Size)
{
	uint16_t i = 0;

	for(i = 0; i < Size; i++)
	{
		if(HAL_OK != HAL_I2C_Mem_Write(hi2c, SlaveAdd, MemAdd, I2C_MEMADD_SIZE_16BIT, pData, 1, 1000))
		{
			return IIC_ERROR;
		}
		
		MemAdd++;
		pData++;
		
		HAL_Delay(5);
	}
	
	return OK;
}	

uint8_t EEPROM_Read(I2C_HandleTypeDef *hi2c, uint16_t SlaveAdd, uint16_t MemAdd, uint8_t *pData, uint16_t Size)
{
	uint16_t i = 0;

	for(i = 0; i < Size; i++)
	{
		if(HAL_OK != HAL_I2C_Mem_Read(hi2c, SlaveAdd, MemAdd,I2C_MEMADD_SIZE_16BIT, pData, 1, 1000))
		{
			return IIC_ERROR;
		}
		MemAdd++;
		pData++;
	}
	
	return OK;
}
#else
uint8_t EEPROM_Write(I2C_HandleTypeDef *hi2c, uint16_t SlaveAdd, uint16_t MemAdd, uint8_t *pData, uint16_t Size)
{
	uint16_t i = 0;
	uint8_t devaddr_temp=SlaveAdd|((MemAdd&MEMMACRO)>>7);
	uint8_t memaddr_temp=MemAdd&0x00FF;

	for(i = 0; i < Size; i++)
	{
		if(HAL_OK != HAL_I2C_Mem_Write(hi2c, devaddr_temp, memaddr_temp,I2C_MEMADD_SIZE_8BIT, pData, 1, 200))
		{
			return IIC_ERROR;
		}
		
		MemAdd++;
		pData++;

		devaddr_temp=SlaveAdd|((MemAdd&MEMMACRO)>>7);
		memaddr_temp=MemAdd&0x00FF;
		
		HAL_Delay(10);
	}
	
	return OK;
}

uint8_t EEPROM_Read(I2C_HandleTypeDef *hi2c, uint16_t SlaveAdd, uint16_t MemAdd, uint8_t *pData, uint16_t Size)
{
	uint16_t i = 0;
	uint8_t devaddr_temp=SlaveAdd|((MemAdd&MEMMACRO)>>7);
	uint8_t memaddr_temp=MemAdd&0x00FF;

	for(i = 0; i < Size; i++)
	{
		if(HAL_OK != HAL_I2C_Mem_Read(hi2c, devaddr_temp, memaddr_temp,I2C_MEMADD_SIZE_8BIT, pData, 1, 200))
		{
			return IIC_ERROR;
		}
        
		MemAdd++;
		pData++;

		devaddr_temp=SlaveAdd|((MemAdd&MEMMACRO)>>7);
		memaddr_temp=MemAdd&0x00FF;
	}
	
	return OK;
}
#endif


