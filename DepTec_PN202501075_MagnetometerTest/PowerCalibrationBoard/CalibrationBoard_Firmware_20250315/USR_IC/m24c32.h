#ifndef __EP24LC16_H
#define	__EP24LC16_H
#include "misc.h"
uint8_t EEPROM_Read(I2C_HandleTypeDef *hi2c, uint16_t SlaveAdd, uint16_t MemAdd, uint8_t *pData, uint16_t Size);
uint8_t EEPROM_Write(I2C_HandleTypeDef *hi2c, uint16_t SlaveAdd, uint16_t MemAdd, uint8_t *pData, uint16_t Size);

#endif