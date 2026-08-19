// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file timing.h
 * @brief Portable elapsed-time policy primitives.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_TIMING_H
#define INFILTRATR_COMMON_TIMING_H

#include <stdbool.h>

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

#ifdef __cplusplus
}
#endif

#endif
