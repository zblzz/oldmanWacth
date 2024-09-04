#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "stm32l4xx_hal.h"

#define Software_I2C 1

#ifdef Software_I2C

typedef struct
{
    GPIO_TypeDef *i2c_sda_port;
    uint16_t i2c_sda_pin;
    GPIO_TypeDef *i2c_scl_port;
    uint16_t i2c_scl_pin;
} i2c_bus_t;

void SDA_Input_Mode(i2c_bus_t *bus);
void SDA_Output_Mode(i2c_bus_t *bus);
void SDA_Write(i2c_bus_t *bus, uint16_t val);
uint8_t SDA_Read(i2c_bus_t *bus);
void SCL_Write(i2c_bus_t *bus, uint16_t val);
void IIC_Start(i2c_bus_t *bus);
void IIC_Stop(i2c_bus_t *bus);
uint8_t IIC_WaitAck(i2c_bus_t *bus);
void IIC_SendAck(i2c_bus_t *bus);
void IIC_SendNAck(i2c_bus_t *bus);
void IIC_SendByte(i2c_bus_t *bus, uint8_t cSendByte);
uint8_t IIC_ReceiveByte(i2c_bus_t *bus);
uint8_t IIC_Write_One_Byte(i2c_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t data);
uint8_t IIC_Write_Multi_Byte(i2c_bus_t *bus, uint8_t daddr, uint8_t reg, uint32_t length, uint8_t *buff);
uint8_t IIC_Read_One_Byte(i2c_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t *dat);
uint8_t IIC_Read_Multi_Byte(i2c_bus_t *bus, uint8_t daddr, uint8_t reg, uint32_t length, uint8_t *buff);
void IIC_Init(i2c_bus_t *bus);

#endif
#endif
