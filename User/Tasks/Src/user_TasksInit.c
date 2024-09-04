/**
 * @file   user_TasksInit.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
/* Private includes -----------------------------------------------------------*/
// includes
#include "user_TasksInit.h"
// sys
#include "sys.h"
#include "stdio.h"
#include "lcd.h"
#include "bsp_usart.h"
#include "log.h"
// #include "WDOG.h"
// gui
#include "lvgl.h"
#include "ui_TimerPage.h"
// tasks
#include "user_StopEnterTask.h"
#include "user_KeyTask.h"
#include "user_ScrRenewTask.h"
#include "user_HomePageTask.h"
#include "user_SensorPageTask.h"
#include "user_ChargPageTask.h"
#include "user_MessageSendTask.h"
#include "user_MPUCheckTask.h"
#include "user_DataSaveTask.h"
#include "user_AirTask.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Timers --------------------------------------------------------------------*/
osTimerId_t IdleTimerHandle;

/* Tasks ---------------------------------------------------------------------*/
// LVGL Handler task
osThreadId_t LvHandlerTaskHandle;
const osThreadAttr_t LvHandlerTask_attributes = {
    .name = "LvHandlerTask",
    .stack_size = 128 * 28,
    .priority = (osPriority_t)osPriorityLow,
};

// // WDOG Feed task
// osThreadId_t WDOGFeedTaskHandle;
// const osThreadAttr_t WDOGFeedTask_attributes = {
//     .name = "WDOGFeedTask",
//     .stack_size = 128 * 1,
//     .priority = (osPriority_t)osPriorityHigh2,
// };

// Idle Enter Task
osThreadId_t IdleEnterTaskHandle;
const osThreadAttr_t IdleEnterTask_attributes = {
    .name = "IdleEnterTask",
    .stack_size = 128 * 2,
    .priority = (osPriority_t)osPriorityHigh,
};

// Stop Enter Task
osThreadId_t StopEnterTaskHandle;
const osThreadAttr_t StopEnterTask_attributes = {
    .name = "StopEnterTask",
    // .stack_size = 128 * 16,
    .stack_size = 128 * 1,
    .priority = (osPriority_t)osPriorityHigh1,
};

// Key task
osThreadId_t KeyTaskHandle;
const osThreadAttr_t KeyTask_attributes = {
    .name = "KeyTask",
    .stack_size = 128 * 1,
    .priority = (osPriority_t)osPriorityNormal,
};

// ScrRenew task
osThreadId_t ScrRenewTaskHandle;
const osThreadAttr_t ScrRenewTask_attributes = {
    .name = "ScrRenewTask",
    .stack_size = 128 * 10,
    .priority = (osPriority_t)osPriorityLow1,
};

// TimeRenew task
osThreadId_t TimeRenewTaskHandle;
const osThreadAttr_t TimeRenewTask_attributes = {
    .name = "TimeRenewTask",
    .stack_size = 128 * 8,
    .priority = (osPriority_t)osPriorityLow1,
};

// HomeUpdata task
osThreadId_t HomeUpdataTaskHandle;
const osThreadAttr_t HomeUpdataTask_attributes = {
    .name = "HomeUpdataTask",
    .stack_size = 128 * 6,
    .priority = (osPriority_t)osPriorityLow1,
};

// SensorDataRenew task
osThreadId_t SensorDataTaskHandle;
const osThreadAttr_t SensorDataTask_attributes = {
    .name = "SensorDataTask",
    .stack_size = 128 * 6,
    .priority = (osPriority_t)osPriorityLow1,
};

// HRDataRenew task
// osThreadId_t HRDataTaskHandle;
// const osThreadAttr_t HRDataTask_attributes = {
//     .name = "HRDataTask",
//     .stack_size = 128 * 5,
//     .priority = (osPriority_t)osPriorityLow1,
// };

// ChargPageEnterTask
// osThreadId_t ChargPageEnterTaskHandle;
// const osThreadAttr_t ChargPageEnterTask_attributes = {
//     .name = "ChargPageEnterTask",
//     .stack_size = 128 * 10,
//     .priority = (osPriority_t)osPriorityLow1,
// };

// ChargPageRenewTask
// osThreadId_t ChargPageRenewTaskHandle;
// const osThreadAttr_t ChargPageRenewTask_attributes = {
//     .name = "ChargPageRenewTask",
//     .stack_size = 128 * 5,
//     .priority = (osPriority_t)osPriorityLow1,
// };

