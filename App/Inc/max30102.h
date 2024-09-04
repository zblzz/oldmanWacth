#ifndef __MAX30102_H
#define __MAX30102_H

#include "main.h"

void max30102_cal_data(int *dis_hr, int *dis_spo2); // 得到心率血氧数据
void max30102_get_data(int *dis_hr, int *dis_spo2);
void max30102_get_sample(void);

void max30102_enable(void);
void max30102_disable(void);

extern uint8_t sendhr_flag;
#endif
