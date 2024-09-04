/**
 * @file   user_ScrRenewTask.c
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
#include "lvgl.h"
#include "ui_HomePage.h"
#include "ui_MenuPage.h"
#include "ui_GameSelectPage.h"
#include "ui_SetPage.h"
#include "ui_OffTimePage.h"
#include "ui_DateTimeSetPage.h"

#include "max30102.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern osMessageQueueId_t Key_MessageQueue;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  Screen renew task
 * @param  argument: Not used
 * @retval None
 */
void ScrRenewTask(void *argument)
{
    uint8_t keystr = 0;
    while (1)
    {
        if (osMessageQueueGet(Key_MessageQueue, &keystr, NULL, 0) == osOK)
        {
            // key1 pressed
            if (keystr == 1)
            {
                Page_Back();
                if (Page_Get_NowPage()->page_obj == &ui_MenuPage)
                {
                    max30102_disable();
                    // //HR sensor sleep
                    // EM7028_hrs_DisEnable();
                    // //sensor sleep
                    // LSM303DLH_Sleep();
                    // // SPL_Sleep();
                }
                else if (Page_Get_NowPage()->page_obj == &ui_HomePage)
                {
                    uint8_t HomeUpdataStr = 0;
                    osMessageQueuePut(HomeUpdata_MessageQueue, &HomeUpdataStr, 0, 1);
                }
            }
            // key2 pressed
            else if (keystr == 2)
            {
                Page_Back_Bottom();
                // // HR sensor sleep
                // EM7028_hrs_DisEnable();
                // // sensor sleep
                // LSM303DLH_Sleep();
                // // SPL_Sleep();
            }
        }
        osDelay(10);
    }
}
