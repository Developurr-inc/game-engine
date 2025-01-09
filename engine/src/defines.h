//
// Created by Vinícius Ferreira Aguiar on 05/01/25.
//

#ifndef DEFINES_H
# define DEFINES_H

#include <PAL/types.h>

/**********************************************************************************************************************
 *****                                                  DEFINES                                                   *****
 **********************************************************************************************************************/

# ifdef N_EXPORT
#  ifdef _MSC_VER
#   define N_API  __declspec(dllexport)
#  else
#   define N_API  __attribute__((visibility("default")))
#  endif
# else
#  ifdef _MSC_VER
#   define N_API  __declspec(dllimport)
#  else
#   define N_API
#  endif
# endif

// Debugging

// Debug
# ifdef _DEBUG
#  define NAYLA_DEBUG

// Release
# else
#  define NAYLA_RELEASE

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
# else
#  define N_DEPRECATED(message)
// #  error "Unsupported compiler - don't know how to define deprecations!"

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
    return (operand + (granularity - 1)) & ~(granularity - 1);
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