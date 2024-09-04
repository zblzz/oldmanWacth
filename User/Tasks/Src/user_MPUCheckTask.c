/**
 * @file   user_MPUCheckTask.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
/* Private includes -----------------------------------------------------------*/
// includes
#include "user_MPUCheckTask.h"
#include "user_ScrRenewTask.h"

#include "main.h"
#include "stm32l4xx_it.h"
#include "lsm.h"
#include "bsp_beep.h"
#include "log.h"
#include "air.h"

#include "ui.h"
#include "ui_SetPage.h"
#include "ui_HomePage.h"
#include "ui_MenuPage.h"
#include "ui_SetPage.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t user_MPU_Wrist_State = WRIST_UP;
uint8_t user_MPU_Wrist_EN = 1;

/* Private function prototypes -----------------------------------------------*/

/* Tasks ---------------------------------------------------------------------*/

/**
 * @brief  MPU6050 Check the state
 * @param  argument: Not used
 * @retval None
 */
void MPUCheckTask(void *argument)
{
    float ax, ay, az = 0;
    float pre_az = 0;
    uint8_t warn_send = 0, t = 0;
    uint8_t AirSendstr = 0;
    while (1)
    {
        vTaskSuspendAll();
        lsm_getAcceleration(&ax, &ay, &az);
        xTaskResumeAll();
        EMlog(LOG_DEBUG, "ax:%.2f, ay:%.2f, az:%.2f\r\n", ax, ay, az);

        if (t == 0)
        {
            pre_az = az;
            t = 1;
        }
        else
        {
            if (!warn_send && (az - pre_az > 10 || az - pre_az < -10) && az > 0)
            {
                beep_on();
                air_send_warn(1);
                warn_send = 1;
            }
            if (warn_send)
            {
                AirSendstr = 2;
                if (az < -7)
                {
                    warn_send = 0;
                    beep_off();
                    air_send_warn(0);
                    AirSendstr = 3;
                }
                osMessageQueuePut(AirSend_MessageQueue, &AirSendstr, 0, 1);
            }
        }
        pre_az = az;

        if (user_MPU_Wrist_EN)
        {
            if (lsm_isHorizontal())
            {
                user_MPU_Wrist_State = WRIST_UP;
                uint8_t IdleBreakestr = 0;
                osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakestr, 0, 1);
            }
            // else
            // {
            //     // if (user_MPU_Wrist_State == WRIST_UP)
            //     // {
            //     //     user_MPU_Wrist_State = WRIST_DOWN;
            //     //     if (Page_Get_NowPage()->page_obj == &ui_HomePage ||
            //     //         Page_Get_NowPage()->page_obj == &ui_MenuPage ||
            //     //         Page_Get_NowPage()->page_obj == &ui_SetPage)
            //     //     {
            //     //         uint8_t Idlestr = 0;
            //     //         osMessageQueuePut(Idle_MessageQueue, &Idlestr, 0, 1);
            //     //     }
            //     // }
            //     user_MPU_Wrist_State = WRIST_DOWN;
            // }
        }

        osDelay(300);
    }
}
