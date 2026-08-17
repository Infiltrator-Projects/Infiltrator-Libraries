// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file core.c
 * @brief Dependency-free implementation of shared C project primitives.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/core.h"
#include "infiltratr/compiler.h"

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static bool populated(const char *text)
{
    return text && text[0] != '\0';
}

bool infiltratr_project_info_is_valid(const InfiltratrProjectInfo *info)
{
    return info && info->struct_size >= sizeof(*info) &&
           info->abi_version == INFILTRATR_PROJECT_INFO_ABI &&
           populated(info->program_name) && populated(info->executable_name) &&
           populated(info->application_id) && populated(info->version) &&
           populated(info->source_id) && populated(info->build_profile) &&
           populated(info->author) && populated(info->website) &&
           populated(info->license_id) && populated(info->comments) &&
           populated(info->icon_name) && populated(info->copyright_text);
}

static int print_field(FILE *stream, const char *name, const char *value)
{
    if (fprintf(stream, "%s=", name) < 0) return -1;
    for (const unsigned char *cursor = (const unsigned char *)value;
         *cursor; cursor++) {
        const int character = *cursor < 32U || *cursor == 127U ? ' ' : *cursor;
        if (fputc(character, stream) == EOF) return -1;
    }
    return fputc('\n', stream) == EOF ? -1 : 0;
}

int infiltratr_project_info_print(FILE *stream,
                                  const InfiltratrProjectInfo *info)
{
    if (!stream || !infiltratr_project_info_is_valid(info)) return -1;
    if (print_field(stream, "name", info->program_name) != 0 ||
        print_field(stream, "version", info->version) != 0 ||
        print_field(stream, "common-library",
                    "infiltratr-common-" INFILTRATR_COMMON_VERSION) != 0 ||
        print_field(stream, "source-id", info->source_id) != 0 ||
        print_field(stream, "build-profile", info->build_profile) != 0 ||
        print_field(stream, "application-id", info->application_id) != 0 ||
        print_field(stream, "author", info->author) != 0 ||
        print_field(stream, "website", info->website) != 0 ||
        print_field(stream, "license", info->license_id) != 0)
        return -1;
    return ferror(stream) == 0 ? 0 : -1;
}

void infiltratr_copy_string(char *destination, size_t size,
                            const char *source)
{
    if (INFILTRATR_UNLIKELY(!destination || size == 0U)) return;
    if (!source) source = "";

    size_t length = 0U;
    while (length + 1U < size && source[length] != '\0') length++;
    memcpy(destination, source, length);
    destination[length] = '\0';
}

void infiltratr_trim(char *text)
{
    if (!text) return;

    char *start = text;
    while (*start && isspace((unsigned char)*start)) start++;
    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)end[-1])) end--;

    const size_t length = (size_t)(end - start);
    if (start != text && length > 0U) memmove(text, start, length);
    text[length] = '\0';
}

void infiltratr_trim_line_end(char *text)
{
    if (!text) return;
    size_t length = strlen(text);
    while (length > 0U &&
           (text[length - 1U] == '\n' || text[length - 1U] == '\r'))
        text[--length] = '\0';
}

bool infiltratr_string_equal(const char *left, const char *right)
{
    if (left == right) return true;
    return left && right && strcmp(left, right) == 0;
}

bool infiltratr_string_starts_with(const char *text, const char *prefix)
{
    if (!text || !prefix) return false;
    const size_t prefix_length = strlen(prefix);
    return strncmp(text, prefix, prefix_length) == 0;
}

bool infiltratr_string_ends_with(const char *text, const char *suffix)
{
    if (!text || !suffix) return false;
    const size_t text_length = strlen(text);
    const size_t suffix_length = strlen(suffix);
    return text_length >= suffix_length &&
           strcmp(text + text_length - suffix_length, suffix) == 0;
}

bool infiltratr_parse_u64(const char *text, unsigned int base,
                          uint64_t *value)
{
    if (!text || !value || base == 1U || base > 36U) return false;
    while (*text && isspace((unsigned char)*text)) text++;
    if (*text == '+' || *text == '-' || *text == '\0') return false;

    errno = 0;
    char *end = NULL;
    const unsigned long long parsed = strtoull(text, &end, (int)base);
    if (errno != 0 || end == text || parsed > (unsigned long long)UINT64_MAX)
        return false;
    while (*end && isspace((unsigned char)*end)) end++;
    if (*end != '\0') return false;
    *value = (uint64_t)parsed;
    return true;
}

