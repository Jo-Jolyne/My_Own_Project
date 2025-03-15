#include "app_timer.h"
#include "app_modbus.h"
#include "app_net.h"
#include "board.h"

void HAL_Tick_SysCall(void)
{ 
    static uint16_t RunLEDCnt = 0;
    //LED Blink
    if (RunLEDCnt++ > 500)
    {
        RunLEDCnt = 0;
        HAL_GPIO_TogglePin(LED_RUN_GPIO_Port,LED_RUN_Pin);
    }

    //Reset Key
    Addr_Reset_Check();

    //Net Link
    vNet_LinkLose_Count();
}