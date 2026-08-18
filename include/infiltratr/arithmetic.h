// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file arithmetic.h
 * @brief Dependency-free signed arithmetic primitives shared by programs.
 *
 * These helpers cover arithmetic contracts that are easy to get subtly wrong
 * when negative values or integer limits are involved. They perform no
 * allocation and have no operating-system or toolkit dependency.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_ARITHMETIC_H
#define INFILTRATR_COMMON_ARITHMETIC_H

#include <stdbool.h>
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

#ifdef __cplusplus
}
#endif

#endif
