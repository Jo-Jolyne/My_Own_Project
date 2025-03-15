#ifndef	_APP_MODBUS_H_
#define _APP_MODBUS_H_

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

#include "misc.h"
#include "board.h"

//for BootLoader Para
#define START_BOOTLOADER           0x5A	
#define START_APP                  0xA5

#define UPDATA_2_UART              0xA3  
#define UPDATA_2_NET               0xAA   

#define	BROADCAST_ADD	             0xFF    //modbus rtu广播地址
#define MODBUS_TCP_ADDR            0x01   //modbus tcp单元标识符
     
#define BOOT_MAX_REG_ADD           0x10
#define BOOT_MAX_INPUT_REG_ADD     0x03

#define HARD_VER_REG               0x00
#define SOFT_VER_REG               0x01
#define DEV_ADDR_REG               0x04
#define IP_START_REG               0x05
#define PORT_REG                   0x07
#define RUN_MODE_REG               0x09
#define UPDATE_TYPE_REG            0x0A
// #define ERASE_FLASH_REG            0x0B
// #define CHECK_START_FLASH_REG      0x0C
#define SYS_RST_REG                0x0F
     
//ModBusTCP输入寄存器起始地址
#define APP_DI_START_REG           0x10
//ModBusTCP保持寄存器相关地址定义
#define APP_DO1_START_REG          0x20
#define MCU_CTRL_DMM_VOL           0x20
#define MCU_CTRL_DMM_CUR           0x21
#define MCU_RL_CTRL_RES            0x22
#define MCU_CTRL_ALLIO             0x23
     
//通信协议中Bootloader_04H~0FH存储串口通讯地址和IP地址信息
#define BOOT_MIN_HOLD_REG_ADD      0x04
#define BOOT_MAX_HOLD_REG_ADD      0x0F
     
//APP最大寄存器地址
#define APP_MAX_REG_ADD            0x30
     
//APP保持寄存器地址范围
#define APP_MIN_HOLD_REG_ADD       0x20
#define APP_MAX_HOLD_REG_ADD       0x2F
     
//APP输入寄存器地址范围
#define APP_MIN_INPUT_REG_ADD      0x10
#define APP_MAX_INPUT_REG_ADD      0x1F
     
typedef struct _bootpara{
  uint8_t UartADDR;
  uint8_t RSV1;
  uint8_t IP1;
  uint8_t IP2;
  uint8_t IP3;
  uint8_t IP4;
  uint8_t PortH;
  uint8_t PortL;
  uint8_t RSV2;
  uint8_t RSV3;
  uint8_t RunMode;
  uint8_t ID;
  uint8_t UpdateType;
  uint8_t RSV4;
  uint8_t EraseFlagH;
  uint8_t EraseFlagL;
  uint32_t CheckFlashLen;
  uint32_t RSV5;
}T_BOOT_PARA;
     
extern const T_BOOT_PARA DefaultBootPara;
     
typedef enum
{
  MODBUS_OK       = 0x00,
  MODBUS_FUN_ERR  = 0x01,   
  MODBUS_ADD_ERR  = 0x02,   
  MODBUS_DAT_ERR  = 0x03,   
  MODBUS_DEV_ERR  = 0x04,   
}E_MODBUS_ERRORCODE;
     
     
extern uint8_t aucModbusData[APP_MAX_REG_ADD][2];
     
uint8_t Modbus_Input_RegData_Process(uint16_t RegAddr);
uint8_t Modbus_Hold_RegData_Process(uint16_t RegAddr, uint8_t *pData);
uint8_t Modbus_Check_Reg_Data(uint8_t func, uint16_t reg, uint16_t regLen);
void vApp_Modbus_DataInit(void);
void vApp_DevPara_ResetCheck(void);
uint8_t Modbus_Coil_Set(uint16_t reg, uint8_t data);

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif