#ifndef __LSM_H__
#define __LSM_H__

#include "main.h"
#include "bsp_lsm.h"

void lsm_init(void);
uint8_t lsm_isHorizontal(void);
void lsm_getAcceleration(float *ax, float *ay, float *az);

#endif