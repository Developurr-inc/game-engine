//
// Created by Vinícius Ferreira Aguiar on 10/01/25.
//

#ifndef INLINE_H
#define INLINE_H

#if defined(_MSC_VER)
    #define PAL_INLINE    __forceinline
    #define PAL_NOINLINE  __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
    #define PAL_INLINE    __attribute__((always_inline)) inline
    #define PAL_NOINLINE  __attribute__((noinline))
#else
    #define PAL_INLINE    inline
    #define PAL_NOINLINE
#endif

#endif //INLINE_H
