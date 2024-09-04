/**
 * @file   bsp_lsm.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "bsp_lsm.h"
#include <string.h>
#include "log.h"
#include "math.h"
#include "bsp_delay.h"

#define PI acos(-1); // 定义PI

/* 私有函数声明区域 */
static void Lsm6ds3_Init(void);
static uint8_t Lsm6ds3_WriteByte(uint8_t reg, uint8_t data);
static uint8_t Lsm6ds3_ReadByte(uint8_t reg);
static uint16_t Lsm6ds3_ReadMultiple(uint8_t reg_addr_begin, uint8_t reg_addr_end, uint8_t *data);
static void test_offset(void);
static void get_acc(float *ax, float *ay, float *az);
static void get_angle(float *pitch, float *roll, float *yaw);

uint8_t Lsm6ds3_ReadOrigin(void);
uint8_t Lsm6ds3_ReadWhoAmI(void);
int16_t *Lsm6ds3_ReadAccelerationRaw();
int16_t *Lsm6ds3_ReadAngularRateRaw();
int16_t *Lsm6ds3_ReadTemperatureRaw();

// lsm6ds3 i2c总线设备
#define LSM_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE()
i2c_bus_t LSM_dev =
    {
        .i2c_sda_port = GPIOB,
        .i2c_scl_port = GPIOB,
        .i2c_sda_pin = GPIO_PIN_11,
        .i2c_scl_pin = GPIO_PIN_10,
};

/* lsm6ds3传感器对象初始化 */
sLSM6DS3_Dev g_lsm6ds3 =
    {
        .handle = NULL,
        .m_dataBuf = {0},
        .slaveAddress = LSM6DS3_SlaveAddress,
        .deviceID = LSM6DS3_DeviceID,

        .Init = Lsm6ds3_Init,
        .testoffset = test_offset,
        .getangle = get_angle,
        .getacc = get_acc,
        .ReadByte = Lsm6ds3_ReadByte,
        .WriteByte = Lsm6ds3_WriteByte,
        .ReadMultiple = Lsm6ds3_ReadMultiple,

        .ReadOrigin = Lsm6ds3_ReadOrigin,
        .ReadWhoAmI = Lsm6ds3_ReadWhoAmI,
        .ReadAngularRateRaw = Lsm6ds3_ReadAngularRateRaw,
        .ReadAccelerationRaw = Lsm6ds3_ReadAccelerationRaw,
        .ReadTemperatureRaw = Lsm6ds3_ReadTemperatureRaw,
};

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param[in]  handle    customizable argument. In this examples is used in
 *                       order to select the correct sensor bus handler.
 * @param[in]  reg       register to read
 * @param[in]  bufp      pointer to buffer that store the data read
 * @param[in]  len       number of consecutive register to read
 *
 */
// static I2C_Status platform_ReadByte(void *handle, uint8_t slaveAddress, uint8_t reg, uint8_t *bufp, uint16_t len)
//{
//     IIC_ReadBytes(slaveAddress, reg, bufp, len);
// }

///*
// * @brief  Write generic device register (platform dependent)
// *
// * @param[in]  handle    customizable argument. In this examples is used in
// *                   order to select the correct sensor bus handler.
// * @param[in]  reg       register to write
// * @param[in]  bufp      pointer to data to write in register reg
// * @param[in]  len       number of consecutive register to write
// *
// */
// static I2C_Status platform_WriteByte(void *handle, uint8_t slaveAddress, uint8_t reg, uint8_t *bufp, uint16_t len)
//{
//	return HAL_I2C_Mem_Write(handle, slaveAddress, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t *)bufp, len, 1000);
//}

/**
 * @brief      写入一个字节
 *
 * @param[in]  reg   寄存器地址
 * @param[in]  data  待写入的数据
 */
