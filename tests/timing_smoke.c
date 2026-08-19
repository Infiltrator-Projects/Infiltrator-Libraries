// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file timing_smoke.c
 * @brief Contract tests for portable interval-due policy.
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

int main(void)
{
    assert(!infiltratr_interval_due(10.5, 10.0, 1.0));
    assert(infiltratr_interval_due(11.0, 10.0, 1.0));
    assert(infiltratr_interval_due(10.0, 0.0, 1.0));
    assert(infiltratr_interval_due(9.0, 10.0, 1.0));
    assert(infiltratr_interval_due(NAN, 10.0, 1.0));
    assert(infiltratr_interval_due(10.0, 10.0, 0.0));
    return 0;
}
