/**
 * @file   user_SensorPageTask.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
/* Private includes -----------------------------------------------------------*/
// includes
#include "user_TasksInit.h"
#include "user_ScrRenewTask.h"
#include "user_SensorPageTask.h"
#include "ui_HRPage.h"
#include "ui_SPO2Page.h"
#include "ui_ENVPage.h"
#include "ui_CompassPage.h"
#include "ui_HomePage.h"
#include "main.h"

#include "stdlib.h"
#include "max30102.h"
// #include "AHT21.h"
// #include "LSM303.h"
// #include "SPL06_001.h"
// #include "em70x8.h"
// #include "HrAlgorythm.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint32_t user_HR_timecount = 0;

/* Private function prototypes -----------------------------------------------*/
// extern uint8_t GET_BP_MAX(void);
// extern uint8_t GET_BP_MIN(void);
// extern void Blood_Process(void);
// extern void Blood_50ms_process(void);
// extern void Blood_500ms_process(void);
// extern int em70xx_bpm_dynamic(int RECEIVED_BYTE, int g_sensor_x, int g_sensor_y, int g_sensor_z);
// extern int em70xx_reset(int ref);

// /**
//  * @brief  heart data renew task
//  * @param  argument: Not used
//  * @retval None
//  */
// void HRDataRenewTask(void *argument)
// {
//     uint8_t value_strbuf[4];
//     uint8_t IdleBreakstr = 0;
//     uint16_t dat = 0;
//     uint8_t hr_temp = 0;
//     int hr_rate = 0;
//     while (1)
//     {
//         if (Page_Get_NowPage()->page_obj == &ui_HRPage)
//         {
//             osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
//             hr_rate = rand() % 100 + 1;
//             hr_rate = hr_rate / 10 + 65;

//             sprintf(value_strbuf, "%d", hr_rate);
//             lv_label_set_text(ui_HRPageNumLabel, value_strbuf);

//             // // sensor wake up
//             // EM7028_hrs_Enable();
//             // // receive the sensor wakeup message, sensor wakeup
//             // if (!Sensor_EM_Erro)
//             // {
//             //     // Hr messure
//             //     vTaskSuspendAll();
//             //     hr_temp = HR_Calculate(EM7028_Get_HRS1(), user_HR_timecount);
//             //     xTaskResumeAll();
//             //     if (ui_HRValue != hr_temp && hr_temp > 50 && hr_temp < 120)
//             //     {
//             //         // set text
//             //         ui_HRValue = hr_temp;
//             //         sprintf(value_strbuf, "%d", ui_HRValue);
//             //         lv_label_set_text(ui_HRPageNumLabel, value_strbuf);
//             //     }
//             // }
//         }
//         // osDelay(50);
//         osDelay(3000);
//     }
// }

/**
 * @brief  Sensor data renew task
 * @param  argument: Not used
 * @retval None
 */
