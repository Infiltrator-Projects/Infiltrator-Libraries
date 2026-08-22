// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file core.h
 * @brief Dependency-free C primitives and project identity shared by programs.
 *
 * This interface is C11-compatible and does not depend on GLib, GTK or an
 * operating-system API. Applications may therefore use the same source on
 * Linux and retain a clean boundary for future platform providers.
 *
 * Unless a function documents otherwise, caller-owned output storage remains
 * owned by the caller, no allocation is performed, and functions are
 * re-entrant provided callers do not concurrently modify the same objects.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_CORE_H
#define INFILTRATR_COMMON_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INFILTRATR_COMMON_VERSION "1.11.0"
#define INFILTRATR_PROJECT_INFO_ABI 1U
#define INFILTRATR_SCALE_OPTIONS_ABI 1U
#define INFILTRATR_ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))

/**
 * Stable, toolkit-neutral project identity consumed by application adapters.
 *
 * `struct_size` and `abi_version` form the ABI header. Version 1 validation
 * requires `struct_size` to be at least the size of the current structure and
 * `abi_version` to equal `INFILTRATR_PROJECT_INFO_ABI`. Every string field is
 * required and must point to a non-empty NUL-terminated string for a record to
 * be considered valid.
 */
typedef struct {
    size_t struct_size;
    uint32_t abi_version;
    const char *program_name;
    const char *executable_name;
    const char *application_id;
    const char *version;
    const char *source_id;
    const char *build_profile;
    const char *author;
    const char *website;
    const char *license_id;
    const char *comments;
    const char *icon_name;
    const char *copyright_text;
} InfiltratrProjectInfo;

/** Initialise a project record while keeping added ABI fields zeroed. */
#define INFILTRATR_PROJECT_INFO_INIT \
    { .struct_size = sizeof(InfiltratrProjectInfo), \
      .abi_version = INFILTRATR_PROJECT_INFO_ABI }

/**
 * Scaling policy used by the canonical shared quantity formatter.
 *
 * `minimum_unit` and `maximum_unit` are inclusive indices into the unit table
 * supplied by the caller. `SIZE_MAX` for `maximum_unit` means the last unit.
 * The formatter can therefore auto-scale, force one unit, or constrain a
 * display to any useful range without duplicating the scaling algorithm.
 *
 * `divisor` must be finite and greater than 1. `decimal_places` is limited to
 * 0..9. `integer_threshold` must be finite and non-negative. The ABI header
 * follows the same rule as `InfiltratrProjectInfo`: callers initialise the
 * complete structure and the implementation rejects an incompatible ABI.
 */
typedef struct {
    size_t struct_size;
    uint32_t abi_version;
    long double divisor;
    size_t minimum_unit;
    size_t maximum_unit;
    unsigned int decimal_places;
    long double integer_threshold;
    bool integer_at_minimum_unit;
    bool zero_below_minimum_unit;
} InfiltratrScaleOptions;

/** Default binary auto-scaling policy with one fractional digit. */
#define INFILTRATR_SCALE_OPTIONS_INIT \
    { .struct_size = sizeof(InfiltratrScaleOptions), \
      .abi_version = INFILTRATR_SCALE_OPTIONS_ABI, \
      .divisor = 1024.0L, \
      .minimum_unit = 0U, \
      .maximum_unit = SIZE_MAX, \
      .decimal_places = 1U, \
      .integer_threshold = 100.0L, \
      .integer_at_minimum_unit = true, \
      .zero_below_minimum_unit = false }

/**
 * Validate a project identity record.
 *
 * @param info Record to validate; may be NULL.
 * @return true only when the ABI header is compatible and every required
 *         identity string is non-NULL and non-empty.
 */
bool infiltratr_project_info_is_valid(const InfiltratrProjectInfo *info);

/**
 * Write stable machine-readable project metadata as `key=value` lines.
 *
 * Embedded C0 control bytes and DEL in emitted values are replaced with a
 * space so one logical field cannot create additional metadata lines. The
 * function does not close or flush `stream`.
 *
 * @param stream Writable stdio stream.
 * @param info Valid project identity record.
 * @return 0 on success; -1 for invalid arguments or any stream write error.
 */
