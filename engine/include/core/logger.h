//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#ifndef LOGGER_H
# define LOGGER_H

# include "../../src/defines.h"

/**********************************************************************************************************************
 *****                                                  DEFINES                                                   *****
 **********************************************************************************************************************/

# if defined(N_RELEASE)
#  define LOG_WARNING_ENABLED  1
#  define LOG_INFO_ENABLED     1
#  define LOG_DEBUG_ENABLED    0
#  define LOG_TRACE_ENABLED    0
# else
#  define LOG_WARNING_ENABLED  1
#  define LOG_INFO_ENABLED     1
#  define LOG_DEBUG_ENABLED    1
#  define LOG_TRACE_ENABLED    1
# endif

# define N_FATAL(message, ...)  log_message(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)
# define N_ERROR(message, ...)  log_message(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)

# if LOG_WARNING_ENABLED == 1
#  define N_WARNING(message, ...)  log_message(LOG_LEVEL_WARNING, message, ##__VA_ARGS__)
# else
#  define N_WARNING(message, ...)
# endif

# if LOG_INFO_ENABLED == 1
#  define N_INFO(message, ...)  log_message(LOG_LEVEL_INFO, message, ##__VA_ARGS__)
# else
#  define N_INFO(message, ...)
# endif

# if LOG_DEBUG_ENABLED == 1
#  define N_DEBUG(message, ...) log_message(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
# else
#  define N_DEBUG(message, ...)
# endif

# if LOG_TRACE_ENABLED == 1
#  define N_TRACE(message, ...) log_message(LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
# else
#  define N_TRACE(message, ...)
# endif

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
 *****                                             PROTECTED FUNCTIONS                                            *****
 **********************************************************************************************************************/

bool1 logger_create();
void logger_destroy();

/**********************************************************************************************************************
 *****                                              PUBLIC FUNCTIONS                                              *****
 **********************************************************************************************************************/

void log_message(ELogLevel level, const char *message, ...);

#endif //LOGGER_H