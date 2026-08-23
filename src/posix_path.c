// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file posix_path.c
 * @brief POSIX lexical-path primitive implementation.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/posix_path.h"

#include <string.h>

const char *infiltratr_path_basename(const char *path)
{
    if (!path) return "";
    const char *separator = strrchr(path, '/');
    return separator ? separator + 1 : path;
}