int infiltratr_project_info_print(FILE *stream,
                                  const InfiltratrProjectInfo *info);

/**
 * Copy a possibly-NULL string into bounded caller storage.
 *
 * `source == NULL` is treated as the empty string. With a non-NULL destination
 * and `size > 0`, the result is always NUL-terminated; input longer than the
 * available payload is silently truncated. A NULL destination or zero size is
 * a no-op.
 */
void infiltratr_copy_string(char *destination, size_t size,
                            const char *source);

/**
 * Remove leading and trailing whitespace in place.
 *
 * Whitespace classification follows the C library `isspace()` function using
 * the process's active C locale. Interior bytes are preserved. NULL is a
 * no-op; the resulting string remains NUL-terminated.
 */
void infiltratr_trim(char *text);

/** Remove trailing CR and LF bytes in place; NULL is a no-op. */
void infiltratr_trim_line_end(char *text);

/**
 * Compare two possibly-NULL strings for equality.
 *
 * Two NULL pointers compare equal. NULL and non-NULL compare unequal.
 */
bool infiltratr_string_equal(const char *left, const char *right);

/**
 * Test whether `text` begins with `prefix`.
 *
 * Either NULL argument returns false. An empty prefix matches every non-NULL
 * string, including the empty string.
 */
bool infiltratr_string_starts_with(const char *text, const char *prefix);

/**
 * Test whether `text` ends with `suffix`.
 *
 * Either NULL argument returns false. An empty suffix matches every non-NULL
 * string, including the empty string.
 */
bool infiltratr_string_ends_with(const char *text, const char *suffix);

/**
 * Parse a complete unsigned integer.
 *
 * Leading and trailing C-library whitespace are accepted. A leading `+` or
 * `-` is rejected. `base` may be 0 for C-style base detection or 2..36;
 * base 1 and values above 36 are invalid. Partial parses and overflow fail.
 *
 * @param text NUL-terminated input string.
 * @param base Numeric base as described above.
 * @param value Receives the parsed value only on success.
 * @return true on a complete in-range parse; false otherwise. On failure,
 *         `*value` is unchanged when `value` is non-NULL.
 */
bool infiltratr_parse_u64(const char *text, unsigned int base,
                          uint64_t *value);

/**
 * Parse a complete signed integer.
 *
 * Leading/trailing C-library whitespace and an optional sign are accepted.
 * `base` follows the same 0 or 2..36 contract as `infiltratr_parse_u64`.
 * Overflow, partial input and invalid arguments fail without changing output.
 */
bool infiltratr_parse_i64(const char *text, unsigned int base,
                          int64_t *value);

/**
 * Parse an unsigned integer and require `minimum <= value <= maximum`.
 *
 * Invalid bounds, parse failure or an out-of-range value return false and
 * leave caller output unchanged.
 */
bool infiltratr_parse_u64_range(const char *text, unsigned int base,
                                uint64_t minimum, uint64_t maximum,
                                uint64_t *value);

/**
 * Parse a signed integer and require `minimum <= value <= maximum`.
 *
 * Invalid bounds, parse failure or an out-of-range value return false and
 * leave caller output unchanged.
 */
bool infiltratr_parse_i64_range(const char *text, unsigned int base,
                                int64_t minimum, int64_t maximum,
                                int64_t *value);

/**
 * Parse one complete finite ASCII decimal value independently of LC_NUMERIC.
 *
 * The grammar accepts ASCII whitespace, an optional sign, decimal digits, an
 * optional full-stop fraction and an optional `e`/`E` decimal exponent. Hex
 * floating syntax, NaN, infinity, trailing junk, overflow and non-zero values
 * that underflow to zero are rejected. Literal signed zero is preserved.
 *
 * @param value Receives the converted double only on success and is otherwise
 *              unchanged.
 */
