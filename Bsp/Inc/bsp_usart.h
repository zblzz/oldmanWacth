#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "main.h"
#include <stdio.h>

#define USART_BUFFER_SIZE 128

// printf重定向
// #define printf myprintf

typedef enum
{
    DMA_BUF1 = 0,
    DMA_BUF2,
} dma_buf_e;

typedef enum
{
    USART_DEBUG = 0,
    USART_AIR,
} usart_index_e;

typedef struct
{
    uint8_t dmaBuf1[USART_BUFFER_SIZE]; // dma缓冲区1
    uint8_t dmaBuf2[USART_BUFFER_SIZE]; // dma缓冲区2
    uint8_t *usrBuf;                    // 用户缓冲区指针
    volatile uint16_t len;              // 接收一帧数据的长度
    dma_buf_e whichBuf;                 // 缓冲区索引
    usart_index_e index;
} usart_t;

// int myprintf(const char *format, ...);
int fputc(int ch, FILE *fp);
int fgetc(FILE *fp);

void usart_init(void);
void usart_proc(usart_t *dmaRx);
void usart_rx_IDLE(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_huart_rx, usart_t *dmaRx);

extern uint8_t HardInt_uart_flag;
extern uint8_t HardInt_receive_str[USART_BUFFER_SIZE];
extern uint8_t air_receive_flag;
extern uint8_t air_receive_buf[USART_BUFFER_SIZE];
extern uint8_t air_receive_len;

extern usart_t dmaRx_DEBUG;
extern usart_t dmaRx_air;

#endif /* __BSP_USART_H */
