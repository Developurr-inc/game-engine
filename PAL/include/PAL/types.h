//
// Created by Vinícius Ferreira Aguiar on 08/01/25.
//

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#if defined(__clang__) || defined(__GNUC__)
    #define PAL_STATIC_ASSERT  _Static_assert
#else
    #define PAL_STATIC_ASSERT  static_assert
#endif

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
PAL_STATIC_ASSERT(sizeof(bool8) == 1, "Expected bool8 to be 1 byte.");

/** @brief Assert bool16 to be 2 bytes.*/
PAL_STATIC_ASSERT(sizeof(bool16) == 2, "Expected bool16 to be 2 bytes.");

/** @brief Assert bool32 to be 4 bytes.*/
PAL_STATIC_ASSERT(sizeof(bool32) == 4, "Expected bool32 to be 4 bytes.");

/** @brief Assert bool64 to be 8 bytes.*/
PAL_STATIC_ASSERT(sizeof(bool64) == 8, "Expected bool64 to be 8 bytes.");

/** @brief Assert uint8 to be 1 byte.*/
PAL_STATIC_ASSERT(sizeof(uint8) == 1, "Expected u8 to be 1 byte.");

/** @brief Assert uint16 to be 2 bytes.*/
PAL_STATIC_ASSERT(sizeof(uint16) == 2, "Expected u16 to be 2 bytes.");

/** @brief Assert uint32 to be 4 bytes.*/
PAL_STATIC_ASSERT(sizeof(uint32) == 4, "Expected u32 to be 4 bytes.");

/** @brief Assert uint64 to be 8 bytes.*/
PAL_STATIC_ASSERT(sizeof(uint64) == 8, "Expected u64 to be 8 bytes.");

/** @brief Assert int8 to be 1 byte.*/
PAL_STATIC_ASSERT(sizeof(int8) == 1, "Expected i8 to be 1 byte.");

/** @brief Assert int16 to be 2 bytes.*/
PAL_STATIC_ASSERT(sizeof(int16) == 2, "Expected i16 to be 2 bytes.");

/** @brief Assert int32 to be 4 bytes.*/
PAL_STATIC_ASSERT(sizeof(int32) == 4, "Expected i32 to be 4 bytes.");

/** @brief Assert int64 to be 8 bytes.*/
PAL_STATIC_ASSERT(sizeof(int64) == 8, "Expected i64 to be 8 bytes.");

/** @brief Assert float32 to be 4 bytes.*/
PAL_STATIC_ASSERT(sizeof(float32) == 4, "Expected f32 to be 4 bytes.");

/** @brief Assert float64 to be 8 bytes.*/
PAL_STATIC_ASSERT(sizeof(float64) == 8, "Expected f64 to be 8 bytes.");

/** @brief Assert float128 to be at least 8 bytes.*/
PAL_STATIC_ASSERT(sizeof(float128) >= 8, "Expected f64 to be at least 8 bytes.");

#endif //TYPES_H