bool infiltratr_parse_i64(const char *text, unsigned int base, int64_t *value)
{
    if (!text || !value || base == 1U || base > 36U) return false;
    while (*text && isspace((unsigned char)*text)) text++;
    if (*text == '\0') return false;

    errno = 0;
    char *end = NULL;
    const long long parsed = strtoll(text, &end, (int)base);
    if (errno != 0 || end == text ||
        parsed < (long long)INT64_MIN || parsed > (long long)INT64_MAX)
        return false;
    while (*end && isspace((unsigned char)*end)) end++;
    if (*end != '\0') return false;
    *value = (int64_t)parsed;
    return true;
}

bool infiltratr_parse_u64_range(const char *text, unsigned int base,
                                uint64_t minimum, uint64_t maximum,
                                uint64_t *value)
{
    if (!value || minimum > maximum) return false;
    uint64_t parsed = 0U;
    if (!infiltratr_parse_u64(text, base, &parsed) ||
        parsed < minimum || parsed > maximum)
        return false;
    *value = parsed;
    return true;
}

bool infiltratr_parse_i64_range(const char *text, unsigned int base,
                                int64_t minimum, int64_t maximum,
                                int64_t *value)
{
    if (!value || minimum > maximum) return false;
    int64_t parsed = 0;
    if (!infiltratr_parse_i64(text, base, &parsed) ||
        parsed < minimum || parsed > maximum)
        return false;
    *value = parsed;
    return true;
}

static bool ascii_space(char character)
{
    return character == ' ' || character == '\t' || character == '\n' ||
           character == '\r' || character == '\f' || character == '\v';
}

static bool ascii_digit(char character)
{
    return character >= '0' && character <= '9';
}

/*
 * Apply a base-10 exponent without constructing one huge or tiny intermediate
 * power of ten. On implementations where long double has the same exponent
 * range as double, powl(10, -N) can underflow to zero before multiplication by
 * a large significand would bring the final value back into the subnormal
 * range. Bounded 10^18 steps keep every intermediate monotonic with the final
 * magnitude, so a zero here means the represented non-zero value truly cannot
 * survive in long double rather than being an artefact of the scaling method.
 */
static bool apply_decimal_exponent(long double *value, int64_t exponent)
{
    if (!value || !isfinite(*value) || *value == 0.0L) return false;

    static const long double chunk = 1000000000000000000.0L;
    while (exponent >= 18) {
        if (*value > LDBL_MAX / chunk) return false;
        *value *= chunk;
        exponent -= 18;
    }
    while (exponent <= -18) {
        *value /= chunk;
        if (*value == 0.0L) return false;
        exponent += 18;
    }

    long double factor = 1.0L;
    if (exponent > 0) {
        for (int64_t index = 0; index < exponent; index++) factor *= 10.0L;
        if (*value > LDBL_MAX / factor) return false;
        *value *= factor;
    } else if (exponent < 0) {
        for (int64_t index = 0; index > exponent; index--) factor *= 10.0L;
        *value /= factor;
        if (*value == 0.0L) return false;
    }

    return isfinite(*value);
}

/*
 * Parse the machine-readable decimal syntax used by project configuration
 * and kernel text interfaces.  This deliberately does not call strtod():
 * strtod() follows LC_NUMERIC, while these data formats always use an ASCII
 * full stop as the decimal separator.  Keeping the grammar here also makes
 * parsing deterministic when the desktop session uses a comma-decimal locale.
 */
