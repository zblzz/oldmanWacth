/**
 * @file   bsp_max30102.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "bsp_max30102.h"
#include "log.h"

i2c_bus_t MAX30102_dev =
    {
        .i2c_sda_port = GPIOA,
        .i2c_scl_port = GPIOA,
        .i2c_sda_pin = GPIO_PIN_10,
        .i2c_scl_pin = GPIO_PIN_11,
};

/**********************MAX30102_INT初始化**************/
uint8_t max30102_Bus_Write(uint8_t Register_Address, uint8_t Word_Data)
{

    /* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */

    /* 第1步：发起I2C总线启动信号 */
    IIC_Start(&MAX30102_dev);

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_SendByte(&MAX30102_dev, max30102_WR_address | I2C_WR); /* 此处是写指令 */

    /* 第3步：发送ACK */
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址 */
    IIC_SendByte(&MAX30102_dev, Register_Address);
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第5步：开始写入数据 */
    IIC_SendByte(&MAX30102_dev, Word_Data);

    /* 第6步：发送ACK */
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 发送I2C总线停止信号 */
    IIC_Stop(&MAX30102_dev);
    return 1; /* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    IIC_Stop(&MAX30102_dev);
    return 0;
}

uint8_t max30102_Bus_Read(uint8_t Register_Address)
{
    uint8_t data;

    /* 第1步：发起I2C总线启动信号 */
    IIC_Start(&MAX30102_dev);

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_SendByte(&MAX30102_dev, max30102_WR_address | I2C_WR); /* 此处是写指令 */

    /* 第3步：发送ACK */
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址， */
    IIC_SendByte(&MAX30102_dev, (uint8_t)Register_Address);
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第6步：重新启动I2C总线。下面开始读取数据 */
    IIC_Start(&MAX30102_dev);

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_SendByte(&MAX30102_dev, max30102_WR_address | I2C_RD); /* 此处是读指令 */

    /* 第8步：发送ACK */
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第9步：读取数据 */
    data = IIC_ReceiveByte(&MAX30102_dev); /* 读1个字节 */
    IIC_SendNAck(&MAX30102_dev);           /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
    /* 发送I2C总线停止信号 */
    IIC_Stop(&MAX30102_dev);
    return data; /* 执行成功 返回data值 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    IIC_Stop(&MAX30102_dev);
    return 0;
}

void max30102_FIFO_ReadWords(uint8_t Register_Address, uint16_t Word_Data[][2], uint8_t count)
{
    uint8_t i = 0;
    uint8_t no = count;
    uint8_t data1, data2;
    /* 第1步：发起I2C总线启动信号 */
    IIC_Start(&MAX30102_dev);

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_SendByte(&MAX30102_dev, max30102_WR_address | I2C_WR); /* 此处是写指令 */

    /* 第3步：发送ACK */
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址， */
    IIC_SendByte(&MAX30102_dev, (uint8_t)Register_Address);
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第6步：重新启动I2C总线。下面开始读取数据 */
    IIC_Start(&MAX30102_dev);

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_SendByte(&MAX30102_dev, max30102_WR_address | I2C_RD); /* 此处是读指令 */

    /* 第8步：发送ACK */
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第9步：读取数据 */
    while (no)
    {
        data1 = IIC_ReceiveByte(&MAX30102_dev);
        IIC_SendAck(&MAX30102_dev);
        data2 = IIC_ReceiveByte(&MAX30102_dev);
        IIC_SendAck(&MAX30102_dev);
        Word_Data[i][0] = (((uint16_t)data1 << 8) | data2); //

        data1 = IIC_ReceiveByte(&MAX30102_dev);
        IIC_SendAck(&MAX30102_dev);
        data2 = IIC_ReceiveByte(&MAX30102_dev);
        if (1 == no)
            IIC_SendNAck(&MAX30102_dev); /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        else
            IIC_SendAck(&MAX30102_dev);
        Word_Data[i][1] = (((uint16_t)data1 << 8) | data2);

        no--;
        i++;
    }
    /* 发送I2C总线停止信号 */
    IIC_Stop(&MAX30102_dev);

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    IIC_Stop(&MAX30102_dev);
}

