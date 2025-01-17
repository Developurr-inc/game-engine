//
// Created by Vinícius Ferreira Aguiar on 10/01/25.
//

#ifndef DEPRECATION_H
#define DEPRECATION_H

#if defined(_MSC_VER)
    #define PAL_DEPRECATED(message)  __declspec(deprecated(message))
#elif defined(__GNUC__) || defined(__clang__)
    #define PAL_DEPRECATED(message)  __attribute__((deprecated(message)))
#else
    #define PAL_DEPRECATED(message)
#endif

#endif //DEPRECATION_H
