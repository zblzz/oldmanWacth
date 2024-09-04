/**
 * @file   bsp_usart.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "bsp_usart.h"
#include <string.h>
#include <stdarg.h>
#include "usart.h"

#include "app_ai.h"
#include "air.h"

// #include "my_math.h"

// 静态函数声明
static void usart_memset(usart_t *dmaRx);
static void uart_dma_stopRx(UART_HandleTypeDef *huart);

// dma串口接受结构体
usart_t dmaRx_DEBUG =
    {
        .index = USART_DEBUG,
};
usart_t dmaRx_air =
    {
        .index = USART_AIR,
};

// DEBUG 相关变量
uint8_t HardInt_uart_flag = 0;
uint8_t HardInt_receive_str[USART_BUFFER_SIZE] = {0};

// AIR 相关变量
uint8_t air_receive_flag = 0;
uint8_t air_receive_buf[USART_BUFFER_SIZE] = {0};
uint8_t air_receive_len = 0;

/**
 * @brief  串口操作函数
 */
void usart_proc(usart_t *dmaRx)
{
    // TODO:
    if (dmaRx->len == 0)
    {
        return;
    }

    switch (dmaRx->index)
    {
    case USART_DEBUG:
        memcpy(HardInt_receive_str, dmaRx->usrBuf, dmaRx->len);
        HardInt_uart_flag = 1;
        break;
    case USART_AIR:
        memcpy(air_receive_buf, dmaRx->usrBuf, dmaRx->len);
        air_receive_flag = 1;
        air_receive_len = dmaRx->len;
        break;
    default:
        break;
    }
}

/**
 * @brief  空闲中断处理串口+dma接收数据函数
 * @param  huart: 串口句柄
 * @param  hdma_huart_rx:   串口接收的dma句柄
 */
void usart_rx_IDLE(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_huart_rx, usart_t *dmaRx)
{
    uint32_t tmp_flag = 0;
    uint32_t temp;
    tmp_flag = __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE); // 获取IDLE标志位

    if ((tmp_flag != RESET)) // idle标志被置位
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart); // 清除标志位
        // temp = huart1.Instance->SR;  //清除状态寄存器SR,读取SR寄存器可以实现清除SR寄存器的功能
        // temp = huart1.Instance->DR; //读取数据寄存器中的数据
        // 这两句和上面那句等效

        uart_dma_stopRx(huart);                      // 停止DMA接收数据
        temp = __HAL_DMA_GET_COUNTER(hdma_huart_rx); // 获取DMA中未传输的数据个数
        // temp  = hdma_usart1_rx.Instance->NDTR;//读取NDTR寄存器 获取DMA中未传输的数据个数，
        // 这句和上面那句等效

        dmaRx->len = USART_BUFFER_SIZE - temp; // 总计数减去未传输的数据个数，得到已经接收的数据个数

        if (dmaRx->whichBuf == DMA_BUF1)
        {
            dmaRx->usrBuf = dmaRx->dmaBuf1;
            hdma_huart_rx->Instance->CMAR = (uint32_t)dmaRx->dmaBuf2; // 切换为buf2缓冲区地址
            dmaRx->whichBuf = DMA_BUF2;
            HAL_UART_Receive_DMA(huart, dmaRx->dmaBuf2, USART_BUFFER_SIZE); // 开启DMA接收
        }
        else if (dmaRx->whichBuf == DMA_BUF2)
        {
            dmaRx->usrBuf = dmaRx->dmaBuf2;
            hdma_huart_rx->Instance->CMAR = (uint32_t)dmaRx->dmaBuf1; // 切换为buf1缓冲区地址
            dmaRx->whichBuf = DMA_BUF1;
            HAL_UART_Receive_DMA(huart, dmaRx->dmaBuf1, USART_BUFFER_SIZE); // 开启DMA接收
        }

        // 数据处理
        usart_proc(dmaRx);

        // 清除缓冲区
        usart_memset(dmaRx);
    }
}

/**
 * @brief  串口初始化
 */
