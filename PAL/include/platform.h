//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef PLATFORM_H
# define PLATFORM_H

# if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#  define N_PLATFORM_WINDOWS  1
#  if !defined(_WIN64)
#   error "64-bit is required on Windows!"
#  endif

// Linux
# elif defined(__linux__) || defined(__gnu_linux__)
#  define N_PLATFORM_LINUX  1
#  if defined(__ANDROID__)
#   define N_PLATFORM_ANDROID  1
#  endif

// BSD
# elif defined(__unix__)
#  define N_PLATFORM_UNIX  1

// POSIX
# elif defined(_POSIX_VERSION)
#  define N_PLATFORM_POSIX  1

// Apple
# elif defined(__APPLE__)
#  define N_PLATFORM_APPLE  1
#  include <TargetConditionals.h>
#  if TARGET_OS_SIMULATOR
#   define N_PLATFORM_IOS  1
#   define N_PLATFORM_IOS_SIMULATOR  1
#  elif TARGET_OS_IPHONE
#   define N_PLATFORM_IOS  1
#  elif TARGET_OS_MAC
#   define N_PLATFORM_MACOS  1
#  else
#   error "Unknown Apple platform"
#  endif

// Unknown
# else
#  error "Unknown platform!"
# endif

# ifdef PLAT_EXPORT
#  ifdef _MSC_VER
#   define PLAT_API  __declspec(dllexport)
#  else
#   define PLAT_API  __attribute__((visibility("default")))
#  endif
# else
#  ifdef _MSC_VER
#   define PLAT_API  __declspec(dllimport)
#  else
#   define PLAT_API
#  endif
# endif

#endif //PLATFORM_H