void max30102_FIFO_ReadBytes(uint8_t Register_Address, uint8_t *Data)
{
    max30102_Bus_Read(REG_INTR_STATUS_1);
    max30102_Bus_Read(REG_INTR_STATUS_2);

    /* 第1步：发起I2C总线启动信号 */
    IIC_Start(&MAX30102_dev);

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_SendByte(&MAX30102_dev, max30102_WR_address | I2C_WR); /* 此处是写指令 */

    /* 第3步：发送ACK */
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址， */
    IIC_SendByte(&MAX30102_dev, (uint8_t)Register_Address);
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第6步：重新启动I2C总线。下面开始读取数据 */
    IIC_Start(&MAX30102_dev);

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_SendByte(&MAX30102_dev, max30102_WR_address | I2C_RD); /* 此处是读指令 */

    /* 第8步：发送ACK */
    if (IIC_WaitAck(&MAX30102_dev) != 0)
    {
        goto cmd_fail; /* EEPROM器件无应答 */
    }

    /* 第9步：读取数据 */
    // Data[0] = IIC_ReceiveByte(&MAX30102_dev);
    // Data[1] = IIC_ReceiveByte(&MAX30102_dev);
    // Data[2] = IIC_ReceiveByte(&MAX30102_dev);
    // Data[3] = IIC_ReceiveByte(&MAX30102_dev);
    // Data[4] = IIC_ReceiveByte(&MAX30102_dev);
    // Data[5] = IIC_ReceiveByte(&MAX30102_dev);

    uint32_t i;
    for (i = 0; i < 6; i++)
    {
        *Data++ = IIC_ReceiveByte(&MAX30102_dev);
        if (i < 5)
            IIC_SendAck(&MAX30102_dev);
    }
    IIC_SendNAck(&MAX30102_dev);
    /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
    /* 发送I2C总线停止信号 */
    IIC_Stop(&MAX30102_dev);

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    IIC_Stop(&MAX30102_dev);
}

void max30102_init(void)
{
    IIC_Init(&MAX30102_dev);

    max30102_reset();

    if (!max30102_Bus_Write(REG_INTR_ENABLE_1, 0xc0)) // INTR setting
    {
        EMlog(LOG_ERROR, "max30102_Bus_Write failed");
        return;
    }
    max30102_Bus_Write(REG_INTR_ENABLE_2, 0x00);
    max30102_Bus_Write(REG_FIFO_WR_PTR, 0x00); // FIFO_WR_PTR[4:0]
    max30102_Bus_Write(REG_OVF_COUNTER, 0x00); // OVF_COUNTER[4:0]
    max30102_Bus_Write(REG_FIFO_RD_PTR, 0x00); // FIFO_RD_PTR[4:0]
    max30102_Bus_Write(REG_FIFO_CONFIG, 0x0f); // sample avg = 1, fifo rollover=false, fifo almost full = 17
    max30102_Bus_Write(REG_MODE_CONFIG, 0x03); // 0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
    max30102_Bus_Write(REG_SPO2_CONFIG, 0x27); // SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)
    max30102_Bus_Write(REG_LED1_PA, 0x24);     // Choose value for ~ 7mA for LED1
    max30102_Bus_Write(REG_LED2_PA, 0x24);     // Choose value for ~ 7mA for LED2
    max30102_Bus_Write(REG_PILOT_PA, 0x7f);    // Choose value for ~ 25mA for Pilot LED
}

void max30102_reset(void)
{
    max30102_Bus_Write(REG_MODE_CONFIG, 0x40);
    max30102_Bus_Write(REG_MODE_CONFIG, 0x40);
}

void maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data)
{
    IIC_Write_One_Byte(&MAX30102_dev, max30102_WR_address >> 1, uch_addr, uch_data);
}

void maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data)
{
    IIC_Read_One_Byte(&MAX30102_dev, max30102_WR_address >> 1, uch_addr, puch_data);
}

void maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led)
{
    uint32_t un_temp;
    unsigned char uch_temp;
    char ach_i2c_data[6];
    *pun_red_led = 0;
    *pun_ir_led = 0;

    // read and clear status register
    maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_temp);
    maxim_max30102_read_reg(REG_INTR_STATUS_2, &uch_temp);

    IIC_Write_Multi_Byte(&MAX30102_dev, max30102_WR_address >> 1, REG_FIFO_DATA, 6, (uint8_t *)ach_i2c_data);

    un_temp = (unsigned char)ach_i2c_data[0];
    un_temp <<= 16;
    *pun_red_led += un_temp;
    un_temp = (unsigned char)ach_i2c_data[1];
    un_temp <<= 8;
    *pun_red_led += un_temp;
    un_temp = (unsigned char)ach_i2c_data[2];
    *pun_red_led += un_temp;

    un_temp = (unsigned char)ach_i2c_data[3];
    un_temp <<= 16;
    *pun_ir_led += un_temp;
    un_temp = (unsigned char)ach_i2c_data[4];
    un_temp <<= 8;
    *pun_ir_led += un_temp;
    un_temp = (unsigned char)ach_i2c_data[5];
    *pun_ir_led += un_temp;
    *pun_red_led &= 0x03FFFF; // Mask MSB [23:18]
    *pun_ir_led &= 0x03FFFF;  // Mask MSB [23:18]
}