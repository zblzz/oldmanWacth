/**
 * @file   user_KeyTask.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
/* Private includes -----------------------------------------------------------*/
// includes
#include "user_TasksInit.h"
#include "user_StopEnterTask.h"
#include "user_ScrRenewTask.h"
#include "ui_HomePage.h"
#include "main.h"
#include "bsp_key.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  Key press check task
 * @param  argument: Not used
 * @retval None
 */
void KeyTask(void *argument)
{
    uint8_t keystr = 0;
    uint8_t Stopstr = 0;
    uint8_t IdleBreakstr = 0;
    while (1)
    {
        switch (KeyScan(0))
        {
        case 1:
            keystr = 1;
            osMessageQueuePut(Key_MessageQueue, &keystr, 0, 1);
            osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
            break;

        // case 2:
        //     if (Page_Get_NowPage()->page_obj == &ui_HomePage)
        //     {
        //         osMessageQueuePut(Stop_MessageQueue, &Stopstr, 0, 1);
        //     }
        //     else
        //     {
        //         keystr = 2;
        //         osMessageQueuePut(Key_MessageQueue, &keystr, 0, 1);
        //         osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
        //     }
        //     break;
        default:
            break;
        }
        osDelay(1);
    }
}