void usart_init(void)
{
    //  初始化接收结构体
    dmaRx_DEBUG.len = 0;
    dmaRx_DEBUG.whichBuf = DMA_BUF1;
    memset(dmaRx_DEBUG.dmaBuf1, 0, USART_BUFFER_SIZE);
    memset(dmaRx_DEBUG.dmaBuf2, 0, USART_BUFFER_SIZE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); // 使能IDLE中断
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);          //	需要清一次标志位
                                                 // DMA接收函数，此句一定要加，不加接收不到第一次传进来的实数据，是空的，且此时接收到的数据长度为缓存器的数据长度
    HAL_UART_Receive_DMA(&huart1, dmaRx_DEBUG.dmaBuf1, USART_BUFFER_SIZE);

    //  初始化接收结构体
    dmaRx_air.len = 0;
    dmaRx_air.whichBuf = DMA_BUF1;
    memset(dmaRx_air.dmaBuf1, 0, USART_BUFFER_SIZE);
    memset(dmaRx_air.dmaBuf2, 0, USART_BUFFER_SIZE);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); // 使能IDLE中断
    __HAL_UART_CLEAR_IDLEFLAG(&huart2);          //	需要清一次标志位
                                                 // DMA接收函数，此句一定要加，不加接收不到第一次传进来的实数据，是空的，且此时接收到的数据长度为缓存器的数据长度
    HAL_UART_Receive_DMA(&huart2, dmaRx_air.dmaBuf1, USART_BUFFER_SIZE);
}

/**
 * @brief  处理完清空DMA缓冲区
 */
static void usart_memset(usart_t *dmaRx)
{
    memset(dmaRx->dmaBuf1, 0, USART_BUFFER_SIZE);
    memset(dmaRx->dmaBuf2, 0, USART_BUFFER_SIZE);
    dmaRx->len = 0;
}

/**
 * @brief  串口dma停止接收数据
 * @param  huart:
 * @note    不能直接使用 HAL_UART_DMAStop(huart); 因为会连同Tx一同关闭
 * @author zbl (zzz)
 */
static void uart_dma_stopRx(UART_HandleTypeDef *huart)
{
    // HAL_UART_DMAStop(huart);
    const HAL_UART_StateTypeDef rxstate = huart->RxState;

    /* Stop UART DMA Rx request if ongoing */
    if ((HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)) &&
        (rxstate == HAL_UART_STATE_BUSY_RX))
    {
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

        /* Abort the UART DMA Rx channel */
        if (huart->hdmarx != NULL)
        {
            if (HAL_DMA_Abort(huart->hdmarx) != HAL_OK)
            {
                if (HAL_DMA_GetError(huart->hdmarx) == HAL_DMA_ERROR_TIMEOUT)
                {
                    /* Set error code to DMA */
                    huart->ErrorCode = HAL_UART_ERROR_DMA;

                    return;
                }
            }
        }

        /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
        ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
        /* In case of reception waiting for IDLE event, disable also the IDLE IE interrupt source */
        if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
        {
            ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
        }

        /* At end of Rx process, restore huart->RxState to Ready */
        huart->RxState = HAL_UART_STATE_READY;
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        /* Reset RxIsr function pointer */
        huart->RxISR = NULL;
    }
}

// // 重定向
// int myprintf(const char *format, ...)
// {
//     va_list arg;
//     static char SendBuff[USART_BUFFER_SIZE] = {0};
//     int rv;
//     while (!usart_dma_tx_over)
//         ; // 等待前一次DMA发送完成

//     va_start(arg, format);
//     rv = vsnprintf((char *)SendBuff, sizeof(SendBuff) + 1, (char *)format, arg);
//     va_end(arg);

//     HAL_UART_Transmit_DMA(&huart1, (uint8_t *)SendBuff, rv);
//     usart_dma_tx_over = 0; // 清0全局标志，发送完成后重新置1

//     return rv;
// }

// 重定向
int fputc(int ch, FILE *fp)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
    return ch;
}

int fgetc(FILE *fp)
{
    uint8_t ch = 0;
    HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
    return ch;
}