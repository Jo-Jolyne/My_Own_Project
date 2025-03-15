#include "app_modbus.h"
#include "app_net.h"
#include "board.h"

uint8_t aucModbusData[APP_MAX_REG_ADD][2];	//modbus

const T_BOOT_PARA DefaultBootPara = 
{
	.UartADDR = 1,
	.IP1 = 192,
	.IP2 = 168,
	.IP3 = 2,
	.IP4 = 199,
	.PortH = 0x13,
	.PortL = 0x88,
	.RunMode = START_APP,
	.ID		= 0,
	.UpdateType = UPDATA_2_UART,
	.EraseFlagH = 0,
	.EraseFlagL = 0,
	.CheckFlashLen = 0,
	.RSV1 = 0,
	.RSV2 = 0,
	.RSV3 = 0,
	.RSV4 = 0,
	.RSV5 = 0
};

void vApp_Modbus_DataInit(void)
{
	vBoard_Init();
	//添加波特率，停止位，校验位
	if(OK!=ucLoad_DevPara(&aucModbusData[DEV_ADDR_REG][0], (const uint8_t *)&DefaultBootPara, sizeof(T_BOOT_PARA)))
	{
		
	}
	//Verion
	aucModbusData[HARD_VER_REG][0] = aucFWVerion[0];
	aucModbusData[HARD_VER_REG][1] = aucFWVerion[1];
	aucModbusData[SOFT_VER_REG][0] = aucFWVerion[2];
	aucModbusData[SOFT_VER_REG][1] = aucFWVerion[3];
}

void vApp_DevPara_ResetCheck(void)
{
	uint8_t i = 0;
	static uint8_t rstFlag = 0;

	if(TRUE==xAddrResetFlag)
	{
		if(0==rstFlag)
		{
			rstFlag = 1;
			ucDev_Para_Reset((const uint8_t *)&DefaultBootPara, 8);
			// aucModbusData[DEV_ADDR_REG][0] = DefaultBootPara.UartADDR;
			memcpy(&aucModbusData[DEV_ADDR_REG][0], (uint8_t *)&DefaultBootPara, 8);
			for(i=0;i<SOCK_NUM;i++)
			{
				S_State[i] = 0;
				S_Data[i] = 0;
			}
			Load_Net_Parameters(&aucModbusData[IP_START_REG][0]);

			W5500_Hardware_Reset();

			W5500_Initialization();
		}
	}
	else
	{
		rstFlag = 0;
	}
}

uint8_t Modbus_Input_RegData_Process(uint16_t RegAddr)
{
	uint8_t ReturnCode = MODBUS_OK;
	float fData1= 0.0f;
	float fData2= 0.0f;

	switch (RegAddr)
	{
		case APP_DI_START_REG://0x10
			
			break;

		default:
			break;
	}
	
	return ReturnCode;
}

uint8_t Modbus_Hold_RegData_Process(uint16_t RegAddr, uint8_t *pData)
{
	uint8_t ReturnCode = MODBUS_OK;
	uint32_t ulTempData = 0;
	uint32_t Get_baudrate = 0; 
	uint16_t IO_Index1 = 0;
	uint16_t IO_Index2 = 0;

	switch (RegAddr)
	{
		case DEV_ADDR_REG://0x04
			if((pData[0] > 0) && (pData[0] < 0xFF))
			{
				ReturnCode = ucWrite_BootParagram((RegAddr - DEV_ADDR_REG)*2 + 1, pData);
			}
			else
			{
				ReturnCode = MODBUS_DAT_ERR;
			}			
			break;

		case IP_START_REG://0x05
		case IP_START_REG + 1://0x06
			if((0==pData[0]) || (0==pData[1]))
			{
				ReturnCode = MODBUS_DAT_ERR;
			}
			else
			{
				ReturnCode = ucWrite_BootParagram((RegAddr - DEV_ADDR_REG)*2 + 1, pData);
			}
			break;

		case PORT_REG://0x07
			ulTempData = (pData[0] << 8) + pData[1];
			if(ulTempData < 1000)
			{
				ReturnCode = MODBUS_DAT_ERR;
			}
			else
			{
				ReturnCode = ucWrite_BootParagram((RegAddr - DEV_ADDR_REG)*2 + 1, pData);
			}
			break;

		case RUN_MODE_REG://0x09
			if((START_APP==pData[0]) || (START_BOOTLOADER==pData[0]))
			{
				ReturnCode = ucWrite_BootParagram((RegAddr - DEV_ADDR_REG)*2 + 1, pData);
			}
			else
			{
				ReturnCode =  MODBUS_DAT_ERR;
			}
			break;

		case UPDATE_TYPE_REG:
			if((UPDATA_2_UART==pData[0]) || (UPDATA_2_NET==pData[0]))
			{
				ReturnCode = ucWrite_BootParagram((RegAddr - DEV_ADDR_REG)*2 + 1, pData);
			}
			else
			{
				return MODBUS_DAT_ERR;
			}
		break;

		case SYS_RST_REG://0x0F
			if((0x55==pData[0]) && (0xAA==pData[1]))
			{
				ReturnCode = MODBUS_OK;
			}
			else
			{
				return MODBUS_DAT_ERR;
			}
			break;

		case MCU_CTRL_DMM_VOL:
		case MCU_CTRL_DMM_CUR:
			if ((pData[0] == 0x00) || (pData[0] == 0x01))
			{
				ucBoard_OUTPUT_Ctrl((RegAddr - MCU_CTRL_DMM_VOL),(GPIO_PinState)pData[0]);
			}
			else
			{
				ReturnCode =  MODBUS_DAT_ERR;
			}
			break;

		case MCU_RL_CTRL_RES:
			if(pData[0] > 4) 
			{
				ReturnCode = MODBUS_DAT_ERR;
			}
			else 
			{
				for(int i = 0; i < 4; i++) 
				{
					ucBoard_OUTPUT_Ctrl(i+2,GPIO_PIN_RESET);
				}
				if(pData[0] > 0) 
				{
					ucBoard_OUTPUT_Ctrl((pData[0]+1),GPIO_PIN_SET);
				}
			}
			break;

		case MCU_CTRL_ALLIO://测试用
			if ((pData[0] == 0x00) || (pData[0] == 0x01))
			{
				for (size_t i = 0; i < 6; i++)
				{
					ucBoard_OUTPUT_Ctrl(i,(GPIO_PinState)pData[0]);
				}
			}
			else
			{
				ReturnCode =  MODBUS_DAT_ERR;
			}
			break;
        default:
        	break;
	}
	return ReturnCode;
}