void SensorDataRenewTask(void *argument)
{
    uint8_t value_strbuf[6];
    uint8_t IdleBreakstr = 0;
    uint8_t AirSendstr = 0;
    int dis_spo2 = 0;
    int hr_rate = 0;
    // uint8_t hr_temp = 0;
    while (1)
    {
        if (Page_Get_NowPage()->page_obj == &ui_SPO2Page) // 血氧
        {
            osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
            // sensor wake up
            vTaskSuspendAll(); // 进入临界段
            max30102_cal_data(&hr_rate, &dis_spo2);
            xTaskResumeAll();
            // dis_spo2 = rand() % 100 + 1;
            // dis_spo2 = dis_spo2 / 20 + 93;
            AirSendstr = 1;
            osMessageQueuePut(AirSend_MessageQueue, &AirSendstr, 0, 1);
            sprintf(value_strbuf, "%d", dis_spo2);
            lv_label_set_text(ui_SPO2NumLabel, value_strbuf);

            // receive the sensor wakeup message, sensor wakeup
            if (0)
            {
                // SPO2 messure

                // set text
                sprintf(value_strbuf, "%d", ui_SPO2Value);
                lv_label_set_text(ui_SPO2NumLabel, value_strbuf);
            }
        }
        else if (Page_Get_NowPage()->page_obj == &ui_HRPage) // 心率
        {
            osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
            vTaskSuspendAll();
            max30102_cal_data(&hr_rate, &dis_spo2);
            xTaskResumeAll();
            // hr_rate = rand() % 100 + 1;
            // hr_rate = hr_rate / 10 + 65;
            AirSendstr = 1;
            osMessageQueuePut(AirSend_MessageQueue, &AirSendstr, 0, 1);
            sprintf(value_strbuf, "%d", hr_rate);
            lv_label_set_text(ui_HRPageNumLabel, value_strbuf);

            // lv_arc_set_value(ui_HRArc, 100.0 * (float)(hr_rate) / 150);
            // lv_label_set_text(ui_HRNumLabel, value_strbuf);

            // // sensor wake up
            // EM7028_hrs_Enable();
            // // receive the sensor wakeup message, sensor wakeup
            // if (!Sensor_EM_Erro)
            // {
            //     // Hr messure
            //     vTaskSuspendAll();
            //     hr_temp = HR_Calculate(EM7028_Get_HRS1(), user_HR_timecount);
            //     xTaskResumeAll();
            //     if (ui_HRValue != hr_temp && hr_temp > 50 && hr_temp < 120)
            //     {
            //         // set text
            //         ui_HRValue = hr_temp;
            //         sprintf(value_strbuf, "%d", ui_HRValue);
            //         lv_label_set_text(ui_HRPageNumLabel, value_strbuf);
            //     }
            // }
        }
        else if (Page_Get_NowPage()->page_obj == &ui_EnvPage) // 环境
        {
            osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
            // // receive the sensor wakeup message, sensor wakeup
            // if (!Sensor_AHT21_Erro)
            // {
            //     // temp and humi messure
            //     float humi, temp;
            //     AHT_Read(&humi, &temp);
            //     // check
            //     if (temp > -10 && temp < 50 && humi > 0 && humi < 100)
            //     {
            //         ui_EnvTempValue = (int8_t)temp;
            //         ui_EnvHumiValue = (int8_t)humi;
            //     }
            //     // set text
            //     lv_bar_set_value(ui_EnvTempBar, ui_EnvTempValue, LV_ANIM_OFF);
            //     lv_bar_set_value(ui_EnvHumiBar, ui_EnvHumiValue, LV_ANIM_OFF);
            //     sprintf(value_strbuf, "%d", ui_EnvTempValue);
            //     lv_label_set_text(ui_EnvTempNumLabel, value_strbuf);
            //     sprintf(value_strbuf, "%d", ui_EnvHumiValue);
            //     lv_label_set_text(ui_EnvHumiNumLabel, value_strbuf);
            // }
        }
        else if (Page_Get_NowPage()->page_obj == &ui_CompassPage) // 指南针
        {
            osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
            // // receive the sensor wakeup message, sensor wakeup
            // LSM303DLH_Wakeup();
            // // SPL_Wakeup();
            // // if the sensor is no problem
            // if (!Sensor_LSM303_Erro)
            // {
            //     // messure
            //     int16_t Xa, Ya, Za, Xm, Ym, Zm;
            //     LSM303_ReadAcceleration(&Xa, &Ya, &Za);
            //     LSM303_ReadMagnetic(&Xm, &Ym, &Zm);
            //     float temp = Azimuth_Calculate(Xa, Ya, Za, Xm, Ym, Zm) + 0; // 0 offset
            //     if (temp < 0)
            //     {
            //         temp += 360;
            //     }
            //     // check
            //     if (temp >= 0 && temp <= 360)
            //     {
            //         ui_CompassDirValue = (uint16_t)temp;
            //     }
            //     // set text
            //     lv_img_set_angle(ui_Compassneedle, ui_CompassDirValue * 10);
            //     sprintf(value_strbuf, ":%d", ui_CompassDirValue);
            //     lv_label_set_text(ui_CompassDirLabel, value_strbuf);
            // }
            // // if the sensor is no problem
            // if (!Sensor_SPL_Erro)
            // {
            //     // messure
            //     float alti = Altitude_Calculate();
            //     // check
            //     if (1)
            //     {
            //         ui_EnvAltitudeValue = (int16_t)alti;
            //     }
            //     // set text
            //     sprintf(value_strbuf, ":%dm", ui_EnvAltitudeValue);
            //     lv_label_set_text(ui_EnvAltitudeLabel, value_strbuf);
            // }
        }

        // osDelay(300);
        osDelay(300);
    }
}
