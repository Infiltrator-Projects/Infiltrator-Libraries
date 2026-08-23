// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file posix_path_smoke.c
 * @brief Regression coverage for POSIX lexical-path helpers.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/posix_path.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    assert(strcmp(infiltratr_path_basename("alpha"), "alpha") == 0);
    assert(strcmp(infiltratr_path_basename("/alpha/beta"), "beta") == 0);
    assert(strcmp(infiltratr_path_basename("/alpha/beta/"), "") == 0);
    assert(strcmp(infiltratr_path_basename("/"), "") == 0);
    assert(strcmp(infiltratr_path_basename(""), "") == 0);
    assert(strcmp(infiltratr_path_basename(NULL), "") == 0);
    puts("Infiltratr Common POSIX path tests passed.");
    return 0;
}
