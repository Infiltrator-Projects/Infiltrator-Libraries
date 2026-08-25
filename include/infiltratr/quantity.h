// SPDX-License-Identifier: GPL-3.0-or-later
/** @file quantity.h @brief Strict locale-independent binary quantity parsing. */
#ifndef INFILTRATR_COMMON_QUANTITY_H
#define INFILTRATR_COMMON_QUANTITY_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Parse a non-negative binary quantity into whole bytes.
 *
 * Accepted suffixes are B, K/KB/KiB through E/EB/EiB, case-insensitively.
 * No suffix means bytes. Fractional input is accepted only when scaling yields
 * an exact whole-byte result. Surrounding ASCII whitespace is allowed.
 * Caller output is unchanged on failure.
 */
bool infiltratr_parse_binary_quantity_u64(const char *text, uint64_t *bytes);

#ifdef __cplusplus
}
#endif
#endif
