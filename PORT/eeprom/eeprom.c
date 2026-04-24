/**
 ******************************************************************************
 * @file    eeprom.c
 * @brief   AT24C256C EEPROM驱动实现（软件模拟I2C）- 完全修复版
 ******************************************************************************
 */
#include "eeprom.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include <string.h>

/* 私有函数声明 --------------------------------------------------------------*/
static void EEPROM_I2C_Delay(void);
static void EEPROM_I2C_SetSDA_Output(void);
static void EEPROM_I2C_SetSDA_Input(void);
static void EEPROM_I2C_SetSDA(uint8_t state);
static void EEPROM_I2C_SetSCL(uint8_t state);
static uint8_t EEPROM_I2C_ReadSDA(void);

/**
 * @brief  I2C延时函数
 * @retval None
 */
static void EEPROM_I2C_Delay(void)
{
    // 72MHz下，约10微秒，完美适配AT24C256
    for (volatile uint32_t i = 0; i < 100; i++);
}

/**
 * @brief  设置SDA为输出模式
 * @retval None
 */
static void EEPROM_I2C_SetSDA_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = EE_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;  // 开漏输出
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // 上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(EE_SDA_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief  设置SDA为输入模式
 * @retval None
 */
static void EEPROM_I2C_SetSDA_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = EE_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;      // 输入模式
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // 上拉
    HAL_GPIO_Init(EE_SDA_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief  设置SDA引脚电平（输出模式）
 * @param  state: 引脚电平（0:低电平，1:高电平）
 * @retval None
 */
static void EEPROM_I2C_SetSDA(uint8_t state)
{
    if (state) {
        HAL_GPIO_WritePin(EE_SDA_GPIO_Port, EE_SDA_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(EE_SDA_GPIO_Port, EE_SDA_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief  设置SCL引脚电平
 * @param  state: 引脚电平（0:低电平，1:高电平）
 * @retval None
 */
static void EEPROM_I2C_SetSCL(uint8_t state)
{
    if (state) {
        HAL_GPIO_WritePin(EE_SCL_GPIO_Port, EE_SCL_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(EE_SCL_GPIO_Port, EE_SCL_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief  读取SDA引脚电平
 * @retval SDA引脚电平
 */
static uint8_t EEPROM_I2C_ReadSDA(void)
{
    return HAL_GPIO_ReadPin(EE_SDA_GPIO_Port, EE_SDA_Pin);
}

/**
 * @brief  EEPROM初始化
 * @retval None
 */
void EEPROM_Init(void)
{
    // 初始化GPIO为开漏输出模式
    EEPROM_I2C_SetSDA_Output();

    // 初始化SCL为开漏输出模式
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = EE_SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;  // 开漏输出
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // 上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(EE_SCL_GPIO_Port, &GPIO_InitStruct);

    // 确保SCL和SDA为高电平（空闲状态）
    HAL_GPIO_WritePin(EE_SCL_GPIO_Port, EE_SCL_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EE_SDA_GPIO_Port, EE_SDA_Pin, GPIO_PIN_SET);

    EEPROM_I2C_Delay();
}

/**
 * @brief  I2C起始信号
 * @retval None
 */
void EEPROM_I2C_Start(void)
{
    EEPROM_I2C_SetSDA_Output();  // 确保SDA为输出模式
    EEPROM_I2C_SetSDA(1);
    EEPROM_I2C_SetSCL(1);
    EEPROM_I2C_Delay();
    EEPROM_I2C_SetSDA(0);
    EEPROM_I2C_Delay();
    EEPROM_I2C_SetSCL(0);
    EEPROM_I2C_Delay();
}

/**
 * @brief  I2C停止信号
 * @retval None
 */
void EEPROM_I2C_Stop(void)
{
    EEPROM_I2C_SetSDA_Output();  // 确保SDA为输出模式
    EEPROM_I2C_SetSDA(0);
    EEPROM_I2C_Delay();
    EEPROM_I2C_SetSCL(1);
    EEPROM_I2C_Delay();
    EEPROM_I2C_SetSDA(1);
    EEPROM_I2C_Delay();
}

/**
 * @brief  I2C发送应答信号
 * @retval None
 */
void EEPROM_I2C_Ack(void)
{
    EEPROM_I2C_SetSDA_Output();  // 确保SDA为输出模式
    EEPROM_I2C_SetSDA(0);
    EEPROM_I2C_Delay();
    EEPROM_I2C_SetSCL(1);
    EEPROM_I2C_Delay();
    EEPROM_I2C_SetSCL(0);
    EEPROM_I2C_Delay();
}

/**
 * @brief  I2C发送非应答信号
 * @retval None
 */
void EEPROM_I2C_NAck(void)
{
    EEPROM_I2C_SetSDA_Output();  // 确保SDA为输出模式
    EEPROM_I2C_SetSDA(1);
    EEPROM_I2C_Delay();
    EEPROM_I2C_SetSCL(1);
    EEPROM_I2C_Delay();
    EEPROM_I2C_SetSCL(0);
    EEPROM_I2C_Delay();
}

/**
 * @brief  I2C等待应答
 * @retval 0:收到应答，1:未收到应答（通信失败）
 */
uint8_t EEPROM_I2C_WaitAck(void)
{
    uint8_t ack;

    EEPROM_I2C_SetSDA_Input();   // 切换为输入模式
    EEPROM_I2C_Delay();
    EEPROM_I2C_SetSCL(1);
    EEPROM_I2C_Delay();

    ack = EEPROM_I2C_ReadSDA();  // 读取SDA

    EEPROM_I2C_SetSCL(0);
    EEPROM_I2C_Delay();

    EEPROM_I2C_SetSDA_Output();  // 切换回输出模式
    EEPROM_I2C_Delay();

    return ack;
}

/**
 * @brief  I2C发送一个字节
 * @param  data: 要发送的数据
 * @retval None
 */
void EEPROM_I2C_SendByte(uint8_t data)
{
    uint8_t i;

    EEPROM_I2C_SetSDA_Output();  // 确保SDA为输出模式

    for (i = 0; i < 8; i++) {
        EEPROM_I2C_SetSCL(0);
        EEPROM_I2C_Delay();

        if (data & 0x80) {
            EEPROM_I2C_SetSDA(1);
        } else {
            EEPROM_I2C_SetSDA(0);
        }

        EEPROM_I2C_Delay();
        EEPROM_I2C_SetSCL(1);
        EEPROM_I2C_Delay();

        data <<= 1;
    }

    EEPROM_I2C_SetSCL(0);
    EEPROM_I2C_Delay();
}

/**
 * @brief  I2C读取一个字节
 * @param  ack: 是否发送应答（1:发送ACK，0:发送NACK）
 * @retval 读取到的数据
 */
uint8_t EEPROM_I2C_ReadByte(uint8_t ack)
{
    uint8_t i, data = 0;

    EEPROM_I2C_SetSDA_Input();   // 切换为输入模式（释放SDA）

    for (i = 0; i < 8; i++) {
        EEPROM_I2C_SetSCL(0);
        EEPROM_I2C_Delay();
        EEPROM_I2C_SetSCL(1);
        EEPROM_I2C_Delay();

        data <<= 1;
        if (EEPROM_I2C_ReadSDA()) {
            data |= 0x01;
        }
    }

    EEPROM_I2C_SetSCL(0);
    EEPROM_I2C_Delay();

    // 发送ACK或NACK
    if (ack) {
        EEPROM_I2C_Ack();    // ack=1时发送ACK（应答）
    } else {
        EEPROM_I2C_NAck();   // ack=0时发送NACK（非应答）
    }

    return data;
}

/**
 * @brief  从EEPROM读取一个字节
 * @param  addr: 地址（0-32767）
 * @retval 读取到的数据（失败返回0xFF）
 */
uint8_t EEPROM_ReadByte(uint16_t addr)
{
    uint8_t data;

    EEPROM_I2C_Start();
    EEPROM_I2C_SendByte(EEPROM_I2C_ADDRESS);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 0xFF;  // 通信失败
    }

    // 发送16位地址（高8位）
    EEPROM_I2C_SendByte((addr >> 8) & 0xFF);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 0xFF;  // 通信失败
    }

    // 发送16位地址（低8位）
    EEPROM_I2C_SendByte(addr & 0xFF);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 0xFF;  // 通信失败
    }

    EEPROM_I2C_Start();
    EEPROM_I2C_SendByte(EEPROM_I2C_ADDRESS | 0x01); // 读地址
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 0xFF;  // 通信失败
    }

    data = EEPROM_I2C_ReadByte(0); // 读取数据并发送NACK（单个字节）
    EEPROM_I2C_Stop();

    return data;
}

/**
 * @brief  向EEPROM写入一个字节
 * @param  addr: 地址（0-32767）
 * @param  data: 要写入的数据
 * @retval 0:成功，1:失败
 */
uint8_t EEPROM_WriteByte(uint16_t addr, uint8_t data)
{
    EEPROM_I2C_Start();
    EEPROM_I2C_SendByte(EEPROM_I2C_ADDRESS);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    // 发送16位地址（高8位）
    EEPROM_I2C_SendByte((addr >> 8) & 0xFF);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    // 发送16位地址（低8位）
    EEPROM_I2C_SendByte(addr & 0xFF);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    EEPROM_I2C_SendByte(data);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    EEPROM_I2C_Stop();

    // 等待写入完成（AT24C256写入周期约为5ms）
    HAL_Delay(10);

    return 0;  // 成功
}

/**
 * @brief  从EEPROM读取多个字节
 * @param  addr: 起始地址
 * @param  buffer: 数据缓冲区
 * @param  len: 要读取的长度
 * @retval 0:成功，1:失败
 */
uint8_t EEPROM_ReadBuffer(uint16_t addr, uint8_t *buffer, uint16_t len)
{
    uint16_t i;

    if (len == 0) return 1;

    EEPROM_I2C_Start();
    EEPROM_I2C_SendByte(EEPROM_I2C_ADDRESS);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    // 发送16位地址（高8位）
    EEPROM_I2C_SendByte((addr >> 8) & 0xFF);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    // 发送16位地址（低8位）
    EEPROM_I2C_SendByte(addr & 0xFF);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    EEPROM_I2C_Start();
    EEPROM_I2C_SendByte(EEPROM_I2C_ADDRESS | 0x01); // 读地址
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    // 读取数据
    for (i = 0; i < len - 1; i++) {
        buffer[i] = EEPROM_I2C_ReadByte(1); // 前面字节发送ACK
    }

    buffer[len - 1] = EEPROM_I2C_ReadByte(0); // 最后一个字节发送NACK
    EEPROM_I2C_Stop();

    return 0;  // 成功
}

/**
 * @brief  向EEPROM写入多个字节（自动分页）
 * @param  addr: 起始地址
 * @param  buffer: 数据缓冲区
 * @param  len: 要写入的长度
 * @retval 0:成功，1:失败
 */
uint8_t EEPROM_WriteBuffer(uint16_t addr, uint8_t *buffer, uint16_t len)
{
    uint16_t i;
    uint16_t page_boundary;
    uint16_t bytes_to_write;
    uint8_t result;

    for (i = 0; i < len; ) {
        // 计算当前页的边界
        page_boundary = ((addr + i) / EEPROM_PAGE_SIZE + 1) * EEPROM_PAGE_SIZE;
        bytes_to_write = page_boundary - (addr + i);

        if (bytes_to_write > (len - i)) {
            bytes_to_write = len - i;
        }

        result = EEPROM_WritePage(addr + i, &buffer[i], bytes_to_write);
        if (result) {
            return 1;  // 写入失败
        }
        i += bytes_to_write;
    }

    return 0;  // 成功
}

/**
 * @brief  向EEPROM写入一页数据（最多64字节）
 * @param  addr: 页起始地址
 * @param  buffer: 数据缓冲区
 * @param  len: 要写入的长度（1-64）
 * @retval 0:成功，1:失败
 */
uint8_t EEPROM_WritePage(uint16_t addr, uint8_t *buffer, uint8_t len)
{
    uint8_t i;

    if (len == 0 || len > EEPROM_PAGE_SIZE) return 1;

    EEPROM_I2C_Start();
    EEPROM_I2C_SendByte(EEPROM_I2C_ADDRESS);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    // 发送16位地址（高8位）
    EEPROM_I2C_SendByte((addr >> 8) & 0xFF);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    // 发送16位地址（低8位）
    EEPROM_I2C_SendByte(addr & 0xFF);
    if (EEPROM_I2C_WaitAck()) {
        EEPROM_I2C_Stop();
        return 1;  // 通信失败
    }

    for (i = 0; i < len; i++) {
        EEPROM_I2C_SendByte(buffer[i]);
        if (EEPROM_I2C_WaitAck()) {
            EEPROM_I2C_Stop();
            return 1;  // 通信失败
        }
    }

    EEPROM_I2C_Stop();

    // 等待写入完成（AT24C256写入周期约为5ms）
    HAL_Delay(10);

    return 0;  // 成功
}
