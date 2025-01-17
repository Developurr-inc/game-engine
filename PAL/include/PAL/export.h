//
// Created by Vinícius Ferreira Aguiar on 10/01/25.
//

#ifndef PAL_EXPORT_H
#define PAL_EXPORT_H

#if defined(_MSC_VER)
    #ifdef PAL_BUILDING_DLL
        #define PAL_EXPORT  __declspec(dllexport)
    #else
        #define PAL_EXPORT  __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #ifdef PAL_BUILDING_DLL
        #define PAL_EXPORT  __attribute__((visibility("default")))
    #else
        #define PAL_EXPORT
    #endif
#else
    #define PAL_EXPORT
#endif

#endif //PAL_EXPORT_H
