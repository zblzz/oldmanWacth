/**
 * @file   bsp_beep.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "bsp_beep.h"

/**
 * @brief  开启蜂鸣器
 * @author zbl (zzz)
 */
void beep_on(void)
{
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, (htim2.Init.Period - 1) / 2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

/**
 * @brief  关闭蜂鸣器
 * @author zbl (zzz)
 */
void beep_off(void)
{
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}