// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/quantity.h"
#include "infiltratr/core.h"

#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

static int ascii_equal_ci(const char *left, const char *right)
{
    while (*left && *right) {
        unsigned char a = (unsigned char)*left++;
        unsigned char b = (unsigned char)*right++;
        if (a >= 'A' && a <= 'Z') a = (unsigned char)(a + ('a' - 'A'));
        if (b >= 'A' && b <= 'Z') b = (unsigned char)(b + ('a' - 'A'));
        if (a != b) return 0;
    }
    return *left == '\0' && *right == '\0';
}

static bool suffix_power(const char *suffix, unsigned int *power)
{
    static const char *const short_names[] = {"", "k", "m", "g", "t", "p", "e"};
    static const char *const byte_names[] = {"b", "kb", "mb", "gb", "tb", "pb", "eb"};
    static const char *const iec_names[] = {"b", "kib", "mib", "gib", "tib", "pib", "eib"};
    for (unsigned int i = 0U; i < 7U; ++i) {
        if (ascii_equal_ci(suffix, short_names[i]) ||
            ascii_equal_ci(suffix, byte_names[i]) ||
            ascii_equal_ci(suffix, iec_names[i])) {
            *power = i;
            return true;
        }
    }
    return false;
}

bool infiltratr_parse_binary_quantity_u64(const char *text, uint64_t *bytes)
{
    if (!text || !bytes) return false;
    while (*text && isspace((unsigned char)*text)) text++;
    const char *end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) end--;
    if (end == text || (size_t)(end - text) >= 160U) return false;

    const char *suffix_start = end;
    while (suffix_start > text && isalpha((unsigned char)suffix_start[-1]))
        suffix_start--;

    char suffix[8];
    const size_t suffix_length = (size_t)(end - suffix_start);
    if (suffix_length >= sizeof(suffix)) return false;
    memcpy(suffix, suffix_start, suffix_length);
    suffix[suffix_length] = '\0';

    unsigned int power = 0U;
    if (!suffix_power(suffix, &power)) return false;

    const char *number_end = suffix_start;
    while (number_end > text && isspace((unsigned char)number_end[-1])) number_end--;
    const size_t number_length = (size_t)(number_end - text);
    if (number_length == 0U || number_length >= 128U) return false;
    char number[128];
    memcpy(number, text, number_length);
    number[number_length] = '\0';

    double parsed = 0.0;
    if (!infiltratr_parse_double(number, &parsed) || parsed < 0.0 || !isfinite(parsed))
        return false;

    long double scaled = (long double)parsed;
    for (unsigned int i = 0U; i < power; ++i) {
        if (scaled > (long double)UINT64_MAX / 1024.0L) return false;
        scaled *= 1024.0L;
    }
    if (!isfinite(scaled) || scaled < 0.0L || scaled > (long double)UINT64_MAX)
        return false;
    const uint64_t result = (uint64_t)scaled;
    if ((long double)result != scaled) return false;
    *bytes = result;
    return true;
}
