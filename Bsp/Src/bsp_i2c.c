/**
 * @file   bsp_i2c.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.04.25
 */

#include "bsp_i2c.h"
#include "bsp_delay.h"

/**
 * @brief SDA线输入模式配置
 * @param  bus:
 * @author zbl (zzz)
 */
void SDA_Input_Mode(i2c_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = bus->i2c_sda_pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(bus->i2c_sda_port, &GPIO_InitStructure);

    // uint16_t pin = bus->i2c_sda_pin;
    // uint16_t index = 0;
    // while (!(pin & 0x01))
    // {
    //     pin >>= 1;
    //     index++;
    // }
    // bus->i2c_sda_port->MODER &= ~(3 << (2 * index));
}

/**
 * @brief  SDA线输出模式配置
 * @param  bus:
 * @author zbl (zzz)
 */
void SDA_Output_Mode(i2c_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = bus->i2c_sda_pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(bus->i2c_sda_port, &GPIO_InitStructure);

    // uint16_t pin = bus->i2c_sda_pin;
    // uint16_t index = 0;
    // while (!(pin & 0x01))
    // {
    //     pin >>= 1;
    //     index++;
    // }
    // bus->i2c_sda_port->MODER |= 1 << (2 * index);
}

/**
 * @brief  SDA写一个位
 * @param  bus:
 * @param  val:
 * @author zbl (zzz)
 */
void SDA_Write(i2c_bus_t *bus, uint16_t val)
{
    // HAL_GPIO_WritePin()
    if (val)
    {
        bus->i2c_sda_port->BSRR |= bus->i2c_sda_pin;
    }
    else
    {
        bus->i2c_sda_port->BSRR = (uint32_t)bus->i2c_sda_pin << 16U;
    }
}

/**
 * @brief  SDA读一位
 * @param  bus:
 * @return uint8_t:
 * @author zbl (zzz)
 */
uint8_t SDA_Read(i2c_bus_t *bus)
{
    return HAL_GPIO_ReadPin(bus->i2c_sda_port, bus->i2c_sda_pin) == GPIO_PIN_SET ? 1 : 0;
}

/**
 * @brief  SCL写一个位
 * @param  bus:
 * @param  val:
 * @author zbl (zzz)
 */
void SCL_Write(i2c_bus_t *bus, uint16_t val)
{
    if (val)
    {
        bus->i2c_scl_port->BSRR |= bus->i2c_scl_pin;
    }
    else
    {
        bus->i2c_scl_port->BSRR = (uint32_t)bus->i2c_scl_pin << 16U;
    }
}

/**
 * @brief  IIC 起始信号
 * @param  bus:
 * @author zbl (zzz)
 */
void IIC_Start(i2c_bus_t *bus)
{
    SDA_Output_Mode(bus); // sda线输出
    SDA_Write(bus, 1);
    delay_us(1);
    SCL_Write(bus, 1);
    delay_us(1);
    SDA_Write(bus, 0);
    delay_us(1);
    SCL_Write(bus, 0); // START:when CLK is high,DATA change form high to low
}

/**
 * @brief  IIC 终止信号
 * @param  bus:
 * @author zbl (zzz)
 */
void IIC_Stop(i2c_bus_t *bus)
{
    SDA_Output_Mode(bus);
    SCL_Write(bus, 0);
    delay_us(1);
    SDA_Write(bus, 0);
    delay_us(1);
    SCL_Write(bus, 1);
    delay_us(1);
    SDA_Write(bus, 1); // STOP:when CLK is high DATA change form low to high
    delay_us(1);
}

/**
 * @brief IIC 等待确认信号
 * @param  bus:
 * @return uint8_t: 0:正常
 * @author zbl (zzz)
 */
uint8_t IIC_WaitAck(i2c_bus_t *bus)
{
    uint16_t cErrTime = 0;
    SDA_Input_Mode(bus);
    SDA_Write(bus, 1);
    SCL_Write(bus, 1);
    delay_us(1);
    while (SDA_Read(bus))
    {
        cErrTime++;
        if (cErrTime > 250)
        {
            IIC_Stop(bus);
            return 1;
        }
    }
    SCL_Write(bus, 0);
    delay_us(1);
    return 0;
}

/**
 * @brief  IIC发送确认信号
 * @param  bus:
 * @author zbl (zzz)
 */
void IIC_SendAck(i2c_bus_t *bus)
{
    SCL_Write(bus, 0); // 确保时钟线为低时，数据线才能变化为0，否则这就可能成起始信号了！
    SDA_Output_Mode(bus);
    delay_us(1);
    SDA_Write(bus, 0);
    delay_us(1);
    SCL_Write(bus, 1);
    delay_us(2);
    SCL_Write(bus, 0);
}

/**
 * @brief  IIC发送非确认信号
 * @param  bus:
 * @author zbl (zzz)
 */
void IIC_SendNAck(i2c_bus_t *bus)
{
    SCL_Write(bus, 0); // 确保时钟线为低时，数据线才能变化为0，否则这就可能成起始信号了！
    SDA_Output_Mode(bus);
    SDA_Write(bus, 1);
    delay_us(1);
    SCL_Write(bus, 1);
    delay_us(2);
    SCL_Write(bus, 0);
}

/**
 * @brief  IIC发送一个字节
 * @param  bus:
 * @param  cSendByte:   需要发送的字节
 * @author zbl (zzz)
 */