static uint8_t Lsm6ds3_WriteByte(uint8_t reg, uint8_t data)
{
    uint8_t ret = IIC_Write_One_Byte(&LSM_dev, g_lsm6ds3.slaveAddress >> 1, reg, data);
    //    EMlog(LOG_DEBUG, "Lsm6ds3_write   %x\r\n", data);
    if (ret != 0)
    {
        EMlog(LOG_ERROR, "write error\n");
        return 1;
    }
    return ret;
}

/**
 * @brief      读取一个字节
 *
 * @param[in]  reg   寄存器地址
 *
 * @return     返回读取到的数据
 */
static uint8_t Lsm6ds3_ReadByte(uint8_t reg)
{
    uint8_t data = 0;
    if (IIC_Read_One_Byte(&LSM_dev, g_lsm6ds3.slaveAddress >> 1, reg, &data))
    {
        EMlog(LOG_ERROR, "read error\n");
        g_lsm6ds3.m_dataBuf.uReaddata = 0xff;
    }

    g_lsm6ds3.m_dataBuf.uReaddata = data;

    return g_lsm6ds3.m_dataBuf.uReaddata;
}

/**
 * @brief      连续地址读取传感器参数/数据
 *
 * @param[in]  reg_addr_begin  The register address begin
 * @param[in]  reg_addr_end    The register address end
 * @param[out] data            The data
 *
 * @return     返回读取的字节数
 */
static uint16_t Lsm6ds3_ReadMultiple(uint8_t reg_addr_begin, uint8_t reg_addr_end, uint8_t *data)
{
    uint16_t nCount = 0;
    if (reg_addr_begin > reg_addr_end)
    {
        EMlog(LOG_ERROR, "register address invalid!\n");
        return 0;
    }

    while (nCount < (reg_addr_end - reg_addr_begin + 1))
    {
        data[nCount] = Lsm6ds3_ReadByte(reg_addr_begin + nCount);
        nCount++;
        delay_ms(1);
    }

    return nCount;
}

/**
 * @brief      初始化陀螺仪和加速度传感器
 */
static void Lsm6ds3_Init(void)
{
    LSM_CLK_ENABLE;
    IIC_Init(&LSM_dev);

    uint8_t whoAmI = Lsm6ds3_ReadByte(LSM6DS3_WHO_AM_I);
    EMlog(LOG_DEBUG, "Lsm6ds3_Init[G-SensorId] -> 0x%x\n", whoAmI);
    if (whoAmI != LSM6DS3_DeviceID)
    {
        EMlog(LOG_ERROR, "read who am i failed!\n");
        return;
    }
    // 加速度计52HZ（倾斜角检测功能工作在26HZ，因此加速度计ODR必须设置为>=26hz）,2g量程。
    Lsm6ds3_WriteByte(LSM6DS3_CTRL1_XL, 0x20);
    // 使能加速度计x,y,z轴
    Lsm6ds3_WriteByte(LSM6DS3_CTRL9_XL, 0x38);
    // enable accelerometer int1
    Lsm6ds3_WriteByte(LSM6DS3_INT1_CTRL, 0x01);

    // 陀螺仪208hz  2000dps
    Lsm6ds3_WriteByte(LSM6DS3_CTRL2_G, 0x5C);
    // 使能陀螺仪x,y,z轴
    Lsm6ds3_WriteByte(LSM6DS3_CTRL10_C, 0x38);

    // WAKE_UP INTERRUPT Configuration
    Lsm6ds3_WriteByte(LSM6DS3_TAP_CFG, 0x90);
    Lsm6ds3_WriteByte(LSM6DS3_WAKE_UP_DUR, 0x00);
    Lsm6ds3_WriteByte(LSM6DS3_WAKE_UP_THS, 0x02);
    Lsm6ds3_WriteByte(LSM6DS3_MD1_CFG, 0x20);

    // 6D Orientation Configuration
    Lsm6ds3_WriteByte(LSM6DS3_TAP_THS_6D, 0x40);
    Lsm6ds3_WriteByte(LSM6DS3_CTRL8_XL, 0x01);

    EMlog(LOG_DEBUG, "Lsm6ds3 init successful!\n");
}

