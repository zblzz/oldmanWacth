/**
 * @file   user_HomePageTask.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
/* Private includes -----------------------------------------------------------*/
// includes
#include "user_TasksInit.h"
#include "user_ScrRenewTask.h"
#include "main.h"
#include "HWDataAccess.h"
#include "lvgl.h"
#include "ui_HomePage.h"
#include "ui_ENVPage.h"
#include "bsp_power.h"
#include "max30102.h"
// #include "DataSave.h"
// #include "inv_mpu_dmp_motion_driver.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  homepage time renew task
 * @param  argument: Not used
 * @retval None
 */
void TimeRenewTask(void *argument)
{
    uint8_t value_strbuf[10];
    while (1)
    {
        if (Page_Get_NowPage()->page_obj == &ui_HomePage)
        {
            /*
            lv_obj_set_style_text_opa(ui_TimeColonLabel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            osDelay(500);
            lv_obj_set_style_text_opa(ui_TimeColonLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            */

            HW_DateTimeTypeDef DateTime;
            HW_RTC_Get_TimeDate(&DateTime);

            if (ui_TimeMinuteValue != DateTime.Minutes)
            {
                ui_TimeMinuteValue = DateTime.Minutes;
                sprintf((char *)value_strbuf, "%02d", ui_TimeMinuteValue);
                lv_label_set_text(ui_TimeMinuteLabel, value_strbuf);
            }

            if (ui_TimeHourValue != DateTime.Hours)
            {
                ui_TimeHourValue = DateTime.Hours;
                sprintf((char *)value_strbuf, "%2d", ui_TimeHourValue);
                lv_label_set_text(ui_TimeHourLabel, value_strbuf);
            }

            if (ui_DateDayValue != DateTime.Date)
            {
                ui_DateDayValue = DateTime.Date;
                ui_DataWeekdayValue = DateTime.WeekDay;
                sprintf((char *)value_strbuf, "%2d-%02d", ui_DateMonthValue, ui_DateDayValue);
                lv_label_set_text(ui_DateLabel, value_strbuf);
                lv_label_set_text(ui_DayLabel, ui_Days[ui_DataWeekdayValue - 1]);
            }
            if (ui_DateMonthValue != DateTime.Month)
            {
                ui_DateMonthValue = DateTime.Month;
                ui_DateDayValue = DateTime.Date;
                ui_DataWeekdayValue = DateTime.WeekDay;
                sprintf((char *)value_strbuf, "%2d-%02d", ui_DateMonthValue, ui_DateDayValue);
                lv_label_set_text(ui_DateLabel, value_strbuf);
                lv_label_set_text(ui_DayLabel, ui_Days[ui_DataWeekdayValue - 1]);
            }
        }
        osDelay(500);
    }
}

/**
 * @brief  homepage check the battery power and other data
 * @param  argument: Not used
 * @retval None
 */
void HomeUpdata_Task(void *argument)
{
    while (1)
    {
        uint8_t HomeUpdataStr;
        if (osMessageQueueGet(HomeUpdata_MessageQueue, &HomeUpdataStr, NULL, 0) == osOK)
        {
            // bat
            uint8_t value_strbuf[5];

            ui_BatArcValue = PowerCalculate();
            if (ui_BatArcValue > 0 && ui_BatArcValue <= 100)
            {
            }
            else
            {
                ui_BatArcValue = 0;
            }

            // // steps
            // if (!Sensor_MPU_Erro)
            // {
            //     unsigned long STEPS = 0;
            //     if (!Sensor_MPU_Erro)
            //         dmp_get_pedometer_step_count(&STEPS);
            //     ui_StepNumValue = (uint16_t)STEPS;
            // }

            // // temp and humi
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
            // }

            // set text
            if (Page_Get_NowPage()->page_obj == &ui_HomePage)
            {
                // bat set text
                lv_arc_set_value(ui_BatArc, ui_BatArcValue);
                sprintf((char *)value_strbuf, "%2d%%", ui_BatArcValue);
                lv_label_set_text(ui_BatNumLabel, value_strbuf);

                // step set text
                sprintf((char *)value_strbuf, "%d", ui_StepNumValue);
                lv_label_set_text(ui_StepNumLabel, value_strbuf);

                // send data save message queue
                // uint8_t Datastr = 3;
                // osMessageQueuePut(DataSave_MessageQueue, &Datastr, 0, 1);

                // humi and temp set text
                lv_arc_set_value(ui_TempArc, ui_EnvTempValue);
                sprintf((char *)value_strbuf, "%d", ui_EnvTempValue);
                lv_label_set_text(ui_TempNumLabel, value_strbuf);

                // hr
                int hr = 0, spo2 = 0;
                max30102_get_data(&hr, &spo2);
                ui_EnvHumiValue = spo2;
                lv_arc_set_value(ui_HumiArc, ui_EnvHumiValue);
                sprintf((char *)value_strbuf, "%d", ui_EnvHumiValue);
                lv_label_set_text(ui_HumiNumLabel, value_strbuf);

                lv_arc_set_value(ui_HRArc, 100 * hr / 150);
                sprintf((char *)value_strbuf, "%d", hr);
                lv_label_set_text(ui_HRNumLabel, value_strbuf);
            }
        }
        osDelay(500);
    }
}
