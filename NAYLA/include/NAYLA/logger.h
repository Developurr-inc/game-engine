//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#ifndef LOGGER_H
#define LOGGER_H

#include "../../src/debug.h"
#include <NAYLA/export.h>

/**********************************************************************************************************************
 *****                                                  DEFINES                                                   *****
 **********************************************************************************************************************/

#if defined(N_RELEASE)
    #define N_LOG_WARNING_ENABLED  1
    #define N_LOG_INFO_ENABLED     1
    #define N_LOG_DEBUG_ENABLED    0
    #define N_LOG_TRACE_ENABLED    0
#else
    #define N_LOG_WARNING_ENABLED  1
    #define N_LOG_INFO_ENABLED     1
    #define N_LOG_DEBUG_ENABLED    1
    #define N_LOG_TRACE_ENABLED    1
#endif

#define N_FATAL(message, ...)  n_logger_message(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)
#define N_ERROR(message, ...)  n_logger_message(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)

#if N_LOG_WARNING_ENABLED == 1
    #define N_WARNING(message, ...)  n_logger_message(LOG_LEVEL_WARNING, message, ##__VA_ARGS__)
#else
    #define N_WARNING(message, ...)
#endif

#if N_LOG_INFO_ENABLED == 1
    #define N_INFO(message, ...)  n_logger_message(LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
    #define N_INFO(message, ...)
#endif

#if N_LOG_DEBUG_ENABLED == 1
    #define N_DEBUG(message, ...) n_logger_message(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#else
    #define N_DEBUG(message, ...)
#endif

#if N_LOG_TRACE_ENABLED == 1
    #define N_TRACE(message, ...) n_logger_message(LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
#else
    #define N_TRACE(message, ...)
#endif

/**********************************************************************************************************************
 *****                                              ENUMERATIONS                                                  *****
 **********************************************************************************************************************/

typedef enum ELogLevel {
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
} ELogLevel;

/**********************************************************************************************************************
 *****                                              PUBLIC FUNCTIONS                                              *****
 **********************************************************************************************************************/

N_EXPORT void n_logger_message(ELogLevel level, const char *message, ...);

#endif //LOGGER_H