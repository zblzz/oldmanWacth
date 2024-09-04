/**
 * @file   bsp_touch.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "bsp_touch.h"
#include "log.h"
// #include "i2c.h"

#define TOUCH_OFFSET_Y 12
#define REVERSE 1

CST816_Info CST816_Instance;

i2c_bus_t CST816_dev =
    {
        .i2c_sda_port = GPIOC,
        .i2c_scl_port = GPIOC,
        .i2c_sda_pin = GPIO_PIN_1,
        .i2c_scl_pin = GPIO_PIN_0,
};

// uint8_t exit_flag = 0;
// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
// {
//     if (GPIO_Pin == GPIO_PIN_12)
//     {
//         exit_flag = 1;

//         __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
//     }
// }

/*
*********************************************************************************************************
*	函 数 名: CST816_GPIO_Init
*	功能说明: CST816 GPIO口初始化
*	形    参：none
*	返 回 值: none
*********************************************************************************************************
*/
void CST816_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* 初始化复位引脚 */
    GPIO_InitStructure.Pin = TOUCH_RST_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TOUCH_RST_PORT, &GPIO_InitStructure);

    HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_RESET);

    /* 初始化中断引脚,不用中断的话屏蔽掉 */
    // GPIO_InitStructure.Pin = TOUCH_INT_PIN;
    // GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    // GPIO_InitStructure.Pull = GPIO_NOPULL;
    // GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    // HAL_GPIO_Init(TOUCH_INT_PORT, &GPIO_InitStructure);

    /* 初始化I2C引脚 */
    IIC_Init(&CST816_dev);

    /* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
    // IICStop(&CST816_dev);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Init
*	功能说明: CST816初始化
*	形    参：none
*	返 回 值: none
*********************************************************************************************************
*/
void CST816_Init(void)
{
    // CST816_GPIO_Init();
    // CST816_Config_AutoSleepTime(5);

    CST816_GPIO_Init();
    CST816_RESET(); // 芯片上电初始化

    EMlog(LOG_INFO, "ChipID:%d\n", CST816_IIC_ReadREG(0xa7));
    // EMlog(LOG_INFO, "FwVersion:%d\n", CST816_IIC_ReadREG(0xa9));
    // EMlog(LOG_INFO, "FactoryID:%d\n", CST816_IIC_ReadREG(0xAA));
    // EMlog(LOG_INFO, "SleepMode:%d\n", CST816_IIC_ReadREG(0xE5));
    // EMlog(LOG_INFO, "LongPressTick:%d\n", CST816_IIC_ReadREG(0xEB));
    // EMlog(LOG_INFO, "MotionMask:%d\n", CST816_IIC_ReadREG(0xEC));
    // EMlog(LOG_INFO, "IrqCtl:%d\n", CST816_IIC_ReadREG(0xFA));
    // EMlog(LOG_INFO, "DisAutoSleep:%d\n", CST816_IIC_ReadREG(0xFE));
    CST816_RESET();
}

/*
*********************************************************************************************************
*	函 数 名: CST816_IIC_ReadREG
*	功能说明: 读取触摸屏单个寄存器的数据
*	形    参：reg：寄存器地址
*	返 回 值: 返回寄存器存储的数据
*********************************************************************************************************
*/
uint8_t CST816_IIC_ReadREG(uint8_t addr)
{
    uint8_t data = 0;
    IIC_Read_One_Byte(&CST816_dev, Device_Addr, addr, &data);
    // HAL_I2C_Mem_Read(&hi2c3, Device_Addr << 1, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    return data;
}

