/**
 * @file   HWDataAccess.c
 * @brief  加这个文件就是为了方便UI移植, 比如你要把工程移植到PC仿真,
 *   	    直接把MidFunc中的文件和UI文件都复制过去,
 * 			然后直接把.h文件中的HW_USE_HARDWARE变成0就行了.
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "../Inc/HWDataAccess.h"

/**************************************************************************/

static uint8_t Year = 24;
static uint8_t Month = 7;
static uint8_t Date = 23;
static uint8_t WeekDay = 7;
static uint8_t Hours = 11;
static uint8_t Minutes = 59;
static uint8_t Seconds = 55;

void HW_Air_Change_TimeDate(uint8_t curYear, uint8_t curMonth, uint8_t curData, uint8_t curHours, uint8_t curMinutes, uint8_t curSecond)
{
    Year = curYear;
    Month = curMonth;
    Date = curData;
    Hours = curHours;
    Minutes = curMinutes;
    Seconds = curSecond;
    WeekDay = weekday_calculate(Year, Month, Date, 20);
#if HW_USE_RTC
    HW_RTC_Set_Date(Year, Month, Date);
    HW_RTC_Set_Time(Hours, Minutes, Seconds);
#endif
}

void HW_RTC_Get_TimeDate(HW_DateTimeTypeDef *nowdatetime)
{
#if HW_USE_RTC
    if (nowdatetime != NULL)
    {
        RTC_DateTypeDef nowdate;
        RTC_TimeTypeDef nowtime;
        HAL_RTC_GetTime(&hrtc, &nowtime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &nowdate, RTC_FORMAT_BIN);
        nowdatetime->Year = nowdate.Year;
        nowdatetime->Month = nowdate.Month;
        nowdatetime->Date = nowdate.Date;
        nowdatetime->WeekDay = weekday_calculate(nowdatetime->Year, nowdatetime->Month, nowdatetime->Date, 20);
        nowdatetime->Hours = nowtime.Hours;
        nowdatetime->Minutes = nowtime.Minutes;
        nowdatetime->Seconds = nowtime.Seconds;
    }
#else
    nowdatetime->Year = Year;
    nowdatetime->Month = Month;
    nowdatetime->Date = Date;
    nowdatetime->WeekDay = WeekDay;
    nowdatetime->Hours = Hours;
    nowdatetime->Minutes = Minutes;
    nowdatetime->Seconds = Seconds;
#endif
}

void HW_RTC_Set_Date(uint8_t year, uint8_t month, uint8_t date)
{
#if HW_USE_RTC
    RTC_SetDate(year, month, date);
#endif
}

void HW_RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
#if HW_USE_RTC
    RTC_SetTime(hours, minutes, seconds);
#endif
}

/**
 * @brief  日期
 * @param  setyear:
 * @param  setmonth:
 * @param  setday:
 * @param  century:
 * @return uint8_t:
 * @author zbl (zzz)
 */
uint8_t HW_weekday_calculate(uint8_t setyear, uint8_t setmonth, uint8_t setday, uint8_t century)
{
    int w;
    if (setmonth == 1 || setmonth == 2)
    {
        setyear--, setmonth += 12;
    }
    w = setyear + setyear / 4 + century / 4 + 26 * (setmonth + 1) / 10 + setday - 1 - 2 * century;
    while (w < 0)
        w += 7;
    w %= 7;
    w = (w == 0) ? 7 : w;
    return w;
}

/**
 * @brief  陀螺仪
 * @author zbl (zzz)
 */
uint8_t HW_MPU_Wrist_is_Enabled(void)
{
#if HW_USE_IMU
    if (user_MPU_Wrist_EN)
    {
        return true;
    }
#endif
    return false;
}

void HW_MPU_Wrist_Enable(void)
{
#if HW_USE_IMU
    user_MPU_Wrist_EN = 1;
#endif
}

void HW_MPU_Wrist_Disable(void)
{
#if HW_USE_IMU
    user_MPU_Wrist_EN = 0;
#endif
}

/**
 * @brief  蓝牙
 * @author zbl (zzz)
 */
void HW_BLE_Enable(void)
{
#if HW_USE_BLE
    KT6328_Enable();
#endif
}

void HW_BLE_Disable(void)
{
#if HW_USE_BLE
    KT6328_Disable();
#endif
}

/**
 * @brief  关闭电源
 * @author zbl (zzz)
 */
void HW_Power_Shutdown(void)
{
#if HW_USE_BAT
    Power_DisEnable();
#endif
}

/**
 * @brief  LCD亮度调节
 * @param  dc:
 * @author zbl (zzz)
 */
void HW_LCD_Set_Light(uint8_t dc)
{
#if HW_USE_LCD
    LCD_Set_Light(dc);
#endif
}

/**
 * @brief  驱蚊
 * @author zbl (zzz)
 */
void HW_WenZi_Enable(void)
{
#if HW_USE_WENZI
    __HAL_TIM_SetCompare(&htim16, TIM_CHANNEL_1, (htim16.Init.Period - 1) / 2);
    HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1); // 驱蚊pwm
#endif
}

void HW_WenZi_Disable(void)
{
#if HW_USE_WENZI
    HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1); // 驱蚊pwm
#endif
}
