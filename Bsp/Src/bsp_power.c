/**
 * @file   bsp_power.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "bsp_power.h"
#include "bsp_delay.h"
#include "adc.h"

#define INTERNAL_RES 0.128
#define CHARGING_CUR 1

uint8_t ChargeCheck(void) // 1:charging
{
    return 0;
}

float BatCheck(void)
{
    uint16_t dat;
    float BatVoltage;
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 5);
    dat = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    BatVoltage = dat * 2 * 3.3 / 4096;
    return BatVoltage;
}

float BatCheck_8times(void)
{
    uint32_t dat = 0;
    uint8_t i;
    float BatVoltage;
    for (i = 0; i < 8; i++)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 5);
        dat += HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
        delay_ms(1);
    }
    dat = dat >> 3;
    BatVoltage = dat * 2 * 3.3 / 4096;
    return BatVoltage;
}

/**
 * @brief  电压测量
 * @return uint8_t:
 * @author zbl (zzz)
 */
uint8_t PowerCalculate(void)
{
    uint8_t power;
    float voltage;
    voltage = BatCheck_8times();

    if (ChargeCheck())
    {
        voltage -= INTERNAL_RES * CHARGING_CUR;
    }

    if ((voltage >= 4.2))
    {
        power = 100;
    }
    else if (voltage >= 4.06 && voltage < 4.2)
    {
        power = 90;
    }
    else if (voltage >= 3.98 && voltage < 4.06)
    {
        power = 80;
    }
    else if (voltage >= 3.92 && voltage < 3.98)
    {
        power = 70;
    }
    else if (voltage >= 3.87 && voltage < 3.92)
    {
        power = 60;
    }
    else if (voltage >= 3.82 && voltage < 3.87)
    {
        power = 50;
    }
    else if (voltage >= 3.79 && voltage < 3.82)
    {
        power = 40;
    }
    else if (voltage >= 3.77 && voltage < 3.79)
    {
        power = 30;
    }
    else if (voltage >= 3.74 && voltage < 3.77)
    {
        power = 20;
    }
    else if (voltage >= 3.68 && voltage < 3.74)
    {
        power = 10;
    }
    else if (voltage >= 3.45 && voltage < 3.68)
    {
        power = 5;
    }
    return power;
}

/**
 * @brief  关电源
 * @author zbl (zzz)
 */
void Power_DisEnable(void)
{
}

/**
 * @brief  进入低功耗模式
 * @author zbl (zzz)
 */
void Power_Low_Mode(void)
{
    __WFI();
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

/**
 * @brief  进入正常模式
 * @author zbl (zzz)
 */
void Power_Normal_Mode(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    SystemClock_Config();
}