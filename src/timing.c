// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file timing.c
 * @brief Portable elapsed-time policy implementation.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/timing.h"

#include <math.h>

bool infiltratr_interval_due(double now, double last, double interval)
{
    return !isfinite(now) || !isfinite(last) || !isfinite(interval) ||
           last <= 0.0 || interval <= 0.0 || now < last ||
           now - last >= interval;
}
