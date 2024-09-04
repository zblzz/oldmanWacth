/**
 * @file   lsm.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "lsm.h"
#include "log.h"
#include <string.h>
#include "math.h"

static void lsm_WristgetAngles(float *roll, float *pitch, float *yaw);

/**
 * @brief  陀螺仪初始化
 * @author zbl (zzz)
 */
void lsm_init(void)
{
    g_lsm6ds3.Init();       // 陀螺仪初始化
    g_lsm6ds3.testoffset(); // 测试陀螺仪偏移
}

/**
 * @brief
 * @param  roll:
 * @param  pitch:
 * @param  yaw:
 * @author zbl (zzz)
 */
static void lsm_WristgetAngles(float *roll, float *pitch, float *yaw)
{
    int16_t *buff = NULL;
    buff = g_lsm6ds3.ReadAccelerationRaw();

    int16_t ax, ay, az;
    ax = buff[0];
    ay = buff[1];
    az = buff[2];

    *pitch = -atanf(ax / sqrtf(ay * ay + az * az));
    *roll = atanf((float)ay / (float)az);
    *yaw = atanf((float)ax / (float)az);
}

/**
 * @brief  判断是否水平
 * @return uint8_t:
 * @author zbl (zzz)
 */
uint8_t lsm_isHorizontal(void)
{
    float roll, pitch, yaw;
    lsm_WristgetAngles(&pitch, &roll, &yaw);
    // EMlog(LOG_INFO, "roll:%.2f pitch:%.2f yaw:%.2f\n", roll, pitch, yaw);
    if (roll <= 0.50 && roll >= -0.50 && pitch <= 0.50 && pitch >= -0.50)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief  获取角加速度
 * @param  roll:
 * @param  pitch:
 * @param  yaw:
 * @author zbl (zzz)
 */
void lsm_getAcceleration(float *ax, float *ay, float *az)
{
    g_lsm6ds3.getacc(ax, ay, az);
}