/****************************************** 应用接口 *********************************************/
#ifndef USER_APP_INTERFACE
/**
 * @brief      获取传感器坐标零点
 *
 * @return     见#define LSM6DS3_D6D_SRC
 */
uint8_t Lsm6ds3_ReadOrigin(void)
{
    g_lsm6ds3.m_dataBuf.orientation = Lsm6ds3_ReadByte(LSM6DS3_D6D_SRC);
    return g_lsm6ds3.m_dataBuf.orientation;
}

/**
 * @brief      获取芯片ID
 *
 * @return     返回芯片ID
 */
uint8_t Lsm6ds3_ReadWhoAmI(void)
{
    g_lsm6ds3.m_dataBuf.whoamI = Lsm6ds3_ReadByte(LSM6DS3_WHO_AM_I);
    return g_lsm6ds3.m_dataBuf.whoamI;
}

/**
 * @brief      读取角速度寄存器原始值
 *
 * @param[out] pbuf  The pbuf
 *
 * @return     返回角速度寄存器原始值（带符号数值）
 */
int16_t *Lsm6ds3_ReadAngularRateRaw()
{
    // 读取寄存器值使用无符号类型，读取后取值再转为有符号
    uint8_t buf[6] = {0};

    if ((g_lsm6ds3.ReadByte(LSM6DS3_STATUS_REG) & STATUS_GDA_GYRO_E) != 0)
    {
        Lsm6ds3_ReadMultiple(LSM6DS3_OUTX_L_G, LSM6DS3_OUTZ_H_G, buf);
        g_lsm6ds3.m_dataBuf.data_raw_angular_rate[0] = (buf[1] << 8) | buf[0];
        g_lsm6ds3.m_dataBuf.data_raw_angular_rate[1] = (buf[3] << 8) | buf[2];
        g_lsm6ds3.m_dataBuf.data_raw_angular_rate[2] = (buf[5] << 8) | buf[4];
    }

    return g_lsm6ds3.m_dataBuf.data_raw_angular_rate;
}

/**
 * @brief      读取加速度寄存器原始值
 *
 * @param[out] pbuf  The pbuf
 *
 * @return     返回加速度寄存器原始值（带符号数值）
 */
int16_t *Lsm6ds3_ReadAccelerationRaw()
{
    // 读取寄存器值使用无符号类型，读取后取值再转为有符号
    uint8_t buf[6] = {0};
    //	EMlog(LOG_DEBUG, "\r\n%x\r\n",Lsm6ds3_ReadByte(LSM6DS3_STATUS_REG));
    if ((g_lsm6ds3.ReadByte(LSM6DS3_STATUS_REG) & STATUS_XLDA_ACC_E) != 0)
    {
        Lsm6ds3_ReadMultiple(LSM6DS3_OUTX_L_XL, LSM6DS3_OUTZ_H_XL, buf);
        g_lsm6ds3.m_dataBuf.data_raw_acceleration[0] = (buf[1] << 8) | buf[0];
        g_lsm6ds3.m_dataBuf.data_raw_acceleration[1] = (buf[3] << 8) | buf[2];
        g_lsm6ds3.m_dataBuf.data_raw_acceleration[2] = (buf[5] << 8) | buf[4];
    }

    return g_lsm6ds3.m_dataBuf.data_raw_acceleration;
}

/**
 * @brief      读取温度寄存器原始值
 *
 * @param[out] pbuf  The pbuf
 *
 * @return     返回温度存器原始值（带符号数值）
 */
