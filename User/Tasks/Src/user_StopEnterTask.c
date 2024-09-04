/**
 * @file   user_StopEnterTask.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
/* Private includes -----------------------------------------------------------*/
// includes
#include "user_TasksInit.h"
#include "user_MPUCheckTask.h"

#include "ui.h"
#include "ui_HomePage.h"
#include "ui_OffTimePage.h"

#include "main.h"
#include "stm32l4xx_it.h"
#include "usart.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "bsp_power.h"
#include "log.h"
// #include "MPU6050.h"
// #include "key.h"
// #include "WDOG.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint16_t IdleTimerCount = 0;

/* Private function prototypes -----------------------------------------------*/

/* Tasks ---------------------------------------------------------------------*/

/**
 * @brief  Enter Idle state
 * @param  argument: Not used
 * @retval None
 */
void IdleEnterTask(void *argument)
{
    uint8_t darkFlag = 0;
    uint8_t lowPowerFlag = 0;
    uint8_t Idlestr = 0;
    uint8_t IdleBreakstr = 0;
    while (1)
    {
        // light get dark
        if (osMessageQueueGet(Idle_MessageQueue, &Idlestr, NULL, 1) == osOK)
        {
            if (!darkFlag)
            {
                LCD_Set_Light(5);
                darkFlag = 1;
            }
            else
            {
                LCD_Set_Light(0);
                // Power_Low_Mode(); // 低功耗模式
                lowPowerFlag = 1;
            }
        }
        // resume light if light got dark and idle state breaked by key pressing or screen touching
        if (osMessageQueueGet(IdleBreak_MessageQueue, &IdleBreakstr, NULL, 1) == osOK)
        {
            if (lowPowerFlag) // 解除低功耗模式
            {
                // Power_Normal_Mode();
                lowPowerFlag = 0;
            }
            darkFlag = 0;
            IdleTimerCount = 0;
            LCD_Set_Light(ui_LightSliderValue);
        }
        osDelay(10);
    }
}

/**
 * @brief  enter the stop mode and resume
 * @param  argument: Not used
 * @retval None
 */
void StopEnterTask(void *argument)
{
    uint8_t Stopstr;
    uint8_t HomeUpdataStr;
    uint8_t Wrist_Flag = 0;
    while (1)
    {
        if (osMessageQueueGet(Stop_MessageQueue, &Stopstr, NULL, 0) == osOK)
        {

            // /****************************** your sleep operations *****************************/
            // sleep:
            //     IdleTimerCount = 0;

            //     // sensors

            //     // usart
            //     HAL_UART_MspDeInit(&huart1);

            //     // lcd
            //     LCD_RES_Clr();
            //     LCD_Close_Light();
            //     // touch
            //     CST816_Sleep();

            //     /***********************************************************************************/

            //     vTaskSuspendAll();
            //     // Disnable Watch Dog
            //     WDOG_Disnable();
            //     // systick int
            //     CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
            //     // enter stop mode
            //     HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);

            //     // here is the sleep period

            //     // resume run mode and reset the sysclk
            //     SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
            //     HAL_SYSTICK_Config(SystemCoreClock / (1000U / uwTickFreq));
            //     SystemClock_Config();
            //     WDOG_Feed();
            //     xTaskResumeAll();

            //     /****************************** your wakeup operations ******************************/

            //     // MPU Check
            //     if (user_MPU_Wrist_EN)
            //     {
            //         uint8_t hor;
            //         hor = MPU_isHorizontal();
            //         if (hor && user_MPU_Wrist_State == WRIST_DOWN)
            //         {
            //             user_MPU_Wrist_State = WRIST_UP;
            //             Wrist_Flag = 1;
            //             // resume, go on
            //         }
            //         else if (!hor && user_MPU_Wrist_State == WRIST_UP)
            //         {
            //             user_MPU_Wrist_State = WRIST_DOWN;
            //             IdleTimerCount = 0;
            //             goto sleep;
            //         }
            //     }

            //     //
            //     if (!KEY1 || KEY2 || HardInt_Charg_flag || Wrist_Flag)
            //     {
            //         Wrist_Flag = 0;
            //         // resume, go on
            //     }
            //     else
            //     {
            //         IdleTimerCount = 0;
            //         goto sleep;
            //     }

            //     // usart
            //     HAL_UART_MspInit(&huart1);
            //     // lcd
            //     LCD_Init();
            //     LCD_Set_Light(ui_LightSliderValue);
            //     // touch
            //     CST816_Wakeup();
            //     // check if is Charging
            //     if (ChargeCheck())
            //     {
            //         HardInt_Charg_flag = 1;
            //     }
            //     // send the Home Updata message
            //     osMessageQueuePut(HomeUpdata_MessageQueue, &HomeUpdataStr, 0, 1);

            //     /**************************************************************************************/
        }
        osDelay(100);
    }
}

void IdleTimerCallback(void *argument)
{
    IdleTimerCount += 1;

    // make sure the LightOffTime<TurnOffTime
    if (IdleTimerCount == (ui_LTimeValue * 7)) // 屏幕变暗
    {
        uint8_t Idlestr = 0;
        // send the Light off message
        osMessageQueuePut(Idle_MessageQueue, &Idlestr, 0, 1); // 熄屏
    }
    if (IdleTimerCount == (ui_LTimeValue * 10))
    {
        uint8_t Idlestr = 0;
        // send the Light off message
        osMessageQueuePut(Idle_MessageQueue, &Idlestr, 0, 1);
    }
    if (IdleTimerCount == (ui_TTimeValue * 10))
    {
        uint8_t Stopstr = 1;
        IdleTimerCount = 0;
        // send the Stop message
        osMessageQueuePut(Stop_MessageQueue, &Stopstr, 0, 1);
    }
}
