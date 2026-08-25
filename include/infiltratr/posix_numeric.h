// SPDX-License-Identifier: GPL-3.0-or-later
/** @file posix_numeric.h @brief Typed signed numeric POSIX file readers. */
#ifndef INFILTRATR_COMMON_POSIX_NUMERIC_H
#define INFILTRATR_COMMON_POSIX_NUMERIC_H

#include "infiltratr/posix.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

InfiltratrIoResult infiltratr_read_i64_file_ex(const char *path,
                                               int64_t *value);
bool infiltratr_read_i64_file(const char *path, int64_t *value);

#ifdef __cplusplus
}
#endif
#endif
