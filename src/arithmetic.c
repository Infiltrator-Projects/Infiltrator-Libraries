// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file arithmetic.c
 * @brief Dependency-free implementation of shared signed arithmetic.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/arithmetic.h"

#include <limits.h>

bool infiltratr_i64_floor_divmod(int64_t value, int64_t divisor,
                                 int64_t *quotient, int64_t *remainder)
{
    if (divisor <= 0 || (!quotient && !remainder)) return false;

    int64_t q = value / divisor;
    int64_t r = value % divisor;
    if (r < 0) {
        q--;
        r += divisor;
    }

    if (quotient) *quotient = q;
    if (remainder) *remainder = r;
    return true;
}

int64_t infiltratr_i64_subtract_saturating(int64_t left, int64_t right)
{
    if (right > 0 && left < INT64_MIN + right) return INT64_MIN;
    if (right < 0 && left > INT64_MAX + right) return INT64_MAX;
    return left - right;
}