bool infiltratr_parse_double(const char *text, double *value)
{
    if (!text || !value) return false;

    const char *cursor = text;
    while (ascii_space(*cursor)) cursor++;

    bool negative = false;
    if (*cursor == '+' || *cursor == '-') {
        negative = *cursor == '-';
        cursor++;
    }

    bool saw_digit = false;
    bool saw_nonzero = false;
    uint64_t significand = 0U;
    unsigned int significant_digits = 0U;
    int64_t decimal_exponent = 0;
    int first_discarded_digit = -1;
    bool discarded_nonzero = false;

    while (ascii_digit(*cursor)) {
        const unsigned int digit = (unsigned int)(*cursor - '0');
        saw_digit = true;
        if (!saw_nonzero && digit == 0U) {
            cursor++;
            continue;
        }
        saw_nonzero = true;
        if (significant_digits < 19U) {
            significand = significand * 10U + (uint64_t)digit;
            significant_digits++;
        } else {
            if (first_discarded_digit < 0)
                first_discarded_digit = (int)digit;
            else if (digit != 0U)
                discarded_nonzero = true;
            if (decimal_exponent == INT64_MAX) return false;
            decimal_exponent++;
        }
        cursor++;
    }

    if (*cursor == '.') {
        cursor++;
        while (ascii_digit(*cursor)) {
            const unsigned int digit = (unsigned int)(*cursor - '0');
            saw_digit = true;
            if (!saw_nonzero && digit == 0U) {
                if (decimal_exponent == INT64_MIN) return false;
                decimal_exponent--;
                cursor++;
                continue;
            }
            saw_nonzero = true;
            if (significant_digits < 19U) {
                significand = significand * 10U + (uint64_t)digit;
                significant_digits++;
                if (decimal_exponent == INT64_MIN) return false;
                decimal_exponent--;
            } else {
                if (first_discarded_digit < 0)
                    first_discarded_digit = (int)digit;
                else if (digit != 0U)
                    discarded_nonzero = true;
            }
            cursor++;
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
            const int digit = *cursor - '0';
            if (explicit_exponent > (INT64_MAX - digit) / 10) return false;
            explicit_exponent = explicit_exponent * 10 + digit;
            cursor++;
        }
        if (exponent_negative) explicit_exponent = -explicit_exponent;
    }

    while (ascii_space(*cursor)) cursor++;
    if (*cursor != '\0') return false;

    if (!saw_nonzero) {
        *value = negative ? -0.0 : 0.0;
        return true;
    }

    if ((explicit_exponent > 0 &&
         decimal_exponent > INT64_MAX - explicit_exponent) ||
        (explicit_exponent < 0 &&
         decimal_exponent < INT64_MIN - explicit_exponent))
        return false;
    decimal_exponent += explicit_exponent;

    /* Round the retained 19 significant digits before scaling. */
    if (first_discarded_digit > 5 ||
        (first_discarded_digit == 5 &&
         (discarded_nonzero || (significand & 1U) != 0U))) {
        significand++;
    }

    /* Any exponent outside this generous range cannot produce a finite double. */
    if (decimal_exponent > 400 || decimal_exponent < -400) return false;

    long double parsed = (long double)significand;
    if (decimal_exponent != 0 &&
        !apply_decimal_exponent(&parsed, decimal_exponent))
        return false;
    if (negative) parsed = -parsed;
    if (!isfinite(parsed)) return false;

    const double converted = (double)parsed;
    if (!isfinite(converted) || (converted == 0.0 && parsed != 0.0L))
        return false;
    *value = converted;
    return true;
}

bool infiltratr_parse_double_range(const char *text, double minimum,
                                   double maximum, double *value)
{
    if (!value || isnan(minimum) || isnan(maximum) || minimum > maximum)
        return false;
    double parsed = 0.0;
    if (!infiltratr_parse_double(text, &parsed) ||
        parsed < minimum || parsed > maximum)
        return false;
    *value = parsed;
    return true;
}

double infiltratr_clamp_double(double value, double lower, double upper)
{
    if (isnan(value) || isnan(lower) || isnan(upper) || lower > upper)
        return value;
    if (value < lower) return lower;
    return value > upper ? upper : value;
}

bool infiltratr_u64_add_checked(uint64_t left, uint64_t right,
                                uint64_t *result)
{
    if (!result || right > UINT64_MAX - left) return false;
    *result = left + right;
    return true;
}

uint64_t infiltratr_u64_add_saturating(uint64_t left, uint64_t right)
{
    return right > UINT64_MAX - left ? UINT64_MAX : left + right;
}

uint64_t infiltratr_u64_multiply_saturating(uint64_t left, uint64_t right)
{
    return left != 0U && right > UINT64_MAX / left
        ? UINT64_MAX : left * right;
}

double infiltratr_percent_u64(uint64_t part, uint64_t whole)
{
    if (whole == 0U) return 0.0;
    const long double percentage =
        100.0L * (long double)part / (long double)whole;
    if (percentage <= 0.0L) return 0.0;
    return percentage >= 100.0L ? 100.0 : (double)percentage;
}

bool infiltratr_u64_counter_rate(uint64_t current, uint64_t previous,
                                 long double units_per_count,
                                 double elapsed_seconds, double *rate)
{
    if (!rate) return false;
    *rate = 0.0;
    if (current < previous || units_per_count < 0.0L ||
        !isfinite(units_per_count) || elapsed_seconds <= 0.0 ||
        !isfinite(elapsed_seconds))
        return false;

    const long double calculated =
        (long double)(current - previous) * units_per_count /
        (long double)elapsed_seconds;
    if (!isfinite(calculated) || calculated > (long double)DBL_MAX)
        return false;
    *rate = (double)calculated;
    return true;
}

