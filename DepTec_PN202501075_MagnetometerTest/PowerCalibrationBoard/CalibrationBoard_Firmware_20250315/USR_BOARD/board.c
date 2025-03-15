#include "board.h"
const char aucFWVerion[] = {"A101"};

#define	 ADDR_RESET_COUNT	2000
uint16_t usAddrKeyCount = 0;
uint8_t  ucAddrKeyState = 0;
BOOL 	xAddrResetFlag = FALSE;

MCU_GPIO_CTRL MCU_IO_CTRL[6] = 
{
	{MCU_CTRL_DMM_VOL_GPIO_Port,MCU_CTRL_DMM_VOL_Pin},
	{MCU_CTRL_DMM_CUR_GPIO_Port,MCU_CTRL_DMM_CUR_Pin},
	{MCU_RL_CTRL_RES1_GPIO_Port,MCU_RL_CTRL_RES1_Pin},
	{MCU_RL_CTRL_RES2_GPIO_Port,MCU_RL_CTRL_RES2_Pin},
	{MCU_RL_CTRL_RES3_GPIO_Port,MCU_RL_CTRL_RES3_Pin},
    {MCU_RL_CTRL_RES4_GPIO_Port,MCU_RL_CTRL_RES4_Pin}
};

void vBoard_Init(void)
{
	LED_RUN_OFF;
}

int8_t ucWrite_BootParagram(uint16_t usParaMemBase, uint8_t *pucParaData)
{
  int8_t ucRetCode = OK;

  ucRetCode = EEPROM_Write(&hi2c1, DEV_I2C_ADD, usParaMemBase, pucParaData, 2);

  return ucRetCode;
}

int8_t ucLoad_DevPara(uint8_t *pucLoadPara, const uint8_t *pucInitPara, const uint16_t usParaLen)
{
  	uint8_t i = 0;
  	uint8_t ucRetCode = OK;
	BOOL ucHasParaFlag = FALSE;
	// uint8_t aucTempData[usParaLen + 1] = 0;
	uint8_t *pucTempData = malloc(usParaLen + 1);	
	if(NULL==pucTempData)
	{
		return ERR;
	}

	for(i = 0; i < 3; i++)
	{
		if(OK==EEPROM_Read(&hi2c1, DEV_I2C_ADD, DEV_PARA_FLAG_BASE, &pucTempData[0], usParaLen + 1))
		{
			if(pucTempData[0]==DEV_PARAM_FLAG)
			{
				memcpy(pucLoadPara, &pucTempData[1], usParaLen);
				ucHasParaFlag = TRUE;
				break;
			}
		}
		HAL_Delay(10);
		ucHasParaFlag = FALSE;
	}

	if(FALSE==ucHasParaFlag)
	{
		pucTempData[0] = DEV_PARAM_FLAG;
		memcpy(&pucTempData[1], &pucInitPara[0], usParaLen);
		ucRetCode = EEPROM_Write(&hi2c1, DEV_I2C_ADD, DEV_PARA_FLAG_BASE, &pucTempData[0], usParaLen + 1);

		memcpy(&pucLoadPara[0], &pucInitPara[0], usParaLen);
	}

	free(pucTempData);
	pucTempData = NULL;

  	return ucRetCode;
}

int8_t ucLoad_SysPara(uint8_t *pucLoadPara, const uint8_t *pucInitPara, const uint16_t usParaLen)
{
  	uint8_t i = 0;
  	uint8_t ucRetCode = OK;
	BOOL ucHasParaFlag = FALSE;
	// uint8_t aucTempData[usParaLen + 1] = 0;
	uint8_t *pucTempData = malloc(usParaLen + 1);	
	if(NULL==pucTempData)
	{
		return ERR;
	}

	for(i = 0; i < 3; i++)
	{
		if(OK==EEPROM_Read(&hi2c1, DEV_I2C_ADD, SYS_PARA_FLAG_BASE, pucTempData, usParaLen + 1))
		{
			if(pucTempData[0]==SYS_PARAM_FLAG)
			{
				memcpy(pucLoadPara, &pucTempData[1], usParaLen);
				ucHasParaFlag = TRUE;
				break;
			}
		}
		HAL_Delay(10);
		ucHasParaFlag = FALSE;
	}
	if(FALSE==ucHasParaFlag)
	{
		pucTempData[0] = SYS_PARAM_FLAG;
		memcpy(&pucTempData[1], pucInitPara, usParaLen);    
		ucRetCode = EEPROM_Write(&hi2c1, DEV_I2C_ADD, SYS_PARA_FLAG_BASE, pucTempData, usParaLen + 1);

		memcpy(pucLoadPara, pucInitPara, usParaLen);
	}
	free(pucTempData);
	pucTempData = NULL;

  	return ucRetCode;
}


void Addr_Reset_Check(void)
{
	uint8_t ucTemp = HAL_GPIO_ReadPin(ADDRESS_KEY_GPIO_Port, ADDRESS_KEY_Pin);

	if(ucAddrKeyState==ucTemp)
	{
		usAddrKeyCount++;
		if(usAddrKeyCount >= ADDR_RESET_COUNT)	
		{
			usAddrKeyCount = ADDR_RESET_COUNT;
			if(GPIO_PIN_RESET==ucTemp)		
			{
				xAddrResetFlag = TRUE;	
			}
			else
			{
				xAddrResetFlag = FALSE;
			}
		}
	}
	else
	{
		usAddrKeyCount = 0;
	}
	
	ucAddrKeyState = ucTemp;
}

int8_t ucDev_Para_Reset(const uint8_t *pucInitPara, const uint16_t usParaLen)
{
	uint8_t ucRetCode = OK;
    
	ucRetCode = EEPROM_Write(&hi2c1, DEV_I2C_ADD, DEV_ADDR_BASE, (uint8_t *)pucInitPara, usParaLen);

	return ucRetCode;
}

uint8_t ucBoard_OUTPUT_Ctrl(uint8_t num, uint8_t state)
{
	uint8_t ReturnCode = OK;
	if (num < 6)
	{
		HAL_GPIO_WritePin(MCU_IO_CTRL[num].port, MCU_IO_CTRL[num].pin, (GPIO_PinState)(state));
	}
	else
	{
		ReturnCode = ERR;	
	}
	return ReturnCode;
}