/*
*********************************************************************************************************
*	函 数 名: CST816_IIC_WriteREG
*	功能说明: 向触摸屏的寄存器写入数据
*	形    参：addr：寄存器地址
*						dat:	写入的数据
*	返 回 值: 返回寄存器存储的数据
*********************************************************************************************************
*/
void CST816_IIC_WriteREG(uint8_t addr, uint8_t dat)
{
    IIC_Write_One_Byte(&CST816_dev, Device_Addr, addr, dat);
    // HAL_I2C_Mem_Write(&hi2c3, Device_Addr, addr, I2C_MEMADD_SIZE_8BIT, &dat, 1, 10);
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_RESET
*	功能说明: 触摸屏复位
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_RESET(void)
{
    TOUCH_RST_0;
    HAL_Delay(10);
    TOUCH_RST_1;
    HAL_Delay(100);
}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_READ_X
*	功能说明: 读取触摸屏在触摸时的坐标值
*	形    参：无
*	返 回 值: 无 （数据存储在CST816_Instance结构体中）
*********************************************************************************************************
*/
void CST816_Get_XY_AXIS(void)
{
    uint8_t DAT[4] = {0};

    IIC_Read_Multi_Byte(&CST816_dev, Device_Addr, XposH, 4, DAT);
    // HAL_I2C_Mem_Read(&hi2c3, Device_Addr, XposH, I2C_MEMADD_SIZE_8BIT, DAT, 4, 10);
    CST816_Instance.X_Pos = (uint16_t)((DAT[0] & 0x0F) << 8) | DAT[1];                  //(temp[0]&0X0F)<<4|
    CST816_Instance.Y_Pos = (uint16_t)((DAT[2] & 0x0F) << 8) | DAT[3] + TOUCH_OFFSET_Y; //(temp[2]&0X0F)<<4|
    // if (CST816_Instance.Y_Pos < 160)
    // {
    //     CST816_Instance.Y_Pos += TOUCH_OFFSET_Y;
    // }
    //  #if REVERSE
    //      CST816_Instance.X_Pos = 239 - CST816_Instance.X_Pos;
    //      CST816_Instance.Y_Pos = 279 - CST816_Instance.Y_Pos;
    //  #endif
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Get_FingerNum
*	功能说明: 读取触摸屏的手指触摸个数,0xFF为睡眠
*	形    参：无
*	返 回 值: 返回芯片ID
*********************************************************************************************************
*/
uint8_t CST816_Get_FingerNum(void)
{
    return CST816_IIC_ReadREG(FingerNum);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Get_ChipID
*	功能说明: 读取触摸屏的芯片ID
*	形    参：无
*	返 回 值: 返回芯片ID
*********************************************************************************************************
*/
uint8_t CST816_Get_ChipID(void)
{
    return CST816_IIC_ReadREG(ChipID);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Config_MotionMask
*	功能说明: 使能连续动作（连续左右滑动，连续上下滑动，双击）
*	形    参：mode：模式(5种)
*	返 回 值: 无
*	注    意：使能连续动作会增加响应时间
*********************************************************************************************************
*/
void CST816_Config_MotionMask(uint8_t mode)
{
    CST816_IIC_WriteREG(MotionMask, mode);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Config_AutoSleepTime
*	功能说明: 规定time内无触摸，自动进入低功耗模式
*	形    参：time：时间(s)
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Config_AutoSleepTime(uint8_t time)
{
    CST816_IIC_WriteREG(AutoSleepTime, time);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Sleep
*	功能说明: 进入睡眠，无触摸唤醒功能
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Sleep(void)
{
    CST816_IIC_WriteREG(SleepMode, 0x03);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Wakeup
*	功能说明: 唤醒
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Wakeup(void)
{
    CST816_RESET();
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Config_MotionSlAngle
*	功能说明: 手势检测滑动分区角度控制。Angle=tan(c)*10 c为以x轴正方向为基准的角度。
*	形    参：x_right_y_up_angle：角度值
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Config_MotionSlAngle(uint8_t x_right_y_up_angle)
{
    CST816_IIC_WriteREG(MotionSlAngle, x_right_y_up_angle);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Config_NorScanPer
*	功能说明: 正常快速检测周期配置函数。
*						此值会影响到LpAutoWakeTime和AutoSleepTime。
*						单位10ms，可选值：1～30。默认值为1。
*	形    参：Period：周期值
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Config_NorScanPer(uint8_t Period)
{
    if (Period >= 30)
        Period = 30;
    CST816_IIC_WriteREG(NorScanPer, Period);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Config_IrqPluseWidth
*	功能说明: 中断低脉冲输出宽度配置函数
*	形    参：Period：周期值
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Config_IrqPluseWidth(uint8_t Width)
{
    if (Width >= 200)
        Width = 200;
    CST816_IIC_WriteREG(IrqPluseWidth, Width);
}

/*
*********************************************************************************************************
*	函 数 名: CST816_Config_NorScanPer
*	功能说明: 低功耗扫描唤醒门限配置函数。越小越灵敏。默认值48
*	形    参：TH：门限值
*	返 回 值: 无
*********************************************************************************************************
*/
void CST816_Config_LpScanTH(uint8_t TH)
{
    CST816_IIC_WriteREG(LpScanTH, TH);
}
