#ifndef _BSP_DELAY_H
#define _BSP_DELAY_H

#include "main.h"

extern void delay_init(void);
extern void delay_us(uint32_t nus);
extern void delay_ms(uint32_t nms);

#endif
