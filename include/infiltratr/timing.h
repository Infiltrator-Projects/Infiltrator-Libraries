// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file timing.h
 * @brief Portable elapsed and periodic timing policy primitives.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_TIMING_H
#define INFILTRATR_COMMON_TIMING_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Decide whether a monotonic interval should run now.
 *
 * A missing/invalid baseline, invalid interval, non-finite clock value or
 * clock rollback is treated as due so callers can safely re-establish state.
 *
 * @param [in] now Current monotonic time in caller-selected units.
 * @param [in] last Previous successful time in the same units.
 * @param [in] interval Required positive interval in the same units.
 * @return true when work is due or the baseline should be re-established.
 */
bool infiltratr_interval_due(double now, double last, double interval);

/**
 * Calculate the positive distance to the next repeating boundary.
 *
 * `position` and `period` use caller-selected but identical units. Exact
 * boundaries return one full period rather than zero, which is the useful
 * result for one-shot scheduling after a boundary has just been processed.
 * Negative positions are handled with Euclidean phase semantics.
 *
 * @return false for NULL output, non-finite input or a non-positive period;
 *         caller output is unchanged on failure.
 */
bool infiltratr_period_remaining(long double position, long double period,
                                 long double *remaining);

/**
 * Convert a positive finite duration in seconds to an upward-rounded
 * millisecond delay.
 *
 * Positive sub-millisecond durations become one millisecond. Values above the
 * uint64 range saturate to UINT64_MAX. Invalid input leaves caller output
 * unchanged and returns false.
 */
bool infiltratr_seconds_to_milliseconds_ceil(long double seconds,
                                              uint64_t *milliseconds);

/**
 * Advance a periodic deadline to the first boundary strictly after `now`.
 *
 * If `deadline` is already in the future it advances by exactly one interval,
 * matching the semantics of marking that scheduled occurrence as dispatched.
 * If one or more occurrences were missed, they are skipped without cadence
 * drift. Arithmetic saturates rather than wrapping.
 *
 * @return false for a zero interval or NULL output; caller output is unchanged
 *         on failure.
 */
bool infiltratr_periodic_deadline_advance(uint64_t deadline,
                                          uint64_t now,
                                          uint64_t interval,
                                          uint64_t *next_deadline);

#ifdef __cplusplus
}
#endif

#endif
