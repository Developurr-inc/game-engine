//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#ifndef ASSERTS_H
# define ASSERTS_H

/**********************************************************************************************************************
 *****                                                  DEFINES                                                   *****
 **********************************************************************************************************************/

# define N_ASSERTIONS_ENABLED

# ifdef N_ASSERTIONS_ENABLED
#  if defined(_MSC_VER)
#   include <intrin.h>
#   define N_DEBUG_BREAK() __debugbreak()
#  else
#   define N_DEBUG_BREAK() __builtin_trap()
#  endif

#  define N_ASSERT_MSG(expression, message)                                     \
    do {                                                                        \
        if (!(expression))                                                      \
        {                                                                       \
            report_assertion_failure(#expression, message, __FILE__, __LINE__); \
            N_DEBUG_BREAK();                                                    \
        }                                                                       \
    } while (0)

#  define N_ASSERT(expression) N_ASSERT_MSG(expression, "")

#  if defined(_DEBUG)
#   define N_ASSERT_DEBUG(expression) N_ASSERT(expression)
#  else
#   define N_ASSERT_DEBUG(expression)
#  endif

# else
#  define N_ASSERT_MSG(expression, message)
#  define N_ASSERT(expression)
#  define N_ASSERT_DEBUG(expression)

# endif

/**********************************************************************************************************************
 *****                                                  FUNCTIONS                                                 *****
 **********************************************************************************************************************/

void report_assertion_failure(const char *expression, const char *message, const char *file, int32 line);

#endif //ASSERTS_H