bool infiltratr_parse_double(const char *text, double *value);

/**
 * Parse a finite decimal and require inclusive caller-selected bounds.
 *
 * NaN bounds, reversed bounds, parse failures and out-of-range values fail
 * without changing caller output.
 */
bool infiltratr_parse_double_range(const char *text, double minimum,
                                   double maximum, double *value);

/**
 * Clamp a value to inclusive bounds.
 *
 * If the value or either bound is NaN, or `lower > upper`, the original value
 * is returned unchanged.
 */
double infiltratr_clamp_double(double value, double lower, double upper);

/**
 * Add two unsigned values while rejecting overflow.
 *
 * @param result Receives the sum only on success.
 * @return false for NULL output or overflow. On overflow, caller output is
 *         unchanged.
 */
bool infiltratr_u64_add_checked(uint64_t left, uint64_t right,
                                uint64_t *result);

/** Add unsigned values, returning `UINT64_MAX` instead of wrapping. */
uint64_t infiltratr_u64_add_saturating(uint64_t left, uint64_t right);

/** Multiply unsigned values, returning `UINT64_MAX` instead of wrapping. */
uint64_t infiltratr_u64_multiply_saturating(uint64_t left, uint64_t right);

/**
 * Return `part / whole` as an inclusive 0..100 percentage.
 *
 * A zero denominator maps to 0. Values above the whole are clamped to 100.
 */
double infiltratr_percent_u64(uint64_t part, uint64_t whole);

/**
 * Convert a monotonic unsigned-counter delta into a finite rate.
 *
 * Counter rollback, negative/non-finite scaling, non-positive/non-finite
 * elapsed time and an unrepresentable result are rejected. For every failure
 * after a valid `rate` pointer is supplied, `*rate` is set to 0.
 */
bool infiltratr_u64_counter_rate(uint64_t current, uint64_t previous,
                                 long double units_per_count,
                                 double elapsed_seconds, double *rate);

/**
 * Scale one finite quantity according to a caller-selected unit policy.
 *
 * This is the common implementation used by byte, capacity, network and
 * related formatters. It preserves the sign of the input and returns both the
 * scaled value and selected unit index, allowing compound displays to share a
 * unit without reimplementing unit-selection logic.
 *
 * `unit_count` must be non-zero and the option range must resolve entirely
 * inside it. `zero_below_minimum_unit` maps magnitudes below the first allowed
 * unit boundary to zero in that minimum unit. On failure, both output values
 * are left unchanged.
 */
bool infiltratr_scale_quantity(long double value,
                               const InfiltratrScaleOptions *options,
                               size_t unit_count, long double *scaled_value,
                               size_t *unit_index);

/**
 * Format one finite scaled quantity using caller-supplied units and suffix.
 *
 * The caller owns all strings and output storage. A valid output buffer is
 * cleared before argument/scaling validation. Formatting uses `snprintf`; if
 * the textual result does not fit, false is returned and the buffer contains
 * the bounded NUL-terminated prefix produced by `snprintf`.
 *
 * @return true only when arguments, scaling and complete formatting succeed.
 */
bool infiltratr_format_scaled_quantity(long double value,
                                        const char *const *units,
                                        size_t unit_count,
                                        const char *suffix,
                                        const InfiltratrScaleOptions *options,
                                        char *buffer, size_t buffer_size);

/**
 * Format bytes with 1024-based traditional B/KB/MB/GB/TB labels.
 *
 * The convenience wrapper always returns `buffer`; callers needing explicit
 * truncation/error reporting should use `infiltratr_format_scaled_quantity`.
 */
char *infiltratr_format_bytes(uint64_t bytes, char *buffer,
                              size_t buffer_size);

/**
 * Format a byte rate with 1024-based traditional labels and `/s`.
 *
 * Negative or non-finite input is normalised to zero. The wrapper always
 * returns `buffer`; use the rich formatter when completion status is required.
 */
char *infiltratr_format_rate(double bytes_per_second, char *buffer,
                             size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif
