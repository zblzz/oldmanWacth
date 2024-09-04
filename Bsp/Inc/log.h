#ifndef __LOG_H_
#define __LOG_H_

#include "main.h"
#include "bsp_usart.h"

#define LOG_OPEN 1         // 1:open  0:close
#define LOG_LEVEL LOG_INFO // TODO: 修改优先级即可选择打印输出
typedef enum
{
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
} E_LOGLEVEL;

#define EMlog(level, fmt...) EM_LOG(level, __FILE__, __FUNCTION__, __LINE__, fmt)

void EM_LOG(const int level, const char *file, const char *func, const int line, const char *fmt, ...);

#endif
