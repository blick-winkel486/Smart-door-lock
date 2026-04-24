/**
 ******************************************************************************
 * @file    eeprom.h
 * @brief   AT24C256C EEPROM驱动头文件（软件模拟I2C）
 ******************************************************************************
 */

#ifndef __EEPROM_H
#define __EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdint.h"

/* 定义 ----------------------------------------------------------------------*/
#define EEPROM_I2C_ADDRESS     0xA0    // AT24C256C I2C地址（写地址）
#define EEPROM_SIZE            32768   // 32KB = 32768字节
#define EEPROM_PAGE_SIZE       64      // 页面大小64字节
#define EEPROM_WRITE_DELAY     5       // 写操作延迟（ms）

/* 函数声明 ------------------------------------------------------------------*/
void EEPROM_Init(void);
uint8_t EEPROM_ReadByte(uint16_t addr);
uint8_t EEPROM_WriteByte(uint16_t addr, uint8_t data);
uint8_t EEPROM_ReadBuffer(uint16_t addr, uint8_t *buffer, uint16_t len);
uint8_t EEPROM_WriteBuffer(uint16_t addr, uint8_t *buffer, uint16_t len);
uint8_t EEPROM_WritePage(uint16_t addr, uint8_t *buffer, uint8_t len);

/* 软件I2C函数 --------------------------------------------------------------*/
void EEPROM_I2C_Start(void);
void EEPROM_I2C_Stop(void);
void EEPROM_I2C_SendByte(uint8_t data);
uint8_t EEPROM_I2C_ReadByte(uint8_t ack);
uint8_t EEPROM_I2C_WaitAck(void);
void EEPROM_I2C_Ack(void);
void EEPROM_I2C_NAck(void);

#ifdef __cplusplus
}
#endif

#endif /* __EEPROM_H */
