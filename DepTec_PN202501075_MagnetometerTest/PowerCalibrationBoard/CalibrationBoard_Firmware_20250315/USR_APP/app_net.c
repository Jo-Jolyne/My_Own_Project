#include "app_net.h"
#include "app_timer.h"
#include "app_modbus.h"

uint8_t ethRxBuffer[NET_RX_SIZE];	 
uint8_t ethTxBuffer[NET_TX_SIZE];		

uint16_t cmdSize=0;	

// T_ETH_RXDATA ethRxData;
T_ETH_PARA eth_para;

const uint8_t netConst[NET_PARAMETER_SIZE] = 
{
	192,168,2,1,						
	255,255,255,0,					
	0x54,0x05,0xDB,0x60,0xCA,0xE1,	
	192,168,2,199,					
	0x13,0x88,						
	0xA5, 0xAA, 0x03				
};

void Load_Net_Parameters(uint8_t *pucNetPara)
{
	uint8_t i = 0;
	uint16_t portData = 0;

	for(i=0;i<4;i++)
	{
		Sub_Mask[i] = netConst[4+i];
	}
	
	for(i=0;i<2;i++)
	{
		Phy_Addr[i] = netConst[8+i];
	}

	for(i=0;i<4;i++)
	{
		IP_Addr[i] = pucNetPara[i];
		Phy_Addr[i+2] = pucNetPara[i];
	}

	for(i=0;i<3;i++)
	{
		Gateway_IP[i] = IP_Addr[i];
	}

	Gateway_IP[3] = 1;

	portData =  (pucNetPara[4]<<8) + pucNetPara[5];

	for(i = 0; i < SOCK_NUM; i++)
	{
		S_Port[i] = portData;
		S_Mode[i] = TCP_SERVER;
	}
}

void vApp_Net_Init(uint8_t *pucNetPara)
{

	Load_Net_Parameters(pucNetPara);

	W5500_Hardware_Reset();

	W5500_Initialization();
}

void vNet_LinkLose_Count(void)
{
	if(eth_para.Link_Status==0)
	{
		eth_para.Link_Count++;
		if(eth_para.Link_Count >= LINK_LOSE_COUNT)
		{
			eth_para.Link_Count = LINK_LOSE_COUNT;
		}
	}
	else
	{
		eth_para.Link_Count = 0;
	}
}

void ModbusTCP_ACK_Error(SOCKET s, T_MODBUS_HEAD *pInData, uint8_t errcode)
{
	uint16_t tlen = 0;
	T_MODBUS_HEAD *pOutHeadData = (T_MODBUS_HEAD *)ethTxBuffer;
	uint8_t *pModbus_Out_Data = &(pOutHeadData->Func);

	memcpy(ethTxBuffer, pInData, sizeof(T_MODBUS_HEAD));

	pOutHeadData->Func |= 0x80;
	pModbus_Out_Data[1] = errcode;
	pOutHeadData->Len = u16EdianChange(2+1);		
	tlen = sizeof(T_MODBUS_HEAD) + 1;

	if(S_State[s] == (S_INIT|S_CONN))
	{
		S_Data[s] &= ~S_TRANSMITOK;
		Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
	}
}

