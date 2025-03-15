#ifndef _APP_NET_H_
#define _APP_NET_H_

#include "w5500.h"
#include "board.h"

#define NET_PARAMETER_SIZE	23

#define LINK_LOSE_COUNT 	5000	//5s=5000ms

//TCP
#define   MODBUSTCP_FUNC_Pos   0
#define   MODBUSTCP_START_REG_H_Pos  1
#define   MODBUSTCP_START_REG_L_Pos  2
#define   MODBUSTCP_REG_LEN_H_Pos    3
#define   MODBUSTCP_REG_LEN_L_Pos    4
#define   MODBUSTCP_SETDATA_NUM_Pos  5
#define   MODBUSTCP_START_DATA_Pos   6

#define   MODBUSTCP_DATA_LEN_Pos     1
#define   MODBUSTCP_DATA_START_Pos   2

#define   MODBUSTCP_SET_SINGLE_DATA_H_Pos    3
#define   MODBUSTCP_SET_SINGLE_DATA_L_Pos    4

typedef struct _modbushead
{
  uint16_t Event_ID;
  uint16_t Protocol_ID;
  uint16_t Len;
  uint8_t Addr;
  uint8_t Func;
}T_MODBUS_HEAD;

typedef struct _eth_para
{
	volatile uint8_t Eth_Rst_Flag;		
	volatile uint8_t Link_Status;			
	volatile uint16_t Link_Count;			
}T_ETH_PARA;

#define	NET_RX_SIZE		1024
#define	NET_TX_SIZE		1024

void Load_Net_Parameters(uint8_t *pucNetPara);
void vNet_LinkLose_Count(void);
void vApp_Net_Init(uint8_t *pucNetPara);
void vApp_ModbusTCP_Process(void);
#endif