// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file token.h
 * @brief Allocation-free token parsing for structured text interfaces.
 *
 * This module complements the strict whole-string parsers in core.h. Token
 * parsing is intended for procfs, sysfs, text protocols and similar records
 * where one numeric field is followed by more fields in the same buffer.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_TOKEN_H
#define INFILTRATR_COMMON_TOKEN_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Parse one unsigned integer token and advance a caller cursor.
 *
 * Leading C-library whitespace is skipped. A leading sign is rejected. `base`
 * may be 0 for C-style base detection or 2..36. Parsing stops at the first
 * byte not belonging to the integer; trailing text is deliberately permitted.
 * On failure neither `*cursor` nor `*value` is changed.
 */
bool infiltratr_parse_u64_token(const char **cursor, unsigned int base,
                                uint64_t *value);

#ifdef __cplusplus
}
#endif

#endif
