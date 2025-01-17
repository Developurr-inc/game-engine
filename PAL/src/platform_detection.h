//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef PLATFORM_DETECTION_H
#define PLATFORM_DETECTION_H

#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MACOS
#elif defined(__linux__)
    #define PLATFORM_LINUX
#elif defined(__unix__)
    #define PLATFORM_UNIX
#elif defined(__FreeBSD__)
    #define PLATFORM_FREEBSD
#else
    #error "Unknown platform!"
#endif

#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
    #define ARCH_X86_64
#elif defined(_M_ARM64) || defined(__aarch64__)
    #define ARCH_ARM64
#elif defined(__powerpc64__) || defined(__ppc64__)
    #define ARCH_PPC64
#elif defined(__riscv)
    #define ARCH_RISCV
#else
    #error "Unknown 64-bit architecture!"
#endif

#endif //PLATFORM_DETECTION_H