uint8_t Modbus_Coil_Set(uint16_t reg, uint8_t data)
{
	uint8_t ReturnCode = MODBUS_OK;
	uint16_t regIndex = 0;
	uint8_t  byteIndex = 0;
	uint8_t  bitIndex = 0;

	regIndex = reg / 16;
	byteIndex = (reg % 16)/8;//1 - ((reg % 16)/8);
	bitIndex = reg % 8;

	if(data==0x00)
	{
		ucBoard_DO_Single_Set(reg, 0);
		aucModbusData[APP_DO1_START_REG + regIndex][byteIndex] &= ~(0x01 << bitIndex);
	}
	else if(data==0x01)
	{
		ucBoard_DO_Single_Set(reg, 1);
		aucModbusData[APP_DO1_START_REG + regIndex][byteIndex] |= (0x01 << bitIndex);
	}
	else
	{
		ReturnCode = MODBUS_DAT_ERR;	
	}

	return ReturnCode;
}

uint8_t Modbus_Check_Reg_Data(uint8_t func, uint16_t reg, uint16_t regLen)
{
	uint8_t ReturnCode = MODBUS_OK; 
	
	switch (func)
	{
	case 0x01:
	case 0x05:
	case 0x0F:
		if((reg >= MAX_COIL_STAT_NUM) || ((reg + regLen) > MAX_COIL_STAT_NUM))	
		{
			return MODBUS_ADD_ERR;
		}
	break;

	case 0x02:
	if((reg >= MAX_INPUT_STAT_NUM) || ((reg + regLen) > MAX_INPUT_STAT_NUM)) 
	{
		return MODBUS_ADD_ERR;
	}
	break;

	case 0x04:
		if(reg > BOOT_MAX_INPUT_REG_ADD)
		{
			if(reg < APP_MIN_INPUT_REG_ADD)
			{
				ReturnCode = MODBUS_ADD_ERR;
			}
			else if((reg > APP_MAX_INPUT_REG_ADD) || ((reg + regLen) > (APP_MAX_INPUT_REG_ADD + 1)))	
			{
				ReturnCode = MODBUS_ADD_ERR;
			}
		}
		else if((reg + regLen) > (BOOT_MAX_INPUT_REG_ADD + 1))	
		{
			ReturnCode = MODBUS_ADD_ERR;
		}
	break;

	case 0x03:
	case 0x06:
	case 0x10:
		if(reg > BOOT_MAX_HOLD_REG_ADD)
		{
			if(reg < APP_MIN_HOLD_REG_ADD)
			{
				ReturnCode = MODBUS_ADD_ERR;
			}
			else if((reg > APP_MAX_HOLD_REG_ADD) || ((reg + regLen) > (APP_MAX_HOLD_REG_ADD + 1)))	
			{
				ReturnCode = MODBUS_ADD_ERR;
			}
		}
		else if(((reg + regLen) > (BOOT_MAX_HOLD_REG_ADD + 1)) || (reg < BOOT_MIN_HOLD_REG_ADD)) 
		{
			ReturnCode = MODBUS_ADD_ERR;
		}
		break;
	default:
		ReturnCode = MODBUS_FUN_ERR;	//Invalid Function
		break;
	}
	return ReturnCode;
}