int16_t *Lsm6ds3_ReadTemperatureRaw()
{
    // 读取寄存器值使用无符号类型，读取后取值再转为有符号
    uint8_t buf[2] = {0};

    if ((g_lsm6ds3.ReadByte(LSM6DS3_STATUS_REG) & STATUS_TDA_TEMP_E) != 0)
    {
        Lsm6ds3_ReadMultiple(LSM6DS3_OUT_TEMP_L, LSM6DS3_OUT_TEMP_H, buf);
        g_lsm6ds3.m_dataBuf.data_raw_temperature = (buf[1] << 8) | buf[0];
    }

    return &(g_lsm6ds3.m_dataBuf.data_raw_temperature);
}

/**
 * @brief      读取加速度
 *
 * @param[out] pbuf  The pbuf
 *
 * @return     返回加速度
 */
static void get_acc(float *ax, float *ay, float *az)
{
    int16_t a_buf[3] = {0};
    Lsm6ds3_ReadAccelerationRaw();

    *ax = 2 * 9.8 * g_lsm6ds3.m_dataBuf.data_raw_acceleration[0] / 32768;
    *ay = 2 * 9.8 * g_lsm6ds3.m_dataBuf.data_raw_acceleration[1] / 32768;
    *az = 2 * 9.8 * g_lsm6ds3.m_dataBuf.data_raw_acceleration[2] / 32768;
}

/**
 * @brief      测试加速度的原始角度与漂移
 *
 * @param[out] void
 *
 * @return     void
 */
static int offset_ax = 0, offset_ay = 0, offset_az = 0; // 加速度的漂移值
static float angle_offset1;                             // x加速度的原始角度
static float angle_offset2;                             // y加速度的原始角度
static float angle_offset3;                             // z加速度的原始角度
#define TEST_NUMS 500

static void test_offset(void)
{
    int tempx = 0, tempy = 0, tempz = 0;
    uint16_t i;
    short ax, ay, az;
    int16_t *a_buff = NULL;

    for (i = 0; i < TEST_NUMS; i++)
    {                                           // 取500次的XY轴加速值，进行累加，消除飘移
        a_buff = Lsm6ds3_ReadAccelerationRaw(); // 通过mpu6050拿取加速度的值
        ax = a_buff[0];
        ay = a_buff[1];
        az = a_buff[2];
        tempx += ax;
        tempy += ay;
        tempz += az;
        delay_ms(10); // 延时10ms
    }
    offset_ax = tempx / TEST_NUMS;
    offset_ay = tempy / TEST_NUMS;
    offset_az = tempz / TEST_NUMS;
    angle_offset1 = atan2(offset_ay, offset_ax) * 180 / PI; // 算出最初始的角度
    angle_offset2 = atan2(offset_ax, offset_az) * 180 / PI; // 算出最初始的角度

    angle_offset3 = atan2(offset_ay, offset_az) * 180 / PI; // 算出最初始的角度
    // EMlog(LOG_DEBUG, "%lf", get_angle());                             // 拿取一次角度，判断测量效果
    EMlog(LOG_DEBUG, "angle_offset1:%f, angle_offset2:%f, angle_offset3:%f\r\n", angle_offset1, angle_offset2, angle_offset3);
}

/**
 * @brief      get_angle
 *
 * @param[out] 获取机器的倾斜角度
 *
 * @return     倾斜角度
 */

