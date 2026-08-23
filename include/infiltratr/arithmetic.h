// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file arithmetic.h
 * @brief Dependency-free arithmetic and allocation-size primitives.
 *
 * These helpers cover arithmetic contracts that are easy to get subtly wrong
 * around signed limits, size overflow and contiguous-array growth. They have no
 * operating-system or toolkit dependency; the reserve helper uses only the C
 * allocator and leaves caller ownership explicit.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_ARITHMETIC_H
#define INFILTRATR_COMMON_ARITHMETIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Divide a signed value by a strictly positive divisor using floor semantics.
 *
 * C integer division truncates toward zero; this operation instead returns the
 * mathematical floor quotient and a non-negative remainder satisfying
 * `value = quotient * divisor + remainder` and `0 <= remainder < divisor`.
 * Either output may be NULL, but at least one output must be supplied.
 *
 * @return true on success; false when `divisor <= 0` or both outputs are NULL.
 *         Caller outputs are left unchanged on failure.
 */
bool infiltratr_i64_floor_divmod(int64_t value, int64_t divisor,
                                 int64_t *quotient, int64_t *remainder);

/**
 * Subtract two signed values without invoking signed-overflow undefined
 * behaviour. Results above INT64_MAX saturate to INT64_MAX; results below
 * INT64_MIN saturate to INT64_MIN.
 */
int64_t infiltratr_i64_subtract_saturating(int64_t left, int64_t right);

/**
 * Add two allocation sizes while rejecting `size_t` overflow.
 *
 * @return false for NULL output or overflow; caller output is unchanged on
 *         failure.
 */
bool infiltratr_size_add_checked(size_t left, size_t right, size_t *result);

/**
 * Multiply two allocation sizes while rejecting `size_t` overflow.
 *
 * @return false for NULL output or overflow; caller output is unchanged on
 *         failure.
 */
bool infiltratr_size_multiply_checked(size_t left, size_t right,
                                      size_t *result);

/**
 * Ensure a caller-owned contiguous array can hold `required` elements.
 *
 * Capacity grows geometrically from the current capacity or
 * `initial_capacity` (one when that value is zero). Byte-size overflow is
 * rejected before realloc. On any failure, both `*array` and `*capacity`
 * remain unchanged. Existing contents are preserved by realloc on success.
 *
 * `element_size` must be non-zero. A positive incoming capacity requires a
 * non-NULL incoming array pointer so stale/inconsistent ownership state is
 * rejected instead of silently repaired.
 */
bool infiltratr_array_reserve(void **array, size_t *capacity,
                              size_t element_size, size_t required,
                              size_t initial_capacity);

#ifdef __cplusplus
}
#endif

#endif
