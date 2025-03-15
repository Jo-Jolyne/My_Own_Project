#ifndef __MISC_H_
#define __MISC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "spi.h"
#include "m24c32.h"

// #define MY_DEBUG
#ifdef MY_DEBUG
#define mydebug(format, ...) printf("%s:%d "format"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define my_printf(format, ...) printf(format, ##__VA_ARGS__)
#else
#define mydebug(format, ...) ((void)0U)
#define my_printf(format, ...) ((void)0U)
#endif

//2个字节数据大小端转换
#define u16EdianChange(A) ((((uint16_t)(A) & 0xFF00) >> 8) | (((uint16_t)(A) & 0x00FF) << 8))

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef uint8_t BOOL;

typedef enum
{
    OK           = 0x00U,
    ERR          = 0x01U,
    SPI_ERROR    = 0x02U,
    IIC_ERROR    = 0x03U,
    PARA_OVER    = 0x04U,
    TEST_ERROR   = 0x05U,
    COMMD_ERROR  = 0xFFU
}ErrorCode;

#ifdef __cplusplus
}
#endif
#endif