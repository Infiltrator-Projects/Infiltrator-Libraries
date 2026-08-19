// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file timing_smoke.c
 * @brief Contract tests for portable elapsed and periodic timing policy.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/timing.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <math.h>
#include <stdint.h>

int main(void)
{
    long double remaining = 0.0L;
    uint64_t milliseconds = 0U;
    uint64_t deadline = 0U;

    assert(!infiltratr_interval_due(10.5, 10.0, 1.0));
    assert(infiltratr_interval_due(11.0, 10.0, 1.0));
    assert(infiltratr_interval_due(10.0, 0.0, 1.0));
    assert(infiltratr_interval_due(9.0, 10.0, 1.0));
    assert(infiltratr_interval_due(NAN, 10.0, 1.0));
    assert(infiltratr_interval_due(10.0, 10.0, 0.0));

    assert(infiltratr_period_remaining(0.0L, 60.0L, &remaining));
    assert(fabsl(remaining - 60.0L) < 1.0e-12L);
    assert(infiltratr_period_remaining(59.5L, 60.0L, &remaining));
    assert(fabsl(remaining - 0.5L) < 1.0e-12L);
    assert(infiltratr_period_remaining(-1.0L, 60.0L, &remaining));
    assert(fabsl(remaining - 1.0L) < 1.0e-12L);
    remaining = 77.0L;
    assert(!infiltratr_period_remaining(1.0L, 0.0L, &remaining));
    assert(remaining == 77.0L);

    assert(infiltratr_seconds_to_milliseconds_ceil(
        0.0001L, &milliseconds));
    assert(milliseconds == 1U);
    assert(infiltratr_seconds_to_milliseconds_ceil(
        1.0001L, &milliseconds));
    assert(milliseconds == 1001U);
    milliseconds = 77U;
    assert(!infiltratr_seconds_to_milliseconds_ceil(0.0L, &milliseconds));
    assert(milliseconds == 77U);

    assert(infiltratr_periodic_deadline_advance(100U, 50U, 10U,
                                                &deadline));
    assert(deadline == 110U);
    assert(infiltratr_periodic_deadline_advance(100U, 100U, 10U,
                                                &deadline));
    assert(deadline == 110U);
    assert(infiltratr_periodic_deadline_advance(100U, 135U, 10U,
                                                &deadline));
    assert(deadline == 140U);
    assert(infiltratr_periodic_deadline_advance(UINT64_MAX - 2U,
                                                UINT64_MAX - 3U,
                                                10U,
                                                &deadline));
    assert(deadline == UINT64_MAX);
    deadline = 77U;
    assert(!infiltratr_periodic_deadline_advance(1U, 1U, 0U, &deadline));
    assert(deadline == 77U);
    return 0;
}
