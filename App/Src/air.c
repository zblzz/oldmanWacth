/**
 * @file   air.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "max30102.h"
#include "air.h"

#include "usart.h"
#include "bsp_delay.h"
#include "HWDataAccess.h"
#include "log.h"

// uint8_t loc[200];      // 存信息数组
// uint8_t ii = 0, r = 0; // 完成标志位，第r元素,是否接收
// uint8_t r_flag = 1;
// uint8_t rx_buf[200]; // 接收缓存数组

uint8_t hr_warnflag = 0;
struct airTime time;

void air_send_warn(int air_warn)
{
    unsigned char msg_buf[256]; // 缓存区
    sprintf((char *)msg_buf, AIR_SEND_warn, air_warn);
    HAL_UART_Transmit(&huart2, (unsigned char *)msg_buf, strlen((const char *)msg_buf), 1000);
}

double cifang(int ii, int flag) // 传入的次方数，标志位是1整数计算，0是小数计算
{
    double result = 1;
    int t = 0;
    if (flag == 1)
    {
        for (t = 1; t <= ii; t++)
            result *= 10;

        return result;
    }
    else
    {
        for (t = 1; t <= ii; t++)
            result /= 10;
        return result;
    }
}

void air_data_trans(double *jing, double *wei)
{
    double xishu = 1;
    int ii = 0;

    *jing = (air_receive_buf[19] - '0') * 100 + (air_receive_buf[20] - '0') * 10 + (air_receive_buf[21] - '0') * 1;

    // 经度转换(小数部分）
    for (ii = 23; ii < 30; ii++)
    {
        xishu = cifang(ii - 22, 0);
        *jing += ((air_receive_buf[ii] - '0') * (xishu));
    }

    xishu = 1;
    // 纬度转换(整数部分）
    for (ii = 31; ii < 34; ii++)
    {
        xishu = cifang(ii - 31, 1);
        *wei += ((air_receive_buf[ii] - '0') * (100 / xishu));
    }
    // 经度转换(小数部分）
    for (ii = 35; ii < 42; ii++)
    {
        xishu = cifang(ii - 34, 0);
        *wei += ((air_receive_buf[ii] - '0') * (xishu));
    }

    EMlog(LOG_DEBUG, "jing%f\r\n wei:%f\r\n", *jing, *wei);
}

void receive_clear(void)
{
    memset(air_receive_buf, 0x00, 100);
    //	receive_count=0;
}

void air_get_location(void) // 如果传1就接收
{
    //	receive_clear();
    HAL_UART_Transmit(&huart2, (unsigned char *)AIR_GET_LOC, strlen((const char *)AIR_GET_LOC), 1000); // 使用基站定位
}

void air_get_time(void) // 如果传1就接收
{
    //	receive_clear();
    HAL_UART_Transmit(&huart2, (unsigned char *)AIR_GET_TIME, strlen((const char *)AIR_GET_TIME), 1000); // 获取时间
}

/**
 * @brief  解析时间
 * @author zbl (zzz)
 */
void air_trans_time(void)
{
    int Year, Month, Date, Hour, Minute, Second, Millisecond = 0;

    sscanf((const char *)air_receive_buf, "\r\nconfig,nettime,ok,%d,%d,%d,%d,%d,%d,%d\r\n",
           &Year, &Month, &Date, &Hour, &Minute, &Second, &Millisecond);

    time.year = Year % 2000;
    time.month = Month;
    time.date = Date;
    time.hour = Hour;
    time.minute = Minute;
    time.second = Second;
    time.millisecond = Millisecond;

    EMlog(LOG_DEBUG, "year: %d\r\nmonth: %d\r\ndate: %d\r\nhour: %d\r\nminute: %d\r\nsencond: %d\r\nmillisecond: %d\r\n",
          time.year, time.month, time.date, time.hour, time.minute, time.second, time.millisecond);
}

uint8_t countt = 0;
uint32_t sum_hr = 0;
float sum_spo2 = 0;

void air_send_hr(int dis_hr, int dis_spo2)
{
    unsigned char msg_buf[256]; // 缓存区

    sprintf((char *)msg_buf, AIR_SEND_hr, dis_hr, dis_spo2);

    HAL_UART_Transmit(&huart2, (unsigned char *)msg_buf, strlen((const char *)msg_buf), 1000);
}

void air_send_location(void)
{
    unsigned char msg_buf[256]; // 缓存区
    // double jing, wei;           // 定义经纬度

    // air_data_trans(&jing, &wei);

    // // 上传阿里云
    // if (jing < 120 && wei < 30)
    // {
    //     sprintf((char *)msg_buf, AIR_SEND_LOC, jing, wei);
    //     HAL_UART_Transmit(&huart2, (unsigned char *)msg_buf, strlen((const char *)msg_buf), 1000);
    // }

    sprintf((char *)msg_buf, AIR_SEND_LOC, 118.654136, 32.089769);
    HAL_UART_Transmit(&huart2, (unsigned char *)msg_buf, strlen((const char *)msg_buf), 1000);
}

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {

//     if (huart->Instance == USART2)
//     {
//         if (r_flag == 1)
//         {
//             HAL_UART_Receive_IT(huart, (uint8_t *)rx_buf, 1);
//             //			printf("%s",rx_buf);
//             air_receive_buf[r] = rx_buf[0]; // 因为他每次都需要进一次中断，所以每次都往receive_buf里面填充
//             r++;                        // 每次receive_buf的元素+1
//             ii = 1;                     // 接收完成标志位
//         }
//     }
// }

static uint8_t get_air_receive_flag(void)
{
    return air_receive_flag;
}

/**
 * @brief  云初始化
 * @author zbl (zzz)
 */
uint8_t air_init(void)
{
    for (int i = 0; i < 5; i++)
    {
        uint8_t flag = 0;
        air_receive_flag = 0;
        // air_get_location();
        // while (flag == 0)
        // {
        //     flag = get_air_receive_flag();
        //     delay_ms(100); // 减少轮询的频率
        // }
        // if (air_receive_len == 0)
        //     return 1;
        EMlog(LOG_DEBUG, "buff: %s", air_receive_buf);
        air_send_location();
        delay_ms(100);
        air_send_warn(0);
        memset(air_receive_buf, 0, sizeof(air_receive_buf));

        delay_ms(500);

        flag = 0;
        air_receive_flag = 0;
        air_get_time();
        while (flag == 0)
        {
            flag = get_air_receive_flag();
            delay_ms(200); // 减少轮询的频率
        }
        // if (air_receive_len == 0)
        //     return 1;
        EMlog(LOG_DEBUG, "buff: %s", air_receive_buf);
        air_trans_time();
        memset(air_receive_buf, 0, sizeof(air_receive_buf));
    }
    HW_Air_Change_TimeDate(time.year, time.month, time.date, time.hour, time.minute, time.second);

    return 0;
}
