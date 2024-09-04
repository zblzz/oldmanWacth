/**
 * @file   bsp_key.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "bsp_key.h"
#include "user_TasksInit.h"
#include "bsp_delay.h"

void Key_Port_Init(void)
{
    // GPIO_InitTypeDef GPIO_InitStruct = {0};

    // __HAL_RCC_GPIOB_CLK_ENABLE();

    // GPIO_InitStruct.Pin = KEY1_PIN;
    // GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    // GPIO_InitStruct.Pull = GPIO_PULLUP;
    // HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);

    // /* EXTI interrupt init*/
    // HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/**
 * @brief 按键扫描
 * @param  mode:    0：复位
 * @return uint8_t:
 * @author zbl (zzz)
 */
uint8_t KeyScan(uint8_t mode)
{
    static uint8_t key_up = 1;
    static uint8_t key_down = 0;
    uint8_t keyvalue = 0;

    if (mode)
    {
        key_up = 1;
        key_down = 0;
    }

    if (key_up && !KEY1)
    {
        osDelay(3); // ensure the key
        if (!KEY1)
            key_down = 1;
        // if (KEY2)
        //     key_down = 2;
        if (key_down)
            key_up = 0;
    }

    if (key_down && KEY1)
    {
        osDelay(3); // ensure the key
        if (KEY1)
        {
            key_up = 1;
            keyvalue = key_down;
            key_down = 0;
        }
    }

    return keyvalue;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY1_PIN)
    {
        static uint16_t key1_long_press;
        if (!KEY1)
        {
            key1_long_press += 1;
            if (key1_long_press >= 1000)
            {
                // Power_DisEnable();
            }
        }
        else
        {
            key1_long_press = 0;
        }

        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
    }
}