static bool scale_options_valid(const InfiltratrScaleOptions *options,
                                size_t unit_count, size_t *maximum_unit)
{
    if (!options || options->struct_size < sizeof(*options) ||
        options->abi_version != INFILTRATR_SCALE_OPTIONS_ABI ||
        !isfinite(options->divisor) || options->divisor <= 1.0L ||
        unit_count == 0U || options->minimum_unit >= unit_count ||
        options->decimal_places > 9U ||
        !isfinite(options->integer_threshold) ||
        options->integer_threshold < 0.0L)
        return false;

    const size_t maximum = options->maximum_unit == SIZE_MAX
        ? unit_count - 1U : options->maximum_unit;
    if (maximum >= unit_count || maximum < options->minimum_unit)
        return false;
    if (maximum_unit) *maximum_unit = maximum;
    return true;
}

bool infiltratr_scale_quantity(long double value,
                               const InfiltratrScaleOptions *options,
                               size_t unit_count, long double *scaled_value,
                               size_t *unit_index)
{
    size_t maximum_unit = 0U;
    if (!scaled_value || !unit_index || !isfinite(value) ||
        !scale_options_valid(options, unit_count, &maximum_unit))
        return false;

    const long double magnitude = fabsl(value);
    long double minimum_boundary = 1.0L;
    for (size_t index = 0U; index < options->minimum_unit; index++) {
        if (minimum_boundary > LDBL_MAX / options->divisor) {
            minimum_boundary = LDBL_MAX;
            break;
        }
        minimum_boundary *= options->divisor;
    }

    size_t unit = options->minimum_unit;
    if (options->zero_below_minimum_unit && magnitude < minimum_boundary) {
        *scaled_value = 0.0L;
        *unit_index = unit;
        return true;
    }

    long double scaled = value;
    for (size_t index = 0U; index < unit; index++)
        scaled /= options->divisor;

    while (unit < maximum_unit && fabsl(scaled) >= options->divisor) {
        scaled /= options->divisor;
        unit++;
    }

    *scaled_value = scaled;
    *unit_index = unit;
    return true;
}

bool infiltratr_format_scaled_quantity(long double value,
                                        const char *const *units,
                                        size_t unit_count,
                                        const char *suffix,
                                        const InfiltratrScaleOptions *options,
                                        char *buffer, size_t buffer_size)
{
    if (buffer && buffer_size > 0U) buffer[0] = '\0';
    if (!units || !buffer || buffer_size == 0U) return false;

    long double scaled = 0.0L;
    size_t unit = 0U;
    if (!infiltratr_scale_quantity(value, options, unit_count,
                                   &scaled, &unit) || !units[unit])
        return false;

    unsigned int decimal_places = options->decimal_places;
    if ((options->integer_at_minimum_unit &&
         unit == options->minimum_unit) ||
        (options->integer_threshold > 0.0L &&
         fabsl(scaled) >= options->integer_threshold))
        decimal_places = 0U;

    const char *actual_suffix = suffix ? suffix : "";
    const int written = snprintf(buffer, buffer_size, "%.*Lf %s%s",
                                 (int)decimal_places, scaled, units[unit],
                                 actual_suffix);
    return written >= 0 && (size_t)written < buffer_size;
}

char *infiltratr_format_bytes(uint64_t bytes, char *buffer,
                              size_t buffer_size)
{
    static const char *const units[] = {"B", "KB", "MB", "GB", "TB"};
    const InfiltratrScaleOptions options = INFILTRATR_SCALE_OPTIONS_INIT;
    (void)infiltratr_format_scaled_quantity((long double)bytes,
                                             units,
                                             INFILTRATR_ARRAY_LENGTH(units),
                                             "", &options, buffer,
                                             buffer_size);
    return buffer;
}

char *infiltratr_format_rate(double bytes_per_second, char *buffer,
                             size_t buffer_size)
{
    if (!isfinite(bytes_per_second) || bytes_per_second < 0.0)
        bytes_per_second = 0.0;
    static const char *const units[] = {"B", "KB", "MB", "GB", "TB"};
    const InfiltratrScaleOptions options = INFILTRATR_SCALE_OPTIONS_INIT;
    (void)infiltratr_format_scaled_quantity((long double)bytes_per_second,
                                             units,
                                             INFILTRATR_ARRAY_LENGTH(units),
                                             "/s", &options, buffer,
                                             buffer_size);
    return buffer;
}
