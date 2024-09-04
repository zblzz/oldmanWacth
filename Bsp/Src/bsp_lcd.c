/**
 * @file   bsp_lcd.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "bsp_lcd.h"
#include "bsp_delay.h"
#include "spi.h"
#include "tim.h"

/******************************************************************************
      函数说明：LCD端口初始化
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStructure.Pin = RES_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;        // 推挽输出
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 速度50MHz
    HAL_GPIO_Init(RES_PORT, &GPIO_InitStructure);         // 初始化GPIOB
    HAL_GPIO_WritePin(RES_PORT, RES_PIN, GPIO_PIN_SET);

    GPIO_InitStructure.Pin = DC_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;        // 推挽输出
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 速度50MHz
    HAL_GPIO_Init(DC_PORT, &GPIO_InitStructure);          // 初始化GPIOC
    HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);

    GPIO_InitStructure.Pin = CS_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;        // 推挽输出
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 速度50MHz
    HAL_GPIO_Init(CS_PORT, &GPIO_InitStructure);          // 初始化GPIOD
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

/******************************************************************************
      函数说明：LCD串行数据写入函数(software SPI)
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{
    // hard SPI
    HAL_SPI_Transmit_DMA(&hspi1, &dat, 1);
    while (__HAL_DMA_GET_COUNTER(&hdma_spi1_tx) != 0)
        ;

    // soft SPI
    // u8 i;
    // for (i = 0; i < 8; i++)
    // {
    //     LCD_SCLK_Clr();
    //     if (dat & 0x80)
    //     {
    //         LCD_MOSI_Set();
    //     }
    //     else
    //     {
    //         LCD_MOSI_Clr();
    //     }
    //     LCD_SCLK_Set();
    //     dat <<= 1;
    // }
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
    LCD_Writ_Bus(dat);
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
    // LCD_Writ_Bus((u8)dat >> 8);
    // LCD_Writ_Bus(dat & 0xff);
    uint8_t temp[2];
    temp[0] = (dat >> 8) & 0xff;
    temp[1] = dat & 0xff;
    HAL_SPI_Transmit_DMA(&hspi1, temp, 2);
    while (__HAL_DMA_GET_COUNTER(&hdma_spi1_tx) != 0)
        ;
}

/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
    LCD_DC_Clr(); // 写命令
    LCD_Writ_Bus(dat);
    LCD_DC_Set(); // 写数据
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_WR_REG(0x2a); // 列地址设置
    LCD_WR_DATA(x1);
    LCD_WR_DATA(x2);
    LCD_WR_REG(0x2b); // 行地址设置
    LCD_WR_DATA(y1);
    LCD_WR_DATA(y2);
    LCD_WR_REG(0x2c); // 储存器写
}

/******************************************************************************
      函数说明：LCD调节背光
      入口数据：dc,占空比,5%~100%
      返回值：  无
******************************************************************************/
void LCD_Set_Light(uint8_t dc)
{
    if (dc >= 0 && dc <= 100)
        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, dc * PWM_PERIOD / 100);
}

/******************************************************************************
      函数说明：LCD关闭背光
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Close_Light(void)
{
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 0);
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
}

/******************************************************************************
      函数说明：LCD开启背光
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Open_Light(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

/******************************************************************************
      函数说明：ST7789 SLEEP IN
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_ST7789_SleepIn(void)
{
    LCD_WR_REG(0x10);
    delay_ms(100);
}

/******************************************************************************
      函数说明：ST7789 SLEEP OUT
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_ST7789_SleepOut(void)
{
    LCD_WR_REG(0x11);
    delay_ms(100);
}

/******************************************************************************
      函数说明：LCD初始化
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Init(void)
{
    LCD_GPIO_Init(); // 初始化GPIO
    LCD_Open_Light();
    LCD_CS_Clr(); // chip select

    LCD_RES_Clr(); // 复位
    delay_ms(100);
    LCD_RES_Set();
    delay_ms(100);

    LCD_Set_Light(50); // 背光

    LCD_WR_REG(0x11);
    delay_ms(120);
    LCD_WR_REG(0x36);
    if (USE_HORIZONTAL == 0)
        LCD_WR_DATA8(0x00);
    else if (USE_HORIZONTAL == 1)
        LCD_WR_DATA8(0xC0);
    else if (USE_HORIZONTAL == 2)
        LCD_WR_DATA8(0x70);
    else
        LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x32); // Vcom=1.35V

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x15); // GVDD=4.8V  颜色深度

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20); // VDV, 0x20:0v

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F); // 0x0F:60Hz

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);
    LCD_WR_REG(0x21);

    LCD_WR_REG(0x29);
}