void IIC_SendByte(i2c_bus_t *bus, uint8_t cSendByte)
{
    uint8_t t;
    SDA_Output_Mode(bus);
    SCL_Write(bus, 0); // 拉低时钟开始数据传输
    for (t = 0; t < 8; t++)
    {
        delay_us(1);
        SDA_Write(bus, (cSendByte & 0x80) >> 7);
        cSendByte <<= 1;
        delay_us(1);
        SCL_Write(bus, 1);
        delay_us(2);
        SCL_Write(bus, 0);
    }
}

/**
 * @brief  IIC接收一个字节
 * @param  bus:
 * @return uint8_t:
 * @author zbl (zzz)
 */
uint8_t IIC_ReceiveByte(i2c_bus_t *bus)
{
    uint8_t i, receive = 0;
    SDA_Input_Mode(bus); // SDA设置为输入
    SDA_Write(bus, 1);   // 拉高，准备接收数据
    delay_us(1);
    for (i = 0; i < 8; i++)
    {
        receive <<= 1;
        SCL_Write(bus, 0);
        delay_us(2);
        SCL_Write(bus, 1);
        delay_us(1);
        if (SDA_Read(bus))
            receive |= 0x01;
        delay_us(1);
    }
    SCL_Write(bus, 0);
    return receive;
}

/**
 * @brief   IIC 写一个字节数据
 * @param  bus:
 * @param  daddr:   IIC 地址
 * @param  reg:     寄存器地址
 * @param  data:    数据
 * @return uint8_t: 0：正常
 * @author zbl (zzz)
 */
uint8_t IIC_Write_One_Byte(i2c_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t data)
{
    IIC_Start(bus);
    IIC_SendByte(bus, daddr << 1);
    if (IIC_WaitAck(bus)) // 等待应答
    {
        IIC_Stop(bus);
        return 1;
    }

    // TODO: 根据寄存器地址，修改是否需要发送高8位
    IIC_SendByte(bus, reg);
    IIC_WaitAck(bus);

    IIC_SendByte(bus, data);
    IIC_WaitAck(bus);
    IIC_Stop(bus);
    return 0;
}

/**
 * @brief  IIC 写多个字节数据
 * @param  bus:
 * @param  daddr:   IIC 地址
 * @param  reg:     寄存器地址
 * @param  length:  数据长度
 * @param  buff:    数据
 * @return uint8_t: 0：正常
 * @author zbl (zzz)
 */
uint8_t IIC_Write_Multi_Byte(i2c_bus_t *bus, uint8_t daddr, uint8_t reg, uint32_t length, uint8_t *buff)
{
    uint32_t i;
    IIC_Start(bus);
    IIC_SendByte(bus, daddr << 1);
    if (IIC_WaitAck(bus))
    {
        IIC_Stop(bus);
        return 1;
    }

    // TODO: 根据寄存器地址，修改是否需要发送高8位
    IIC_SendByte(bus, reg);
    IIC_WaitAck(bus);

    for (i = 0; i < length; i++)
    {
        IIC_SendByte(bus, *buff++);
        IIC_WaitAck(bus);
    }
    IIC_Stop(bus);
    return 0;
}

/**
 * @brief   IIC 读一个字节数据
 * @param  bus:
 * @param  daddr:   IIC 地址
 * @param  reg:     寄存器地址
 * @return uint8_t: 0：正常
 * @author zbl (zzz)
 */
uint8_t IIC_Read_One_Byte(i2c_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t *dat)
{
    IIC_Start(bus);
    IIC_SendByte(bus, daddr << 1);
    // IIC_WaitAck(bus);
    if (IIC_WaitAck(bus)) // 等待应答
    {
        IIC_Stop(bus);
        return 1;
    }

    // TODO: 根据寄存器地址，修改是否需要发送高8位
    IIC_SendByte(bus, reg);
    IIC_WaitAck(bus);

    IIC_Start(bus);
    IIC_SendByte(bus, (daddr << 1) + 1);
    IIC_WaitAck(bus);
    *dat = IIC_ReceiveByte(bus);
    IIC_SendNAck(bus);
    IIC_Stop(bus);
    return 0;
}

/**
 * @brief  IIC 读多个字节数据
 * @param  bus:
 * @param  daddr:   IIC 地址
 * @param  reg:     寄存器地址
 * @param  length:  读数据的长度
 * @param  buff:    缓冲区
 * @return uint8_t: 0：正常
 * @author zbl (zzz)
 */
uint8_t IIC_Read_Multi_Byte(i2c_bus_t *bus, uint8_t daddr, uint8_t reg, uint32_t length, uint8_t *buff)
{
    uint32_t i;
    IIC_Start(bus);
    IIC_SendByte(bus, daddr << 1);
    if (IIC_WaitAck(bus))
    {
        IIC_Stop(bus);
        return 1;
    }

    // TODO: 根据寄存器地址，修改是否需要发送高8位
    IIC_SendByte(bus, reg);
    IIC_WaitAck(bus);

    IIC_Start(bus);
    IIC_SendByte(bus, (daddr << 1) + 1);
    IIC_WaitAck(bus);
    for (i = 0; i < length; i++)
    {
        *buff++ = IIC_ReceiveByte(bus);
        if (i < length - 1)
            IIC_SendAck(bus);
    }

    IIC_SendNAck(bus);
    IIC_Stop(bus);
    return 0;
}

// 初始化IIC
void IIC_Init(i2c_bus_t *bus)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = bus->i2c_sda_pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(bus->i2c_sda_port, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = bus->i2c_scl_pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(bus->i2c_scl_port, &GPIO_InitStructure);

    HAL_GPIO_WritePin(bus->i2c_scl_port, bus->i2c_scl_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(bus->i2c_sda_port, bus->i2c_sda_pin, GPIO_PIN_SET);
}