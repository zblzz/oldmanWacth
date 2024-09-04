/**
 * @file   log.c
 * @brief
 *
 * @author zbl (zzz)
 *         Created on: 2024.07.29
 */
#include "log.h"
#include <stdarg.h>
#include <string.h>

static char buf[16] = {0};

/**
 * @brief  获取日志等级
 * @param  level:
 * @return char*:
 * @author zbl (zzz)
 */
static char *EM_LOGlevelGet(const int level)
{
    switch (level)
    {
    case LOG_DEBUG:
        memcpy(buf, "LOG_DEBUG", sizeof("LOG_DEBUG"));
        break;
    case LOG_INFO:
        memcpy(buf, "LOG_INFO", sizeof("LOG_INFO"));
        break;
    case LOG_WARN:
        memcpy(buf, "LOG_WARN", sizeof("LOG_WARN"));
        break;
    case LOG_ERROR:
        memcpy(buf, "LOG_ERROR", sizeof("LOG_ERROR"));
        break;
    default:
        break;
    }

    return buf;
}

/**
 * @brief  日志输出
 * @param  level:
 * @param  func:
 * @param  line:
 * @param  fmt:
 * @param  ...:
 * @author zbl (zzz)
 */
void EM_LOG(const int level, const char *file, const char *func, const int line, const char *fmt, ...)
{
#if (LOG_OPEN) == 1
    if (level >= LOG_LEVEL)
    {
        va_list arg;
        va_start(arg, fmt);
        char buf[1024];
        vsnprintf(buf, sizeof(buf), fmt, arg);
        va_end(arg);

        printf("[%s] [%s %s %d]:\r\n%s\r\n", EM_LOGlevelGet(level), file, func, line, buf);
    }

    // 存储信息
#endif
}
