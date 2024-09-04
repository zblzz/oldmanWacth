#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "main.h"

// KEY1
#define KEY1_PORT GPIOB
#define KEY1_PIN GPIO_PIN_0
#define KEY1 HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN)

void Key_Port_Init(void);
// void Key_Interrupt_Callback(void);
uint8_t KeyScan(uint8_t mode);

#endif
