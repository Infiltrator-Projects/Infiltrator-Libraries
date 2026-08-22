// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file utf8.h
 * @brief Allocation-free strict UTF-8 validation.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_UTF8_H
#define INFILTRATR_COMMON_UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Validate one bounded UTF-8 byte sequence.
 *
 * The validator rejects truncated sequences, stray continuation bytes,
 * overlong encodings, UTF-16 surrogate code points and values above U+10FFFF.
 * Embedded NUL bytes are ordinary U+0000 code points; callers that forbid NUL
 * in names or text fields apply that policy separately.
 */
static inline bool infiltratr_utf8_validate(const void *bytes, size_t length)
{
    const uint8_t *p = (const uint8_t *)bytes;
    size_t i = 0U;

    if (!p && length != 0U)
        return false;

    while (i < length) {
        const uint8_t first = p[i++];
        if (first <= 0x7fU)
            continue;

        uint32_t value;
        unsigned int continuation;
        uint32_t minimum;
        if (first >= 0xc2U && first <= 0xdfU) {
            value = first & 0x1fU;
            continuation = 1U;
            minimum = 0x80U;
        } else if (first >= 0xe0U && first <= 0xefU) {
            value = first & 0x0fU;
            continuation = 2U;
            minimum = 0x800U;
        } else if (first >= 0xf0U && first <= 0xf4U) {
            value = first & 0x07U;
            continuation = 3U;
            minimum = 0x10000U;
        } else {
            return false;
        }

        if ((size_t)continuation > length - i)
            return false;
        for (unsigned int n = 0U; n < continuation; ++n) {
            const uint8_t next = p[i++];
            if ((next & 0xc0U) != 0x80U)
                return false;
            value = (value << 6) | (next & 0x3fU);
        }

        if (value < minimum || value > 0x10ffffU ||
            (value >= 0xd800U && value <= 0xdfffU))
            return false;
    }

    return true;
}

#ifdef __cplusplus
}
#endif

#endif
