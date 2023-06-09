#ifndef __THREAD_COMMON_LOG_H_
#define __THREAD_COMMON_LOG_H_

#include "chip.h"
#include "board.h"
#include <stdarg.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "Clock.h"

extern Clock *global_clock;
extern QueueHandle_t logging_queue;

/* ================= Settings ================== */
#define LOG_COLORED_OUTPUT
#define HIGH_PRIORITY_DEBUG
#define LOG_DEBUG_MESSAGES 0
/* ================= Settings ================== */

// internal debug defines
#ifdef HIGH_PRIORITY_DEBUG
#include <assert.h>
#define INT_ASSERT(statement) assert(statement)
#else
#define INT_ASSERT(statement) 
#endif

#ifdef LOG_COLORED_OUTPUT
#define C_INFO "\x1b[34mINFO\x1b[0m"
#define C_DEBUG "\x1b[35mDEBUG\x1b[0m"
#define C_DEBUG_ISR "\x1b[36mDEBUG_ISR\x1b[0m"
#define C_WARN "\x1b[33mWARNING\x1b[0m"
#define C_ERROR "\x1b[31mERROR\x1b[0m"
#else
#define C_INFO "INFO"
#define C_DEBUG "DEBUG"
#define C_DEBUG_ISR "DEBUG_ISR"
#define C_WARN "WARNING"
#define C_ERROR "ERROR"
#endif

#define LOG_BUFFER_MAX_CAP 256
#define LOG_MESSAGE_MAX_CAP 150

#define _LOG_STREAMOUT(message, message_length)                 \
    INT_ASSERT(message_length > 0);                             \
    if (logging_queue) {                                          \
        xQueueSend(logging_queue, (void*)message, portMAX_DELAY);  \
    }

static void create_log_line(const TimeFromStart _timestamp,
                            const char * _status,
                            const char * _location,
                            const char * _func,
                            const size_t _line,
                            const char * _fmt, ...)
{
    va_list args;
    va_start(args, _fmt);
    char message [LOG_BUFFER_MAX_CAP] = {0};
    int message_len = vsnprintf(message, LOG_BUFFER_MAX_CAP, _fmt, args);
    va_end(args);
    char buffer [LOG_BUFFER_MAX_CAP] = {0};
    int buffer_len = snprintf(buffer, LOG_BUFFER_MAX_CAP,
                             "[%02zu:%02zu:%02zu:%02zu:%03zu]:[%s] In [File: %s] [Func: %s] [Line: %zu]\r\n %.*s",
                             _timestamp.days,
                             _timestamp.hours,
                             _timestamp.minutes,
                             _timestamp.seconds,
                             _timestamp.milliseconds,
                             _status,
                             _location,
                             _func,
                             _line,
                             message_len,
                             message);
    _LOG_STREAMOUT(buffer, buffer_len)
    
}

static void create_log_line_nots(const char * _status,
                                const char * _location,
                                const char * _func,
                                const size_t _line,
                                const char * _fmt, ...)
{
    va_list args;
    va_start(args, _fmt);
    char message [LOG_BUFFER_MAX_CAP] = {0};
    int message_len = vsnprintf(message, LOG_BUFFER_MAX_CAP, _fmt, args);
    va_end(args);
    char buffer [LOG_BUFFER_MAX_CAP] = {0};
    int buffer_len = snprintf(buffer, LOG_BUFFER_MAX_CAP,
                             "[%s] In [File: %s] [Func: %s] [Line: %zu]\r\n %.*s",
                             _status,
                             _location,
                             _func,
                             _line,
                             message_len,
                             message);
    _LOG_STREAMOUT(buffer, buffer_len)
    
}

#define LOG_INFO( fmt, ...)                                        \
    create_log_line(global_clock->getTimeFromStart(), C_INFO, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#define LOG_WARNING(fmt, ...)                                        \
    create_log_line(global_clock->getTimeFromStart(), C_WARN, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#define LOG_ERROR(fmt, ...)                                       \
    create_log_line(global_clock->getTimeFromStart(), C_ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

#if LOG_DEBUG_MESSAGES
#define LOG_DEBUG(fmt, ...)                                        \
    create_log_line(global_clock->getTimeFromStart(), C_DEBUG, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);

/*
 * Even though it doesn't call timer, it still can cause a softlock on the UART transaction.
 * Left only for short-term debugging purposes.
 */
#define LOG_DEBUG_ISR(fmt, ...)                                        \
    create_log_line_nots(C_DEBUG_ISR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);
#else
#define LOG_DEBUG(fmt, ...)
#define LOG_DEBUG_ISR(fmt, ...)  
#endif


#endif /* __THREAD_COMMON_LOG_H_ */