void ModbusTCP_01_Process(SOCKET s, T_MODBUS_HEAD *pInData, uint16_t len)
{
	uint8_t ReturnCode = MODBUS_OK;
	uint16_t tlen = 0;
	T_MODBUS_HEAD *pOutHeadData = (T_MODBUS_HEAD *)ethTxBuffer;
	uint8_t *pModbus_In_Data = &(pInData->Func);
	uint8_t *pModbus_Out_Data = &(pOutHeadData->Func);
	uint16_t reg = (pModbus_In_Data[MODBUSTCP_START_REG_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_START_REG_L_Pos];
	uint16_t regLen = (pModbus_In_Data[MODBUSTCP_REG_LEN_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_REG_LEN_L_Pos];
    
	uint8_t i = 0, j = 0;
	uint16_t regIndex = 0;
	uint8_t	 byteIndex = 0;
	uint8_t  bitIndex = 0;

	if(len != 12)
	{
		ReturnCode = MODBUS_DAT_ERR;
	}
	else
	{
		ReturnCode = Modbus_Check_Reg_Data(0x01, reg, regLen);
	}

	if(MODBUS_OK == ReturnCode)
	{
		memcpy(pOutHeadData, pInData, sizeof(T_MODBUS_HEAD));
		tlen = sizeof(T_MODBUS_HEAD);

		i = regLen/8;
		if(0!=(regLen%8))
		{
			i += 1;
		}

		pModbus_Out_Data[MODBUSTCP_DATA_LEN_Pos] = i;
		tlen += 1;

		for(j=0; j<i; j++)
		{
			pModbus_Out_Data[MODBUSTCP_DATA_START_Pos + j] = 0;
		}

		for(j = 0; j < regLen; j++)
		{
			regIndex = (reg + j) / 16;
			byteIndex = ((reg + j) % 16)/8;	//1 - (((reg + j) % 16)/8);
			bitIndex = (reg + j) % 8;

			if(aucModbusData[APP_DO1_START_REG + regIndex][byteIndex] & (0x01 << bitIndex))
			{
				pModbus_Out_Data[MODBUSTCP_DATA_START_Pos + j/8] |= (0x01 << (j%8));
			}
		}

		tlen += i;

		pOutHeadData->Len = u16EdianChange(tlen-6);

		if(S_State[s] == (S_INIT|S_CONN))
		{
			S_Data[s] &= ~S_TRANSMITOK;
			Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
		}
	}
	else
	{
		ModbusTCP_ACK_Error(s, pInData, ReturnCode);
	}
}

void ModbusTCP_02_Process(SOCKET s, T_MODBUS_HEAD *pInData, uint16_t len)
{
	uint8_t ReturnCode = MODBUS_OK;
	uint16_t tlen = 0;
	T_MODBUS_HEAD *pOutHeadData = (T_MODBUS_HEAD *)ethTxBuffer;
	uint8_t *pModbus_In_Data = &(pInData->Func);
	uint8_t *pModbus_Out_Data = &(pOutHeadData->Func);
	uint16_t reg = (pModbus_In_Data[MODBUSTCP_START_REG_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_START_REG_L_Pos];
	uint16_t regLen = (pModbus_In_Data[MODBUSTCP_REG_LEN_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_REG_LEN_L_Pos];

	uint8_t i = 0, j = 0;
	uint16_t regIndex = 0;
	uint8_t	 byteIndex = 0;
	uint8_t  bitIndex = 0;

	if(len != 12)
	{
		ReturnCode = MODBUS_DAT_ERR;
	}
	else
	{
		ReturnCode = Modbus_Check_Reg_Data(0x02, reg, regLen);
	}
	if(MODBUS_OK == ReturnCode)
	{
		memcpy(pOutHeadData, pInData, sizeof(T_MODBUS_HEAD));
		tlen = sizeof(T_MODBUS_HEAD);
		
		i = regLen/8;
		if(0!=(regLen%8))
		{
			i += 1;
		}
        
		pModbus_Out_Data[MODBUSTCP_DATA_LEN_Pos] = i;
		tlen += 1;

		for(j=0; j<i; j++)
		{
			pModbus_Out_Data[MODBUSTCP_DATA_START_Pos + j] = 0;
		}
 		for(j = 0; j < regLen; j++)
		{
			regIndex = (reg + j) / 16;
			byteIndex = ((reg + j) % 16)/8;	//1 - (((reg + j) % 16)/8);
			bitIndex = (reg + j) % 8;
			if(aucModbusData[APP_DI_START_REG + regIndex][byteIndex] & (0x01 << bitIndex))
			{
				pModbus_Out_Data[MODBUSTCP_DATA_START_Pos + j/8] |= (0x01 << (j%8));
			}
		}
		tlen += i;
		pOutHeadData->Len = u16EdianChange(tlen-6);

		if(S_State[s] == (S_INIT|S_CONN))
		{
			S_Data[s] &= ~S_TRANSMITOK;
			Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
		}
	}
	else
	{
		ModbusTCP_ACK_Error(s, pInData, ReturnCode);
	}
}

void ModbusTCP_03_Process(SOCKET s, T_MODBUS_HEAD *pInData, uint16_t len)
{
	uint8_t ReturnCode = MODBUS_OK;
	uint16_t tlen = 0;
	T_MODBUS_HEAD *pOutHeadData = (T_MODBUS_HEAD *)ethTxBuffer;
	uint8_t *pModbus_In_Data = &(pInData->Func);
	uint8_t *pModbus_Out_Data = &(pOutHeadData->Func);
	uint16_t reg = (pModbus_In_Data[MODBUSTCP_START_REG_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_START_REG_L_Pos];
	uint16_t regLen = (pModbus_In_Data[MODBUSTCP_REG_LEN_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_REG_LEN_L_Pos];

	if(len != 12)
	{
		ReturnCode = MODBUS_DAT_ERR;
	}
	else
	{
		ReturnCode = Modbus_Check_Reg_Data(0x03, reg, regLen);
	}

	if(MODBUS_OK == ReturnCode)
	{
		memcpy(pOutHeadData, pInData, sizeof(T_MODBUS_HEAD));
		tlen = sizeof(T_MODBUS_HEAD);

		pModbus_Out_Data[MODBUSTCP_DATA_LEN_Pos] = regLen*2;
		tlen += 1;

		memcpy(&pModbus_Out_Data[MODBUSTCP_DATA_START_Pos], &aucModbusData[reg][0], regLen*2);
        
		tlen += regLen*2;

		pOutHeadData->Len = u16EdianChange(tlen-6);

		if(S_State[s] == (S_INIT|S_CONN))
		{
			S_Data[s] &= ~S_TRANSMITOK;
			Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
		}
	}
	else
	{
		ModbusTCP_ACK_Error(s, pInData, ReturnCode);
	}
}

void ModbusTCP_04_Process(SOCKET s, T_MODBUS_HEAD *pInData, uint16_t len)
{
	uint8_t ReturnCode = MODBUS_OK;
	uint16_t tlen = 0;
	T_MODBUS_HEAD *pOutHeadData = (T_MODBUS_HEAD *)ethTxBuffer;
	uint8_t *pModbus_In_Data = &(pInData->Func);
	uint8_t *pModbus_Out_Data = &(pOutHeadData->Func);
	uint16_t reg = (pModbus_In_Data[MODBUSTCP_START_REG_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_START_REG_L_Pos];
	uint16_t regLen = (pModbus_In_Data[MODBUSTCP_REG_LEN_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_REG_LEN_L_Pos];
	if(len != 12)
	{
		ReturnCode = MODBUS_DAT_ERR;
	}
	else
	{
		ReturnCode = Modbus_Check_Reg_Data(0x04, reg, regLen);
	}

	if(MODBUS_OK==ReturnCode)
	{
		for(uint8_t i = 0; i < regLen; i++)
		{
			ReturnCode = Modbus_Input_RegData_Process(reg + i);
			if(MODBUS_OK != ReturnCode)
			{
				break;
			}
		}
	}

	if(MODBUS_OK == ReturnCode)
	{
		memcpy(pOutHeadData, pInData, sizeof(T_MODBUS_HEAD));
		tlen = sizeof(T_MODBUS_HEAD);

		pModbus_Out_Data[MODBUSTCP_DATA_LEN_Pos] = regLen*2;
		tlen += 1;

		memcpy(&pModbus_Out_Data[MODBUSTCP_DATA_START_Pos], &aucModbusData[reg][0], regLen*2);
		tlen += regLen*2;

		pOutHeadData->Len = u16EdianChange(tlen-6);

		if(S_State[s] == (S_INIT|S_CONN))
		{
			S_Data[s] &= ~S_TRANSMITOK;
			Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
		}
	}
	else
	{
		ModbusTCP_ACK_Error(s, pInData, ReturnCode);
	}
}

void ModbusTCP_05_Process(SOCKET s, T_MODBUS_HEAD *pInData, uint16_t len)
{
	uint8_t ReturnCode = MODBUS_OK;
	uint16_t tlen = 0;
	T_MODBUS_HEAD *pOutHeadData = (T_MODBUS_HEAD *)ethTxBuffer;
	uint8_t *pModbus_In_Data = &(pInData->Func);
	uint16_t reg = (pModbus_In_Data[MODBUSTCP_START_REG_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_START_REG_L_Pos];
	uint16_t TempData = (pModbus_In_Data[MODBUSTCP_SET_SINGLE_DATA_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_SET_SINGLE_DATA_L_Pos];
	
	if(len != 12)
	{
		ReturnCode = MODBUS_DAT_ERR;
	}
	else
	{
		ReturnCode = Modbus_Check_Reg_Data(0x05, reg, 0);
	}

	if(MODBUS_OK == ReturnCode)
	{
		if(TempData==0x0000)
		{
			ReturnCode = Modbus_Coil_Set(reg, 0);
		}
		else if(TempData==0xFF00)
		{
			ReturnCode = Modbus_Coil_Set(reg, 1);
		}
		else
		{
			ReturnCode = MODBUS_DAT_ERR;	
		}
        
		if(MODBUS_OK == ReturnCode)
		{
			memcpy(pOutHeadData, pInData, 12);
			tlen = 12;
			if(S_State[s] == (S_INIT|S_CONN))
			{
				S_Data[s] &= ~S_TRANSMITOK;
				Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
			}
		}
		else
		{
			ModbusTCP_ACK_Error(s, pInData, ReturnCode);
		}
	}
	else
	{
		ModbusTCP_ACK_Error(s, pInData, ReturnCode);
	}
}

void ModbusTCP_06_Process(SOCKET s, T_MODBUS_HEAD *pInData, uint16_t len)
{
	uint8_t ReturnCode = MODBUS_OK;
	uint16_t tlen = 0;
	T_MODBUS_HEAD *pOutHeadData = (T_MODBUS_HEAD *)ethTxBuffer;
	uint8_t *pModbus_In_Data = &(pInData->Func);
	uint16_t reg = (pModbus_In_Data[MODBUSTCP_START_REG_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_START_REG_L_Pos];
	
	if(len != 12)
	{
		ReturnCode = MODBUS_DAT_ERR;
	}
	else
	{
		ReturnCode = Modbus_Check_Reg_Data(0x06, reg, 0);
	}

	if(MODBUS_OK == ReturnCode)
	{
		ReturnCode = Modbus_Hold_RegData_Process(reg, &pModbus_In_Data[MODBUSTCP_SET_SINGLE_DATA_H_Pos]);	
		
		if(MODBUS_OK == ReturnCode)
		{
			memcpy(&aucModbusData[reg][0], &pModbus_In_Data[MODBUSTCP_SET_SINGLE_DATA_H_Pos], 2);
			memcpy(pOutHeadData, pInData, 12);
			tlen = 12;
			if((RUN_MODE_REG==reg)&&(START_BOOTLOADER==aucModbusData[RUN_MODE_REG][0]))
			{
				aucModbusData[UPDATE_TYPE_REG][0] = UPDATA_2_NET;
				ReturnCode = ucWrite_BootParagram((UPDATE_TYPE_REG - DEV_ADDR_REG)*2 + 1, &aucModbusData[UPDATE_TYPE_REG][0]);
				
				if(OK==ReturnCode)
				{
					if(S_State[s] == (S_INIT|S_CONN))
					{
						S_Data[s] &= ~S_TRANSMITOK;
						Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
					}
					HAL_Delay(5);
					HAL_NVIC_SystemReset();
				}
				else
				{
					ModbusTCP_ACK_Error(s, pInData, ReturnCode);
				}
			}
			else if(SYS_RST_REG==reg)
			{
				if(S_State[s] == (S_INIT|S_CONN))
				{
					S_Data[s] &= ~S_TRANSMITOK;
					Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
				}
				HAL_Delay(50);
				HAL_NVIC_SystemReset();
			}
			else
			{
				if(S_State[s] == (S_INIT|S_CONN))
				{
					S_Data[s] &= ~S_TRANSMITOK;
					Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
				}
			}
		}
		else
		{
			ModbusTCP_ACK_Error(s, pInData, ReturnCode);
		}
	}
	else
	{
		ModbusTCP_ACK_Error(s, pInData, ReturnCode);
	}
}

void ModbusTCP_0F_Process(SOCKET s, T_MODBUS_HEAD *pInData, uint16_t len)
{
	uint8_t ReturnCode = MODBUS_OK;
	uint16_t tlen = 0;
	T_MODBUS_HEAD *pOutHeadData = (T_MODBUS_HEAD *)ethTxBuffer;
	uint8_t *pModbus_In_Data = &(pInData->Func);

	uint16_t reg = (pModbus_In_Data[MODBUSTCP_START_REG_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_START_REG_L_Pos];
	uint16_t regLen = (pModbus_In_Data[MODBUSTCP_REG_LEN_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_REG_LEN_L_Pos];
	uint8_t	DataNum = pModbus_In_Data[MODBUSTCP_SETDATA_NUM_Pos];

	uint8_t i = 0;
	uint8_t  bitIndex = 0;
	uint8_t TempValue = 0;

	if((len != (13+DataNum)) || (DataNum!=(regLen/8 + ((regLen%8)?1:0))))	//13:head(8) + startreg(2) + reglen(2)+datanum(1)+data
	{
		ReturnCode = MODBUS_DAT_ERR;
	}
	else
	{
		ReturnCode = Modbus_Check_Reg_Data(0x0F, reg, regLen);
	}

	if(MODBUS_OK == ReturnCode)
	{
		for(i = 0; i < regLen; i++)
		{
			bitIndex = i % 8;
			TempValue = (pModbus_In_Data[MODBUSTCP_START_DATA_Pos + i/8] & (0x01<<bitIndex)) >> bitIndex;
			ReturnCode = Modbus_Coil_Set(reg + i, TempValue);
			if(MODBUS_OK != ReturnCode)
			{
				break;
			}
   		}
        
		if(MODBUS_OK == ReturnCode)
		{
			memcpy(pOutHeadData, pInData, sizeof(T_MODBUS_HEAD) + 4);//4:Reg(2)+reglen(2)
			tlen = sizeof(T_MODBUS_HEAD) + 4;

			pOutHeadData->Len = u16EdianChange(tlen-6);
			if(S_State[s] == (S_INIT|S_CONN))
			{
				S_Data[s] &= ~S_TRANSMITOK;
				Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
			}
		}
		else
		{
			ModbusTCP_ACK_Error(s, pInData, ReturnCode);
		}
	}
	else
	{
		ModbusTCP_ACK_Error(s, pInData, ReturnCode);
	}
}


void ModbusTCP_10_Process(SOCKET s, T_MODBUS_HEAD *pInData, uint16_t len)
{
	uint8_t ReturnCode = MODBUS_OK;
	uint16_t tlen = 0;
	T_MODBUS_HEAD *pOutHeadData = (T_MODBUS_HEAD *)ethTxBuffer;
	uint8_t *pModbus_In_Data = &(pInData->Func);

	uint16_t reg = (pModbus_In_Data[MODBUSTCP_START_REG_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_START_REG_L_Pos];
	uint16_t regLen = (pModbus_In_Data[MODBUSTCP_REG_LEN_H_Pos] << 8) | pModbus_In_Data[MODBUSTCP_REG_LEN_L_Pos];
	uint8_t	DataNum = pModbus_In_Data[MODBUSTCP_SETDATA_NUM_Pos];

	uint8_t i = 0;

	if((len != (13+DataNum)) || (DataNum!=(regLen*2)))	//13:head(8) + startreg(2) + reglen(2)+datanum(1)+data
	{
		ReturnCode = MODBUS_DAT_ERR;
	}
	else
	{
		ReturnCode = Modbus_Check_Reg_Data(0x10, reg, regLen);
	}

	if(MODBUS_OK == ReturnCode)
	{
		for(i = 0; i < regLen; i++)
		{
			ReturnCode = Modbus_Hold_RegData_Process(reg + i, &pModbus_In_Data[MODBUSTCP_START_DATA_Pos + 2*i]);
			if(MODBUS_OK != ReturnCode)
			{
				break;
			}
			else
			{
				memcpy(&aucModbusData[reg + i][0], &pModbus_In_Data[MODBUSTCP_START_DATA_Pos + 2*i], 2);
			}
		}

		if(MODBUS_OK == ReturnCode)
		{
			memcpy(pOutHeadData, pInData, sizeof(T_MODBUS_HEAD) + 4);//4:Reg(2)+reglen(2)
			tlen = sizeof(T_MODBUS_HEAD) + 4;

			pOutHeadData->Len = u16EdianChange(tlen-6);

			if((reg <= RUN_MODE_REG) && ((reg+regLen) > RUN_MODE_REG) && (START_BOOTLOADER==aucModbusData[RUN_MODE_REG][0]))
			{
				aucModbusData[UPDATE_TYPE_REG][0] = UPDATA_2_NET;
				ReturnCode = ucWrite_BootParagram((UPDATE_TYPE_REG - DEV_ADDR_REG)*2 + 1, &aucModbusData[UPDATE_TYPE_REG][0]);
				
				if(OK==ReturnCode)
				{
					if(S_State[s] == (S_INIT|S_CONN))
					{
						S_Data[s] &= ~S_TRANSMITOK;
						Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
					}
					HAL_Delay(5);
					HAL_NVIC_SystemReset();
				}
				else
				{
					ModbusTCP_ACK_Error(s, pInData, ReturnCode);
				}
			}
			else
			{
				if(S_State[s] == (S_INIT|S_CONN))
				{
					S_Data[s] &= ~S_TRANSMITOK;
					Write_SOCK_Data_Buffer(s, (uint8_t *)pOutHeadData, tlen);
				}
			}
		}
		else
		{
			ModbusTCP_ACK_Error(s, pInData, ReturnCode);
		}
	}
	else
	{
		ModbusTCP_ACK_Error(s, pInData, ReturnCode);
	}
}

void Socket_Data_Process(int s)
{
	uint16_t i;
	static uint16_t ethRxSize[SOCK_NUM]={0};					
	uint16_t tmpSize[SOCK_NUM]={0};								
	T_MODBUS_HEAD *pModbus_TCP_Head;				
	//uint8_t *pModbus_In_Data;	

	if((S_Data[s] & S_RECEIVE) == S_RECEIVE)		
	{
		S_Data[s] &= ~S_RECEIVE;
		tmpSize[s] = Read_SOCK_Data_Buffer(s, ethRxBuffer+ethRxSize[s]);	
		ethRxSize[s] += tmpSize[s];
	}
	else
	{
		if(S_State[s] == (S_INIT|S_CONN))
		{
			S_Data[s] &= ~S_TRANSMITOK;

			if(ethRxSize[s] > 0)							
			{
				if(ethRxSize[s] > sizeof(T_MODBUS_HEAD))
				{
					pModbus_TCP_Head = (T_MODBUS_HEAD *)ethRxBuffer;
					cmdSize = u16EdianChange(pModbus_TCP_Head->Len) + 6;	//6:seqh/seql proctol h/l=len_hi len_low
					if(cmdSize <= ethRxSize[s])
					{
						if(pModbus_TCP_Head->Addr==MODBUS_TCP_ADDR)
						{
							switch (pModbus_TCP_Head->Func)
							{
								case 0x01:
									ModbusTCP_01_Process(s, pModbus_TCP_Head, cmdSize);
									break;
								
								case 0x02:
									ModbusTCP_02_Process(s, pModbus_TCP_Head, cmdSize);
									break;

								case 0x03:
									ModbusTCP_03_Process(s, pModbus_TCP_Head, cmdSize);
									break;

								case 0x04:
									ModbusTCP_04_Process(s, pModbus_TCP_Head, cmdSize);
									break;

								case 0x05:
									ModbusTCP_05_Process(s, pModbus_TCP_Head, cmdSize);
									break;

								case 0x06:
									ModbusTCP_06_Process(s, pModbus_TCP_Head, cmdSize);
									break;

								case 0x0F:
									ModbusTCP_0F_Process(s, pModbus_TCP_Head, cmdSize);
									break;

								case 0x10:
									ModbusTCP_10_Process(s, pModbus_TCP_Head, cmdSize);
									break;

								default:
									ModbusTCP_ACK_Error(s, pModbus_TCP_Head, 																MODBUS_FUN_ERR);
									break;
							}
						}
					}
					else
					{
						ethRxSize[s]=0;
						cmdSize=0;
					}
				}
				else
				{
					ethRxSize[s]=0;
					cmdSize=0;
				}

				if(ethRxSize[s] >= cmdSize)
				{
					ethRxSize[s] -= cmdSize;							
				}
				else
				{
					ethRxSize[s]=0;
					cmdSize=0;
				}

				if(ethRxSize[s]==0)
				{
					for(i=0; i<NET_RX_SIZE; i++)
					{
						ethRxBuffer[i] = 0;						
					}
				}
				else
				{
					for(i=0;i<ethRxSize[s];i++)
					{
						ethRxBuffer[i] = ethRxBuffer[cmdSize+i];		
					}
				}
			}
		}
	}
}

void vApp_ModbusTCP_Process(void)
{
	static uint8_t linkFlag = 0;
	uint8_t i = 0;

	//network link check
	eth_para.Link_Status = W5500_Link_Detect();

	if(eth_para.Link_Count >= (LINK_LOSE_COUNT/2))
	{
		if(0==linkFlag)
		{
			for(i=0; i<SOCK_NUM; i++)
			{
				S_State[i] = 0;
				S_Data[i] = 0;
			}
			W5500_Initialization();	
			linkFlag = 1;
		}
	}
	else if(0==eth_para.Link_Count)
	{
		linkFlag = 0;
	}

	W5500_Socket_Set();
	W5500_Interrupt_Process();

	for(i = 0; i < SOCK_NUM; i++)
	{
		Socket_Data_Process(i);
	}
}   