// messagesendtask
osThreadId_t MessageSendTaskHandle;
const osThreadAttr_t MessageSendTask_attributes = {
    .name = "MessageSendTask",
    .stack_size = 128 * 6,
    .priority = (osPriority_t)osPriorityNormal, // TODO: osPriorityLow1 修改成 osPriorityNormal
};

// airtask
osThreadId_t AirTaskHandle;
const osThreadAttr_t AirTask_attributes = {
    .name = "AirTask",
    .stack_size = 128 * 7,
    .priority = (osPriority_t)osPriorityNormal, // TODO: osPriorityLow1 修改成 osPriorityNormal
};

// MPUCheckTask
osThreadId_t MPUCheckTaskHandle;
const osThreadAttr_t MPUCheckTask_attributes = {
    .name = "MPUCheckTask",
    .stack_size = 128 * 8,
    .priority = (osPriority_t)osPriorityLow1,
};

osThreadId_t CPUTaskHandle;
const osThreadAttr_t CPUTask_attributes = {
    .name = "CPUTask",
    .stack_size = 128 * 5,
    .priority = (osPriority_t)osPriorityLow,
};

// // DataSaveTask
// osThreadId_t DataSaveTaskHandle;
// const osThreadAttr_t DataSaveTask_attributes = {
//     .name = "DataSaveTask",
//     .stack_size = 128 * 5,
//     .priority = (osPriority_t)osPriorityLow2,
// };

/* Message queues ------------------------------------------------------------*/
// Key message
osMessageQueueId_t Key_MessageQueue;
osMessageQueueId_t Idle_MessageQueue;
osMessageQueueId_t Stop_MessageQueue;
osMessageQueueId_t IdleBreak_MessageQueue;
osMessageQueueId_t HomeUpdata_MessageQueue;
osMessageQueueId_t DataSave_MessageQueue;
osMessageQueueId_t AirSend_MessageQueue;

/* Private function prototypes -----------------------------------------------*/
void LvHandlerTask(void *argument);
void WDOGFeedTask(void *argument);
void CPUTask(void *argument);

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void User_Tasks_Init(void)
{
    /* add mutexes, ... */

    /* add semaphores, ... */

    /* start timers, add new ones, ... */

    IdleTimerHandle = osTimerNew(IdleTimerCallback, osTimerPeriodic, NULL, NULL);
    osTimerStart(IdleTimerHandle, 100); // 100ms

    /* add queues, ... */
    Key_MessageQueue = osMessageQueueNew(1, 1, NULL);
    Idle_MessageQueue = osMessageQueueNew(1, 1, NULL);
    Stop_MessageQueue = osMessageQueueNew(1, 1, NULL);
    IdleBreak_MessageQueue = osMessageQueueNew(1, 1, NULL);
    HomeUpdata_MessageQueue = osMessageQueueNew(1, 1, NULL);
    DataSave_MessageQueue = osMessageQueueNew(2, 1, NULL);
    AirSend_MessageQueue = osMessageQueueNew(1, 1, NULL);

    /* add threads, ... */
    // lcd lvgl 刷新任务
    LvHandlerTaskHandle = osThreadNew(LvHandlerTask, NULL, &LvHandlerTask_attributes);
    ScrRenewTaskHandle = osThreadNew(ScrRenewTask, NULL, &ScrRenewTask_attributes);
    TimeRenewTaskHandle = osThreadNew(TimeRenewTask, NULL, &TimeRenewTask_attributes);
    HomeUpdataTaskHandle = osThreadNew(HomeUpdata_Task, NULL, &HomeUpdataTask_attributes);

    // 熄屏任务
    IdleEnterTaskHandle = osThreadNew(IdleEnterTask, NULL, &IdleEnterTask_attributes);
    StopEnterTaskHandle = osThreadNew(StopEnterTask, NULL, &StopEnterTask_attributes);
    // WDOGFeedTaskHandle = osThreadNew(WDOGFeedTask, NULL, &WDOGFeedTask_attributes);
    KeyTaskHandle = osThreadNew(KeyTask, NULL, &KeyTask_attributes);

    // 各种传感器采集任务
    SensorDataTaskHandle = osThreadNew(SensorDataRenewTask, NULL, &SensorDataTask_attributes);

    // 心率、血氧任务
    // HRDataTaskHandle = osThreadNew(HRDataRenewTask, NULL, &HRDataTask_attributes);

    // 充电页面任务
    // ChargPageEnterTaskHandle = osThreadNew(ChargPageEnterTask, NULL, &ChargPageEnterTask_attributes);
    // ChargPageRenewTaskHandle = osThreadNew(ChargPageRenewTask, NULL, &ChargPageRenewTask_attributes);

    // 串口消息接收发送任务
    // MessageSendTaskHandle = osThreadNew(MessageSendTask, NULL, &MessageSendTask_attributes);

    // air 任务
    AirTaskHandle = osThreadNew(AirTask, NULL, &AirTask_attributes);

    // mpu检测任务
    MPUCheckTaskHandle = osThreadNew(MPUCheckTask, NULL, &MPUCheckTask_attributes);
    // DataSaveTaskHandle = osThreadNew(DataSaveTask, NULL, &DataSaveTask_attributes);

    // ai任务

    // cpu 任务
    // CPUTaskHandle = osThreadNew(CPUTask, NULL, &CPUTask_attributes);

    /* add events, ... */

    /* add  others ... */
    uint8_t HomeUpdataStr = 0;
    osMessageQueuePut(HomeUpdata_MessageQueue, &HomeUpdataStr, 0, 1);
}

