// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file timing.c
 * @brief Portable elapsed and periodic timing policy implementation.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/timing.h"

#include "infiltratr/core.h"

#include <math.h>
#include <stdint.h>

bool infiltratr_interval_due(double now, double last, double interval)
{
    return !isfinite(now) || !isfinite(last) || !isfinite(interval) ||
           last <= 0.0 || interval <= 0.0 || now < last ||
           now - last >= interval;
}

bool infiltratr_period_remaining(long double position, long double period,
                                 long double *remaining)
{
    long double phase;
    long double result;

    if (remaining == NULL || !isfinite(position) || !isfinite(period) ||
        period <= 0.0L) {
        return false;
    }

    phase = fmodl(position, period);
    if (phase < 0.0L) {
        phase += period;
    }

    result = period - phase;
    if (result <= 0.0L || result > period) {
        result = period;
    }
    *remaining = result;
    return true;
}

bool infiltratr_seconds_to_milliseconds_ceil(long double seconds,
                                              uint64_t *milliseconds)
{
    long double value;

    if (milliseconds == NULL || !isfinite(seconds) || seconds <= 0.0L) {
        return false;
    }

    value = ceill(seconds * 1000.0L);
    if (value < 1.0L) {
        value = 1.0L;
    }
    if (value >= (long double)UINT64_MAX) {
        *milliseconds = UINT64_MAX;
        return true;
    }

    *milliseconds = (uint64_t)value;
    return true;
}

bool infiltratr_periodic_deadline_advance(uint64_t deadline,
                                          uint64_t now,
                                          uint64_t interval,
                                          uint64_t *next_deadline)
{
    uint64_t result;

    if (next_deadline == NULL || interval == 0U) {
        return false;
    }

    if (deadline > now) {
        result = infiltratr_u64_add_saturating(deadline, interval);
    } else {
        const uint64_t late_by = now - deadline;
        const uint64_t steps = infiltratr_u64_add_saturating(
            late_by / interval, 1U);
        const uint64_t advance =
            infiltratr_u64_multiply_saturating(interval, steps);

        result = infiltratr_u64_add_saturating(deadline, advance);
    }

    *next_deadline = result;
    return true;
}
