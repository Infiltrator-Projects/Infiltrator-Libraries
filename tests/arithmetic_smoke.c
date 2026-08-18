// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file arithmetic_smoke.c
 * @brief Regression coverage for dependency-free signed arithmetic helpers.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/arithmetic.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void test_floor_divmod(void)
{
    int64_t quotient = 77;
    int64_t remainder = 77;

    assert(infiltratr_i64_floor_divmod(7, 3, &quotient, &remainder));
    assert(quotient == 2 && remainder == 1);

    assert(infiltratr_i64_floor_divmod(-7, 3, &quotient, &remainder));
    assert(quotient == -3 && remainder == 2);

    assert(infiltratr_i64_floor_divmod(INT64_MIN, INT64_MAX,
                                       &quotient, &remainder));
    assert(quotient == -2 && remainder == INT64_MAX - 1);

    quotient = 77;
    remainder = 77;
    assert(!infiltratr_i64_floor_divmod(1, 0, &quotient, &remainder));
    assert(quotient == 77 && remainder == 77);
    assert(!infiltratr_i64_floor_divmod(1, -1, &quotient, &remainder));
    assert(quotient == 77 && remainder == 77);
    assert(!infiltratr_i64_floor_divmod(1, 1, NULL, NULL));

    assert(infiltratr_i64_floor_divmod(-1, 7, &quotient, NULL));
    assert(quotient == -1);
    assert(infiltratr_i64_floor_divmod(-1, 7, NULL, &remainder));
    assert(remainder == 6);
}

static void test_saturating_subtract(void)
{
    assert(infiltratr_i64_subtract_saturating(10, 3) == 7);
    assert(infiltratr_i64_subtract_saturating(-10, -3) == -7);
    assert(infiltratr_i64_subtract_saturating(INT64_MIN, 1) == INT64_MIN);
    assert(infiltratr_i64_subtract_saturating(INT64_MAX, -1) == INT64_MAX);
    assert(infiltratr_i64_subtract_saturating(INT64_MIN, -1) ==
           INT64_MIN + 1);
    assert(infiltratr_i64_subtract_saturating(INT64_MAX, 1) ==
           INT64_MAX - 1);
}

int main(void)
{
    test_floor_divmod();
    test_saturating_subtract();
    puts("Infiltratr Common signed arithmetic tests passed.");
    return 0;
}
