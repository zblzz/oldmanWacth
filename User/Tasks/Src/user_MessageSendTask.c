/**
 * @file   user_MessageSendTask.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
/* Private includes -----------------------------------------------------------*/
// includes
#include "string.h"
#include "stdio.h"

#include "main.h"
#include "log.h"
#include "rtc.h"

#include "user_TasksInit.h"
#include "user_MessageSendTask.h"

#include "ui.h"
#include "ui_EnvPage.h"
#include "ui_HRPage.h"
#include "ui_SPO2Page.h"
#include "ui_HomePage.h"
#include "ui_DateTimeSetPage.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
struct
{
    RTC_DateTypeDef nowdate;
    RTC_TimeTypeDef nowtime;
    int8_t humi;
    int8_t temp;
    uint8_t HR;
    uint8_t SPO2;
    uint16_t stepNum;
} BLEMessage;

struct
{
    RTC_DateTypeDef nowdate;
    RTC_TimeTypeDef nowtime;
} TimeSetMessage;

/* Private function prototypes -----------------------------------------------*/

void StrCMD_Get(uint8_t *str, uint8_t *cmd)
{
    uint8_t i = 0;
    while (str[i] != '=')
    {
        cmd[i] = str[i];
        i++;
    }
}

// set time//OV+ST=20230629125555
uint8_t TimeFormat_Get(uint8_t *str)
{
    TimeSetMessage.nowdate.Year = (str[8] - '0') * 10 + str[9] - '0';
    TimeSetMessage.nowdate.Month = (str[10] - '0') * 10 + str[11] - '0';
    TimeSetMessage.nowdate.Date = (str[12] - '0') * 10 + str[13] - '0';
    TimeSetMessage.nowtime.Hours = (str[14] - '0') * 10 + str[15] - '0';
    TimeSetMessage.nowtime.Minutes = (str[16] - '0') * 10 + str[17] - '0';
    TimeSetMessage.nowtime.Seconds = (str[18] - '0') * 10 + str[19] - '0';
    if (TimeSetMessage.nowdate.Year > 0 && TimeSetMessage.nowdate.Year < 99 && TimeSetMessage.nowdate.Month > 0 && TimeSetMessage.nowdate.Month <= 12 && TimeSetMessage.nowdate.Date > 0 && TimeSetMessage.nowdate.Date <= 31 && TimeSetMessage.nowtime.Hours >= 0 && TimeSetMessage.nowtime.Hours <= 23 && TimeSetMessage.nowtime.Minutes >= 0 && TimeSetMessage.nowtime.Minutes <= 59 && TimeSetMessage.nowtime.Seconds >= 0 && TimeSetMessage.nowtime.Seconds <= 59)
    {
        RTC_SetDate(TimeSetMessage.nowdate.Year, TimeSetMessage.nowdate.Month, TimeSetMessage.nowdate.Date);
        RTC_SetTime(TimeSetMessage.nowtime.Hours, TimeSetMessage.nowtime.Minutes, TimeSetMessage.nowtime.Seconds);
        EMlog(LOG_DEBUG, "TIMESETOK\r\n");
    }
}

/**
 * @brief  send the message via BLE, use uart
 * @param  argument: Not used
 * @retval None
 */
void MessageSendTask(void *argument)
{
    uint8_t keystr = 0;
    uint8_t Stopstr = 0;
    while (1)
    {
        if (HardInt_uart_flag)
        {
            HardInt_uart_flag = 0;
            uint8_t IdleBreakstr = 0;
            osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, NULL, 1);
            EMlog(LOG_DEBUG, "RecStr:%s\r\n", HardInt_receive_str);
            if (!strcmp((char *)HardInt_receive_str, "OV"))
            {
                EMlog(LOG_DEBUG, "OK\r\n");
            }
            else if (!strcmp((char *)HardInt_receive_str, "OV+VERSION"))
            {
                EMlog(LOG_DEBUG, "VERSION=V2.4\r\n");
            }
            else if (!strcmp((char *)HardInt_receive_str, "OV+SEND"))
            {
                HAL_RTC_GetTime(&hrtc, &(BLEMessage.nowtime), RTC_FORMAT_BIN);
                HAL_RTC_GetDate(&hrtc, &BLEMessage.nowdate, RTC_FORMAT_BIN);
                BLEMessage.humi = ui_EnvHumiValue;
                BLEMessage.temp = ui_EnvTempValue;
                BLEMessage.HR = ui_HRValue;
                BLEMessage.SPO2 = ui_SPO2Value;
                BLEMessage.stepNum = ui_StepNumValue;

                EMlog(LOG_DEBUG, "data:%2d-%02d\r\n", BLEMessage.nowdate.Month, BLEMessage.nowdate.Date);
                EMlog(LOG_DEBUG, "time:%02d:%02d:%02d\r\n", BLEMessage.nowtime.Hours, BLEMessage.nowtime.Minutes, BLEMessage.nowtime.Seconds);
                EMlog(LOG_DEBUG, "humidity:%d%%\r\n", BLEMessage.humi);
                EMlog(LOG_DEBUG, "temperature:%d\r\n", BLEMessage.temp);
                EMlog(LOG_DEBUG, "Heart Rate:%d%%\r\n", BLEMessage.HR);
                EMlog(LOG_DEBUG, "SPO2:%d%%\r\n", BLEMessage.SPO2);
                EMlog(LOG_DEBUG, "Step today:%d\r\n", BLEMessage.stepNum);
            }
            // set time//OV+ST=20230629125555
            else if (strlen((char *)HardInt_receive_str) == 20)
            {
                uint8_t cmd[10];
                memset(cmd, 0, sizeof(cmd));
                StrCMD_Get(HardInt_receive_str, cmd);
                if (user_APPSy_EN && !strcmp((char *)cmd, "OV+ST"))
                {
                    TimeFormat_Get(HardInt_receive_str);
                }
            }
            else if (!strcmp((char *)HardInt_receive_str, "MENU+HOME"))
            {

                if (Page_Get_NowPage()->page_obj == &ui_HomePage)
                {
                    osMessageQueuePut(Stop_MessageQueue, &Stopstr, 0, 1);
                }
                else
                {
                    keystr = 2;
                    osMessageQueuePut(Key_MessageQueue, &keystr, 0, 1);
                    osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
                }
            }
            else if (!strcmp((char *)HardInt_receive_str, "MENU+BACK"))
            {
                keystr = 1;
                osMessageQueuePut(Key_MessageQueue, &keystr, 0, 1);
                osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
            }
            memset(HardInt_receive_str, 0, sizeof(HardInt_receive_str));
        }
        osDelay(1000);
    }
}
