// SPDX-License-Identifier: GPL-3.0-or-later
/** @file token.h @brief Allocation-free numeric token parsing. */
#ifndef INFILTRATR_COMMON_TOKEN_H
#define INFILTRATR_COMMON_TOKEN_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool infiltratr_parse_u64_token(const char **cursor, unsigned int base,
                                uint64_t *value);
bool infiltratr_parse_i64_token(const char **cursor, unsigned int base,
                                int64_t *value);

#ifdef __cplusplus
}
#endif
#endif
