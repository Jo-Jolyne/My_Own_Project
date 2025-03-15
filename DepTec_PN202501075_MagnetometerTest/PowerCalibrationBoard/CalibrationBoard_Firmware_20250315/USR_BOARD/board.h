#ifndef __BOARD_H_
#define __BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "misc.h"

#define   LED_RUN_ON      HAL_GPIO_WritePin(LED_RUN_GPIO_Port, LED_RUN_Pin, GPIO_PIN_SET)
#define   LED_RUN_OFF     HAL_GPIO_WritePin(LED_RUN_GPIO_Port, LED_RUN_Pin, GPIO_PIN_RESET)
#define   LED_RUN_BLINK   HAL_GPIO_TogglePin(LED_RUN_GPIO_Port, LED_RUN_Pin)

#define	  DEV_I2C_ADD			  	0xA0
#define	  DEV_PARA_FLAG_BASE		0
#define	  DEV_PARAM_FLAG		    0x55
#define	  DEV_ADDR_BASE		 	    1
#define   IP_START_BASE             3
#define   NET_PORT_BASE             5
#define   SYS_PARA_FLAG_BASE        100
#define	  SYS_PARAM_FLAG		    0x55
#define   MAX_INPUT_STAT_NUM        16
#define   MAX_COIL_STAT_NUM         32
#define   ucBoard_DO_Single_Set     ucBoard_OUTPUT_Ctrl 

typedef struct IO_CTRL
{
	GPIO_TypeDef     *port;
 	uint16_t         pin;
}MCU_GPIO_CTRL;

extern const char aucFWVerion[];
extern BOOL xAddrResetFlag;

void vBoard_Init(void);
int8_t ucLoad_DevPara(uint8_t *pucLoadPara, const uint8_t *pucInitPara, const uint16_t 						usParaLen);
int8_t ucLoad_SysPara(uint8_t *pucLoadPara, const uint8_t *pucInitPara, const uint16_t 						usParaLen);
int8_t ucWrite_BootParagram(uint16_t usParaMemBase, uint8_t *pucParaData);
void Addr_Reset_Check(void);
int8_t ucDev_Para_Reset(const uint8_t *pucInitPara, const uint16_t usParaLen);
// uint8_t ucBoard_DO_Single_Set(uint16_t num, uint8_t state);
uint8_t ucBoard_OUTPUT_Ctrl(uint8_t num, uint8_t state);
#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */  