static void get_angle(float *pitch, float *roll, float *yaw)
{
    static int acc_x[5] = {0}; // 此数组用于存放多个X或Y轴的加速度，类似滤波，主要为了防止加速度的突变造成测量不准确
    static int acc_y[5] = {0}; // 可以适当加减该数组的成员数，增加成员数能使角度变化更细腻，
    static int acc_z[5] = {0}; // 减少成员数会使角度变化反应更迅速，需自己去测试一个中间值
    int16_t a_bufff[3] = {0};
    int x_num = 0, y_num = 0, z_num = 0;
    float angle[3];
    int16_t ax, ay, az;
    uint8_t i;

    for (i = 0; i < 5; i++)
    { // 如果数组没有拿到5个值，就先拿满五个值 这儿都是第一次才会直接拿五个
        if (acc_x[i] == 0)
        {
            Lsm6ds3_ReadAccelerationRaw();
            acc_x[i] = g_lsm6ds3.m_dataBuf.data_raw_acceleration[0];
            acc_y[i] = g_lsm6ds3.m_dataBuf.data_raw_acceleration[1];
            acc_z[i] = g_lsm6ds3.m_dataBuf.data_raw_acceleration[2];
        }
    }
    for (i = 0; i < 5; i++)
    { // 开始累加5个值
        x_num += acc_x[i];
        y_num += acc_y[i];
        z_num += acc_z[i];
    }
    for (i = 0; i < 4; i++)
    { // 拿掉成员0的值，其余的值依次前进一位
        acc_x[i] = acc_x[i + 1];
        acc_y[i] = acc_y[i + 1];
        acc_z[i] = acc_z[i + 1];
    }
    ax = (x_num) / 5;
    ay = (y_num) / 5;
    az = (z_num) / 5;
    //	if(ax==0||ay==0||az==0)
    //	{
    //		EMlog(LOG_DEBUG, "get  err \r\n");
    //		return;
    //	}
    angle[0] = atan2(ay, ax) * 180 / PI; // 计算角度
    angle[1] = atan2(ax, az) * 180 / PI; // 计算角度

    angle[2] = atan2(ay, az) * 180 / PI; // 计算角度
    //	EMlog(LOG_DEBUG, "a1origin:%f\r\n",angle[0] );

    if (ay >= 0)
    {                                                // 判断角度的正反（即往左还是往右偏），此代码中左偏为负，右偏为正
        angle[0] = (angle[0] - fabs(angle_offset1)); // 此时的值减去初始值，就是当前的倾斜角度
    }
    else
    {
        if (angle_offset1 < 0)
        { // 防止360°反偏
            angle[0] = (angle[0] - angle_offset1);
        }
        else
        {
            angle[0] = (angle[0] + angle_offset1);
        }
    }

    //	EMlog(LOG_DEBUG, "offf:%f\r\n",fabs(angle_offset1) );

    //	EMlog(LOG_DEBUG, "a1:%f\r\n",angle[0] );
    if (ax >= 0)
    {                                                // 判断角度的正反（即往左还是往右偏），此代码中左偏为负，右偏为正
        angle[1] = (angle[1] - fabs(angle_offset2)); // 此时的值减去初始值，就是当前的倾斜角度
    }
    else
    {
        if (angle_offset2 < 0)
        { // 防止360°反偏
            angle[1] = (angle[1] - angle_offset2);
        }
        else
        {
            angle[1] = (angle[1] + angle_offset2);
        }
    }

    if (ay >= 0)
    {                                                // 判断角度的正反（即往左还是往右偏），此代码中左偏为负，右偏为正
        angle[2] = (angle[2] - fabs(angle_offset3)); // 此时的值减去初始值，就是当前的倾斜角度
    }
    else
    {
        if (angle_offset3 < 0)
        { // 防止360°反偏
            angle[2] = (angle[2] - angle_offset3);
        }
        else
        {
            angle[2] = (angle[2] + angle_offset3);
        }
    }

    acc_x[4] = 0; // 成员4赋值0
    acc_y[4] = 0;
    acc_z[4] = 0;
    x_num = 0;
    y_num = 0;
    z_num = 0;

    *pitch = angle[0];
    *roll = angle[1];
    *yaw = angle[2];
}

#else
__weak uint8_t Lsm6ds3_ReadOrigin(void)
{
    return 0xff;
}

__weak uint8_t Lsm6ds3_ReadWhoAmI(void)
{
    return 0xff;
}

__weak int16_t *Lsm6ds3_ReadAngularRateRaw(int16_t *pbuf)
{
    return NULL;
}

__weak int16_t *Lsm6ds3_ReadAccelerationRaw(int16_t *pbuf)
{
    return NULL;
}

__weak int16_t *Lsm6ds3_ReadTemperatureRaw(int16_t *pbuf)
{
    return NULL;
}
#endif
