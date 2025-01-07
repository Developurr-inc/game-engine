//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//
/**
 * @file defines.h
 * @author Travis Vroman (travis@kohiengine.com)
 * @brief This file contains global type definitions which are used
 * throughout the entire engine and applications referencing it.
 * Numeric types are asserted statically to gurantee expected size.
 * @version 2.0
 * @date 2024-04-03
 *
 * @copyright Kohi Game Engine is Copyright (c) Travis Vroman 2021-2024
 *
 */

#ifndef DEFINES_H
# define DEFINES_H

# include <stdint.h>

/**********************************************************************************************************************
 *****                                                  DEFINES                                                   *****
 **********************************************************************************************************************/

// Debugging

// Debug
# ifdef _DEBUG
#  define NAYLA_DEBUG

// Release
# else
#  define NAYLA_RELEASE

# endif


// Platform Detection

// Windows
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
#   define N_PLATFORM_IOS 1
#   define N_PLATFORM_IOS_SIMULATOR 1
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


// API Export/Import

// Exports
# ifdef N_EXPORT
#  ifdef _MSC_VER
#   define N_API  __declspec(dllexport)
#  else
#   define N_API  __attribute__((visibility("default")))
#  endif

// Imports
# else
#  ifdef _MSC_VER
#   define N_API  __declspec(dllimport)
#  else
#   define N_API
#  endif

# endif


// Inlining

// Clang or GCC
# if defined(__clang__) || defined(__gcc__)
#  define N_INLINE     __attribute__((always_inline)) inline
#  define N_NO_INLINE  __attribute__((noinline))

// MSVC
# elif defined(_MSC_VER)
#  define N_INLINE     __forceinline
#  define N_NO_INLINE  __declspec(noinline)

// Unknown
# else
#  define N_INLINE     static inline
#  define N_NO_INLINE

# endif


// Deprecation

// Clang or GCC
# if defined(__clang__) || defined(__gcc__)
#  define N_DEPRECATED(message)  __attribute__((deprecated(message)))

// MSVC
# elif defined(_MSC_VER)
#  define N_DEPRECATED(message)  __declspec(deprecated(message))

// Unknown
#else
# error "Unsupported compiler - don't know how to define deprecations!"

#endif


// Static Assertions

//
# if defined(__clang__) || defined(__GNUC__)
#  define STATIC_ASSERT  _Static_assert

//
# else
#  define STATIC_ASSERT  static_assert

# endif


// Byte Sizes

/** @brief Gets the number of bytes from amount of gibibytes (GiB) (1024*1024*1024) */
#define GIBIBYTES(amount) ((amount) * 1024ULL * 1024ULL * 1024ULL)

/** @brief Gets the number of bytes from amount of mebibytes (MiB) (1024*1024) */
#define MEBIBYTES(amount) ((amount) * 1024ULL * 1024ULL)

/** @brief Gets the number of bytes from amount of kibibytes (KiB) (1024) */
#define KIBIBYTES(amount) ((amount) * 1024ULL)

/** @brief Gets the number of bytes from amount of gigabytes (GB) (1000*1000*1000) */
#define GIGABYTES(amount) ((amount) * 1000ULL * 1000ULL * 1000ULL)

/** @brief Gets the number of bytes from amount of megabytes (MB) (1000*1000) */
#define MEGABYTES(amount) ((amount) * 1000ULL * 1000ULL)

/** @brief Gets the number of bytes from amount of kilobytes (KB) (1000) */
#define KILOBYTES(amount) ((amount) * 1000ULL)


// Useful Macros

/**
 * @brief Clamps value to a range of min and max (inclusive).
 * @param value The value to be clamped.
 * @param min The minimum value of the range.
 * @param max The maximum value of the range.
 * @returns The clamped value.
 */
#define N_CLAMP(value, min, max)  ((value <= min) ? min : (value >= max) ? max : value)

#define N_MIN(x, y)  (x < y ? x : y)
#define N_MAX(x, y)  (x > y ? x : y)


// Constants

/**
 * @brief Any id set to this should be considered invalid,
 * and not actually pointing to a real object.
 */
#define INVALID_ID_U64  18446744073709551615UL
#define INVALID_ID_U32  4294967295U
#define INVALID_ID_U16  65535U
#define INVALID_ID_U8   255U

