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
 * This floating-point form is intended for genuinely continuous quantities.
 * When position and period are integral, use
 * infiltratr_i64_period_remaining() so boundary classification is exact.
 *
 * @return false for NULL output, non-finite input or a non-positive period;
 *         caller output is unchanged on failure.
 */
bool infiltratr_period_remaining(long double position, long double period,
                                 long double *remaining);

/**
 * Calculate the exact positive integer distance to the next repeating boundary.
 *
 * `period` must be strictly positive. The position may be any signed 64-bit
 * value; Euclidean remainder semantics make negative positions behave as the
 * mathematical repeating phase. An exact boundary returns one full period.
 *
 * @return false for NULL output or a non-positive period. Caller output is
 *         unchanged on failure.
 */
bool infiltratr_i64_period_remaining(int64_t position, int64_t period,
                                     uint64_t *remaining);

/**
 * Partition an integer cycle into an arbitrary number of equal rational parts.
 *
 * `position` is reduced modulo `cycle_length`. `partition_index` receives the
 * exact value
 *
 *   floor((position mod cycle_length) * partition_count / cycle_length)
 *
 * without overflowing a 64-bit intermediate, even when the mathematical
 * product exceeds UINT64_MAX. `units_until_next`, when requested, receives the
 * smallest positive whole number of input units that is not earlier than the
 * next exact rational partition boundary. At an exact boundary this therefore
 * describes the complete next partition rather than zero.
 *
 * Either output may be NULL, but at least one output must be supplied. This
 * operation performs no floating-point arithmetic and is exact across the
 * complete uint64_t input domain.
 *
 * @return false for zero cycle/partition counts or when both outputs are NULL;
 *         supplied outputs are unchanged on failure.
 */
bool infiltratr_cycle_partition_u64(uint64_t position,
                                    uint64_t cycle_length,
                                    uint64_t partition_count,
                                    uint64_t *partition_index,
                                    uint64_t *units_until_next);

/**
 * Convert a positive whole-microsecond duration to an upward-rounded
 * millisecond delay using exact integer arithmetic.
 *
 * Positive sub-millisecond values become one millisecond. The complete
 * uint64_t microsecond domain is representable after division, so this
 * operation cannot overflow its result.
 *
 * @return false for NULL output or zero duration; caller output is unchanged
 *         on failure.
 */
bool infiltratr_microseconds_to_milliseconds_ceil(uint64_t microseconds,
                                                   uint64_t *milliseconds);

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