/**
 * @brief  FreeRTOS Tick Hook, to increase the LVGL tick
 * @param  None
 * @retval None
 */
void TaskTickHook(void)
{
    // to increase the LVGL tick
    lv_tick_inc(1);
    // to increase the timerpage's timer(put in here is to ensure the Real Time)
    if (ui_TimerPageFlag)
    {
        ui_TimerPage_ms += 1;
        if (ui_TimerPage_ms >= 10)
        {
            ui_TimerPage_ms = 0;
            ui_TimerPage_10ms += 1;
        }
        if (ui_TimerPage_10ms >= 100)
        {
            ui_TimerPage_10ms = 0;
            ui_TimerPage_sec += 1;
            uint8_t IdleBreakstr = 0;
            osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 0);
        }
        if (ui_TimerPage_sec >= 60)
        {
            ui_TimerPage_sec = 0;
            ui_TimerPage_min += 1;
        }
        if (ui_TimerPage_min >= 60)
        {
            ui_TimerPage_min = 0;
        }
    }
    user_HR_timecount += 1;
}

/**
 * @brief  LVGL Handler task, to run the lvgl
 * @param  argument: Not used
 * @retval None
 */
void LvHandlerTask(void *argument)
{
    uint8_t IdleBreakstr = 0;
    while (1)
    {
        if (lv_disp_get_inactive_time(NULL) < 1000)
        {
            // Idle time break, set to 0
            osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 0);
        }
        lv_task_handler();
        osDelay(1);
    }
}

/**
 * @brief  Watch Dog Feed task
 * @param  argument: Not used
 * @retval None
 */
void WDOGFeedTask(void *argument)
{
    // // owdg
    // WDOG_Port_Init();
    // while (1)
    // {
    //     WDOG_Feed();
    //     WDOG_Enable();
    //     osDelay(100);
    // }
}

/**
 * @brief  CPU 测试占用率
 * @param  argument:
 * @author zbl (zzz)
 * @note 需要开启一个定时器，并且需要使能configGENERATE_RUN_TIME_STATS、configUSE_TRACE_FACILITY 、configUSE_STATS_FORMATTING_FUNCTIONS
 *          还要实现portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() (CPU_RunTime = 0ul) 和
 *           portGET_RUN_TIME_COUNTER_VALUE() CPU_RunTime 这两个宏定义
 */
void CPUTask(void *argument)
{
    // uint8_t CPU_RunInfo[400]; // 保存任务运行时间信息

    // while (1)
    // {
    //     memset(CPU_RunInfo, 0, 400); // 信息缓冲区清零

    //     vTaskList((char *)&CPU_RunInfo); // 获取任务运行时间信息

    //     printf("---------------------------------------------\r\n");
    //     printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
    //     printf("%s", CPU_RunInfo);
    //     printf("---------------------------------------------\r\n");

    //     memset(CPU_RunInfo, 0, 400); // 信息缓冲区清零

    //     vTaskGetRunTimeStats((char *)&CPU_RunInfo);

    //     printf("任务名       运行计数         利用率\r\n");
    //     printf("%s", CPU_RunInfo);
    //     printf("---------------------------------------------\r\n\n");
    //     osDelay(500); /* 延时500个tick */
    // }
}

// volatile uint32_t CPU_RunTime = 0UL;
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//     if (htim->Instance == TIM3)
//     {
//         // 用户代码
//         CPU_RunTime++;
//     }
// }
