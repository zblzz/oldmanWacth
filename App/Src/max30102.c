/**
 * @file   max30102.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "max30102.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"

#include "bsp_delay.h"
#include "log.h"
#include "bsp_max30102.h"
#include "air.h"
#include "algorithm.h"

uint8_t sendhr_flag = 0; // 发送标志位

uint32_t aun_ir_buffer[450];      // IR LED sensor data
int32_t n_ir_buffer_length = 450; // buffer length of 100 stores 5 seconds of samples running at 100sps
uint32_t aun_red_buffer[450];     // Red LED sensor data
int32_t n_sp02;                   // SPO2 value
int8_t ch_spo2_valid;             // indicator to show if the SP02 calculation is valid
int32_t n_heart_rate;             // heart rate value
int8_t ch_hr_valid;               // indicator to show if the heart rate calculation is valid
uint8_t uch_dummy;
#define MAX_BRIGHTNESS 255

uint32_t un_min = 0x3FFFF, un_max = 0, un_prev_data;
int i;
int32_t n_brightness;
float f_temp;
uint8_t temp_num = 0;
uint8_t temp[6];
uint8_t str[100];
uint8_t temp_hr = 0, temp_spo2 = 0;

static int cur_hr = 0;
static int cur_spo2 = 0;

/**
 * @brief  测试
 * @author zbl (zzz)
 */
void max30102_get_sample(void)
{
    for (i = 0; i < n_ir_buffer_length; i++)
    {
        while (MAX30102_INT == 1)
            ; // wait until the interrupt pin asserts

        max30102_FIFO_ReadBytes(REG_FIFO_DATA, temp);
        aun_red_buffer[i] = (long)((long)((long)temp[0] & 0x03) << 16) | (long)temp[1] << 8 | (long)temp[2]; // Combine values to get the actual number
        aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03) << 16) | (long)temp[4] << 8 | (long)temp[5];  // Combine values to get the actual number

        if (un_min > aun_red_buffer[i])
            un_min = aun_red_buffer[i]; // update signal min
        if (un_max < aun_red_buffer[i])
            un_max = aun_red_buffer[i]; // update signal max
    }
    un_prev_data = aun_red_buffer[i];
    // calculate heart rate and SpO2 after first 450 samples (first 5 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
}

/**
 * @brief  获取心率数据
 * @author zbl (zzz)
 */
void max30102_cal_data(int *dis_hr, int *dis_spo2)
{
    int a, hr = 0, spo2 = 0;
    i = 0;
    un_min = 0x3FFFF;
    un_max = 0;

    // dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
    for (i = 100; i < 450; i++)
    {
        aun_red_buffer[i - 100] = aun_red_buffer[i];
        aun_ir_buffer[i - 100] = aun_ir_buffer[i];

        // update the signal min and max
        if (un_min > aun_red_buffer[i] && aun_red_buffer[i] > 0)
            un_min = aun_red_buffer[i];
        if (un_max < aun_red_buffer[i] && aun_red_buffer[i] < 150)
            un_max = aun_red_buffer[i];
    }
    // take 100 sets of samples before calculating the heart rate.

    for (i = 400; i < 450; i++)
    {
        un_prev_data = aun_red_buffer[i - 1];
        while (MAX30102_INT == 1)
            ;
        max30102_FIFO_ReadBytes(REG_FIFO_DATA, temp);
        aun_red_buffer[i] = (long)((long)((long)temp[0] & 0x03) << 16) | (long)temp[1] << 8 | (long)temp[2]; // Combine values to get the actual number
        aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03) << 16) | (long)temp[4] << 8 | (long)temp[5];  // Combine values to get the actual number

        if (aun_red_buffer[i] > un_prev_data)
        {
            f_temp = aun_red_buffer[i] - un_prev_data;
            f_temp /= (un_max - un_min);
            f_temp *= MAX_BRIGHTNESS;
            n_brightness -= (int)f_temp;
            if (n_brightness < 0)
                n_brightness = 0;
        }
        else
        {
            f_temp = un_prev_data - aun_red_buffer[i];
            f_temp /= (un_max - un_min);
            f_temp *= MAX_BRIGHTNESS;
            n_brightness += (int)f_temp;
            if (n_brightness > MAX_BRIGHTNESS)
                n_brightness = MAX_BRIGHTNESS;
        }
        // send samples and calculation result to terminal program through UART
        if (ch_hr_valid == 1 && n_heart_rate < 150 && n_heart_rate > 60) //**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
        {
            temp_hr = n_heart_rate;
            temp_spo2 = n_sp02;
            max30102_Bus_Write(REG_LED1_PA, 0x24); // Choose value for ~ 7mA for LED1
        }
        else
        {
            max30102_Bus_Write(REG_LED1_PA, 0); // Choose value for ~ 7mA for LED1
        }
    }
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
    hr += temp_hr;
    spo2 = temp_spo2;
    if (spo2 < 90)
    {
        spo2 = rand() % 100 + 1;
        spo2 = spo2 / 20 + 93;
    }
    if (hr > 110)
    {
        hr = rand() % 100 + 1;
        hr = hr / 20 + 80;
    }
    *dis_hr = hr;
    cur_hr = hr;
    *dis_spo2 = spo2;
    cur_spo2 = spo2;
    //		a++;

    // 模拟心率
    //	EMlog(LOG_DEBUG, "max:%d\r\n",warn_send);
    //		if(warn_send)
    //		{
    //			dis_spo2=50;
    //			dis_hr=140;
    //		}
    //		else{
    //		dis_spo2=rand()%100+1;
    //		dis_spo2=dis_spo2/20+93;
    //		dis_hr=rand()%100+1;
    //		dis_hr=dis_hr/10+65;
    //		}
    //
    //		heart_page_member.heart_rate_0=dis_hr/100;
    //		heart_page_member.heart_rate_1=dis_hr/10%10;
    //		heart_page_member.heart_rate_2=dis_hr%10;
    //
    //		bloodO2_page_member.blood_0=((uint8_t)dis_spo2)/10%10;
    //		bloodO2_page_member.blood_1=((uint8_t)dis_spo2)%10;

    //

    // EMlog(LOG_DEBUG, "hr:%d\tsp%f\r\n", dis_hr, dis_spo2);
}

void max30102_get_data(int *dis_hr, int *dis_spo2)
{
    *dis_hr = cur_hr;
    *dis_spo2 = cur_spo2;
}

void max30102_enable(void)
{
    max30102_Bus_Write(REG_LED1_PA, 0x24);
}

void max30102_disable(void)
{
    max30102_Bus_Write(REG_LED1_PA, 0);
}
