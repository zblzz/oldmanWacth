/**
 * @file   user_AirTask.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
/* Private includes -----------------------------------------------------------*/
// includes
#include "user_TasksInit.h"
#include "user_AirTask.h"
#include "user_ScrRenewTask.h"

#include "air.h"
#include "max30102.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

void AirTask(void *argument)
{
    uint8_t IdleBreakstr = 0;
    uint8_t AirSendStr = 0;

    int cur_hr = 0, cur_spo2 = 0;

    while (1)
    {
        if (osMessageQueueGet(AirSend_MessageQueue, &AirSendStr, NULL, 0) == osOK)
        {
            switch (AirSendStr)
            {
            case 1: // 上传血氧心率
                osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
                max30102_get_data(&cur_hr, &cur_spo2);
                air_send_hr(cur_hr, cur_spo2);
                break;
            case 2: // 报警
                osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
                for (int i = 0; i < 5; i++)
                {
                    air_send_warn(1);    // 发出警告
                    air_send_location(); // 上传定位
                }
                break;
            case 3: // 解除报警
                // osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
                for (int i = 0; i < 5; i++)
                {
                    air_send_warn(0); // 解除警告
                }
                break;
            default:
                break;
            }
        }

        osDelay(1000);
    }
}