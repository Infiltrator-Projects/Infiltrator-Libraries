// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/quantity.h"
#include "infiltratr/arithmetic.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
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

static bool ascii_space(unsigned char value)
{
    return value == ' ' || value == '\t' || value == '\r' ||
           value == '\n' || value == '\f' || value == '\v';
}

static bool ascii_alpha(unsigned char value)
{
    return (value >= 'A' && value <= 'Z') ||
           (value >= 'a' && value <= 'z');
}

static bool ascii_digit(unsigned char value)
{
    return value >= '0' && value <= '9';
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

typedef struct {
    char digits[128];
    size_t length;
    int64_t exponent10;
} ExactDecimal;

static bool parse_exact_decimal(const char *text, ExactDecimal *decimal)
{
    if (!text || !decimal) return false;

    const unsigned char *cursor = (const unsigned char *)text;
    if (*cursor == '+')
        cursor++;
    else if (*cursor == '-')
        return false;

    char digits[128];
    size_t digit_count = 0U;
    size_t fractional_digits = 0U;
    bool saw_digit = false;

    while (ascii_digit(*cursor)) {
        if (digit_count + 1U >= sizeof(digits)) return false;
        digits[digit_count++] = (char)*cursor++;
        saw_digit = true;
    }
    if (*cursor == '.') {
        cursor++;
        while (ascii_digit(*cursor)) {
            if (digit_count + 1U >= sizeof(digits)) return false;
            digits[digit_count++] = (char)*cursor++;
            fractional_digits++;
            saw_digit = true;
        }
    }
    if (!saw_digit) return false;

    int64_t explicit_exponent = 0;
    bool exponent_negative = false;
    if (*cursor == 'e' || *cursor == 'E') {
        cursor++;
        if (*cursor == '+' || *cursor == '-') {
            exponent_negative = *cursor == '-';
            cursor++;
        }
        if (!ascii_digit(*cursor)) return false;
        while (ascii_digit(*cursor)) {
            const int digit = (int)(*cursor - '0');
            if (explicit_exponent > (INT64_MAX - digit) / 10) return false;
            explicit_exponent = explicit_exponent * 10 + digit;
            cursor++;
        }
        if (exponent_negative) explicit_exponent = -explicit_exponent;
    }
    if (*cursor != '\0') return false;
    if (fractional_digits > (size_t)INT64_MAX) return false;
    if (explicit_exponent < INT64_MIN + (int64_t)fractional_digits)
        return false;

    int64_t exponent10 = explicit_exponent - (int64_t)fractional_digits;
    size_t first = 0U;
    while (first < digit_count && digits[first] == '0') first++;
    if (first == digit_count) {
        decimal->digits[0] = '0';
        decimal->digits[1] = '\0';
        decimal->length = 1U;
        decimal->exponent10 = 0;
        return true;
    }

    size_t last = digit_count;
    while (last > first && digits[last - 1U] == '0') {
        if (exponent10 == INT64_MAX) return false;
        exponent10++;
        last--;
    }

    decimal->length = last - first;
    memcpy(decimal->digits, digits + first, decimal->length);
    decimal->digits[decimal->length] = '\0';
    decimal->exponent10 = exponent10;
    return true;
}

static bool decimal_divide_small(ExactDecimal *decimal, unsigned int divisor)
{
    unsigned int remainder = 0U;
    size_t out = 0U;

    for (size_t i = 0U; i < decimal->length; ++i) {
        const unsigned int value = remainder * 10U +
            (unsigned int)(decimal->digits[i] - '0');
        const unsigned int quotient = value / divisor;
        remainder = value % divisor;
        if (quotient != 0U || out != 0U)
            decimal->digits[out++] = (char)('0' + quotient);
    }
    if (remainder != 0U) return false;
    if (out == 0U) decimal->digits[out++] = '0';
    decimal->digits[out] = '\0';
    decimal->length = out;
    return true;
}

static bool decimal_to_u64(const ExactDecimal *decimal, uint64_t *value)
{
    uint64_t result = 0U;
    for (size_t i = 0U; i < decimal->length; ++i) {
        const uint64_t digit = (uint64_t)(decimal->digits[i] - '0');
        if (result > (UINT64_MAX - digit) / 10U) return false;
        result = result * 10U + digit;
    }
    *value = result;
    return true;
}

static bool multiply_power(uint64_t *value, uint64_t factor, uint64_t exponent)
{
    uint64_t result = *value;
    for (uint64_t i = 0U; i < exponent; ++i) {
        if (!infiltratr_u64_multiply_checked(result, factor, &result))
            return false;
    }
    *value = result;
    return true;
}

bool infiltratr_parse_binary_quantity_u64(const char *text, uint64_t *bytes)
{
    if (!text || !bytes) return false;
    while (*text && ascii_space((unsigned char)*text)) text++;
    const char *end = text + strlen(text);
    while (end > text && ascii_space((unsigned char)end[-1])) end--;
    if (end == text || (size_t)(end - text) >= 160U) return false;

    const char *suffix_start = end;
    while (suffix_start > text && ascii_alpha((unsigned char)suffix_start[-1]))
        suffix_start--;

    char suffix[8];
    const size_t suffix_length = (size_t)(end - suffix_start);
    if (suffix_length >= sizeof(suffix)) return false;
    memcpy(suffix, suffix_start, suffix_length);
    suffix[suffix_length] = '\0';

    unsigned int power = 0U;
    if (!suffix_power(suffix, &power)) return false;

    const char *number_end = suffix_start;
    while (number_end > text && ascii_space((unsigned char)number_end[-1]))
        number_end--;
    const size_t number_length = (size_t)(number_end - text);
    if (number_length == 0U || number_length >= 128U) return false;

    char number[128];
    memcpy(number, text, number_length);
    number[number_length] = '\0';

    ExactDecimal decimal;
    if (!parse_exact_decimal(number, &decimal)) return false;
    if (decimal.length == 1U && decimal.digits[0] == '0') {
        *bytes = 0U;
        return true;
    }

    const uint64_t binary_power = (uint64_t)power * 10U;
    uint64_t result = 0U;

    if (decimal.exponent10 >= 0) {
        if (decimal.exponent10 > 19) return false;
        if (!decimal_to_u64(&decimal, &result) ||
            !multiply_power(&result, 10U, (uint64_t)decimal.exponent10) ||
            !multiply_power(&result, 2U, binary_power))
            return false;
    } else {
        if (decimal.exponent10 == INT64_MIN) return false;
        const uint64_t denominator_power =
            (uint64_t)(-decimal.exponent10);

        /* A non-zero <128-digit decimal cannot contain 5^513 as a factor. */
        if (denominator_power > 512U) return false;
        for (uint64_t i = 0U; i < denominator_power; ++i)
            if (!decimal_divide_small(&decimal, 5U)) return false;

        const uint64_t twos_to_divide = denominator_power > binary_power
            ? denominator_power - binary_power : 0U;
        for (uint64_t i = 0U; i < twos_to_divide; ++i)
            if (!decimal_divide_small(&decimal, 2U)) return false;

        if (!decimal_to_u64(&decimal, &result)) return false;
        const uint64_t twos_to_multiply = binary_power > denominator_power
            ? binary_power - denominator_power : 0U;
        if (!multiply_power(&result, 2U, twos_to_multiply)) return false;
    }

    *bytes = result;
    return true;
}