/**********************************************************************************************************************
 *****                                                  TYPEDEFS                                                  *****
 **********************************************************************************************************************/

/** @brief 1-bit boolean type */
typedef bool bool1;

/** @brief 8-bit boolean type */
typedef uint8_t bool8;

/** @brief 16-bit boolean type */
typedef uint16_t bool16;

/** @brief 32-bit boolean type, used for APIs which require it */
typedef uint32_t bool32;

/** @brief 64-bit boolean type */
typedef uint64_t bool64;

/** @brief Unsigned 8-bit integer */
typedef uint8_t uint8;

/** @brief Unsigned 16-bit integer */
typedef uint16_t uint16;

/** @brief Unsigned 32-bit integer */
typedef uint32_t uint32;

/** @brief Unsigned 64-bit integer */
typedef uint64_t uint64;

/** @brief Signed 8-bit integer */
typedef int8_t int8;

/** @brief Signed 16-bit integer */
typedef int16_t int16;

/** @brief Signed 32-bit integer */
typedef int32_t int32;

/** @brief Signed 64-bit integer */
typedef int64_t int64;

/** @brief 32-bit floating point number */
typedef float float32;

/** @brief 64-bit floating point number */
typedef double float64;

/** @brief 128-bit floating point number */
typedef long double float128;

/** @brief Assert bool8 to be 1 byte.*/
STATIC_ASSERT(sizeof(bool8) == 1, "Expected bool8 to be 1 byte.");

/** @brief Assert bool16 to be 2 bytes.*/
STATIC_ASSERT(sizeof(bool16) == 2, "Expected bool16 to be 2 bytes.");

/** @brief Assert bool32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(bool32) == 4, "Expected bool32 to be 4 bytes.");

/** @brief Assert bool64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(bool64) == 8, "Expected bool64 to be 8 bytes.");

/** @brief Assert uint8 to be 1 byte.*/
STATIC_ASSERT(sizeof(uint8) == 1, "Expected u8 to be 1 byte.");

/** @brief Assert uint16 to be 2 bytes.*/
STATIC_ASSERT(sizeof(uint16) == 2, "Expected u16 to be 2 bytes.");

/** @brief Assert uint32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(uint32) == 4, "Expected u32 to be 4 bytes.");

/** @brief Assert uint64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(uint64) == 8, "Expected u64 to be 8 bytes.");

/** @brief Assert int8 to be 1 byte.*/
STATIC_ASSERT(sizeof(int8) == 1, "Expected i8 to be 1 byte.");

/** @brief Assert int16 to be 2 bytes.*/
STATIC_ASSERT(sizeof(int16) == 2, "Expected i16 to be 2 bytes.");

/** @brief Assert int32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(int32) == 4, "Expected i32 to be 4 bytes.");

/** @brief Assert int64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(int64) == 8, "Expected i64 to be 8 bytes.");

/** @brief Assert float32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(float32) == 4, "Expected f32 to be 4 bytes.");

/** @brief Assert float64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(float64) == 8, "Expected f64 to be 8 bytes.");

/** @brief Assert float128 to be at least 8 bytes.*/
STATIC_ASSERT(sizeof(float128) >= 8, "Expected f64 to be at least 8 bytes.");

/**********************************************************************************************************************
 *****                                                   STRUCTS                                                  *****
 **********************************************************************************************************************/

/** @brief A range, typically of memory */
typedef struct range64 {
    /** @brief The offset in bytes. */
    uint64 offset;
    /** @brief The size in bytes. */
    uint64 size;
} range64;

/** @brief A range, typically of memory */
typedef struct range32 {
    /** @brief The offset in bytes. */
    int32 offset;
    /** @brief The size in bytes. */
    int32 size;
} range32;

/**********************************************************************************************************************
 *****                                                  FUNCTIONS                                                 *****
 **********************************************************************************************************************/

N_INLINE uint64 get_aligned(const uint64 operand, const uint64 granularity)
{
    return operand + (granularity - 1) & ~(granularity - 1);
}

N_INLINE range64 get_aligned_range(const int64 offset, const uint64 size, const uint64 granularity)
{
    const range64 range = {
        .size = get_aligned(offset, granularity),
        .offset = get_aligned(size, granularity)
    };

    return range;
}

#endif //DEFINES_H