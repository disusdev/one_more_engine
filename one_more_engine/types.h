#pragma once

#define BIT(X) (1 << X)

#define BIT_COMPARE(src, cmp) ((src & cmp) == cmp)
#define BIT_N_COMPARE(src, cmp) ((src & cmp) != cmp)

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float_t f32;
typedef double_t f64;

typedef uint8_t byte;

const f64 PI64 = std::atan(1.0) * 4.0;
const f32 PI32 = std::atan(1.0f) * 4.0f;