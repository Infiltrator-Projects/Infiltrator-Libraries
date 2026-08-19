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

#define INFILTRATR_COMMON_VERSION "1.8.0"
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

/** Validate a project identity record. */
bool infiltratr_project_info_is_valid(const InfiltratrProjectInfo *info);

/** Write stable machine-readable project metadata as `key=value` lines. */
int infiltratr_project_info_print(FILE *stream,
                                  const InfiltratrProjectInfo *info);

/** Copy a possibly-NULL string into bounded caller storage. */
void infiltratr_copy_string(char *destination, size_t size,
                            const char *source);

/** Remove leading and trailing whitespace in place. */
void infiltratr_trim(char *text);

/** Remove trailing CR and LF bytes in place; NULL is a no-op. */
void infiltratr_trim_line_end(char *text);

/** Compare two possibly-NULL strings for equality. */
bool infiltratr_string_equal(const char *left, const char *right);

/** Test whether `text` begins with `prefix`. */
bool infiltratr_string_starts_with(const char *text, const char *prefix);

/** Test whether `text` ends with `suffix`. */
bool infiltratr_string_ends_with(const char *text, const char *suffix);

/** Parse a complete unsigned integer. */
bool infiltratr_parse_u64(const char *text, unsigned int base,
                          uint64_t *value);

/** Parse a complete signed integer. */
bool infiltratr_parse_i64(const char *text, unsigned int base,
                          int64_t *value);

/** Parse an unsigned integer and require `minimum <= value <= maximum`. */
bool infiltratr_parse_u64_range(const char *text, unsigned int base,
                                uint64_t minimum, uint64_t maximum,
                                uint64_t *value);

/** Parse a signed integer and require `minimum <= value <= maximum`. */
bool infiltratr_parse_i64_range(const char *text, unsigned int base,
                                int64_t minimum, int64_t maximum,
                                int64_t *value);

/** Parse one complete finite ASCII decimal value independently of LC_NUMERIC. */
bool infiltratr_parse_double(const char *text, double *value);

/** Parse a finite decimal and require inclusive caller-selected bounds. */
bool infiltratr_parse_double_range(const char *text, double minimum,
                                   double maximum, double *value);

/** Clamp a value to inclusive bounds. */
double infiltratr_clamp_double(double value, double lower, double upper);

/** Add two unsigned values while rejecting overflow. */
bool infiltratr_u64_add_checked(uint64_t left, uint64_t right,
                                uint64_t *result);

/** Add unsigned values, returning `UINT64_MAX` instead of wrapping. */
uint64_t infiltratr_u64_add_saturating(uint64_t left, uint64_t right);

/** Multiply unsigned values, returning `UINT64_MAX` instead of wrapping. */
uint64_t infiltratr_u64_multiply_saturating(uint64_t left, uint64_t right);

/** Return `part / whole` as an inclusive 0..100 percentage. */
double infiltratr_percent_u64(uint64_t part, uint64_t whole);

/** Convert a monotonic unsigned-counter delta into a finite rate. */
bool infiltratr_u64_counter_rate(uint64_t current, uint64_t previous,
                                 long double units_per_count,
                                 double elapsed_seconds, double *rate);

/** Scale one finite quantity according to a caller-selected unit policy. */
bool infiltratr_scale_quantity(long double value,
                               const InfiltratrScaleOptions *options,
                               size_t unit_count, long double *scaled_value,
                               size_t *unit_index);

/** Format one finite scaled quantity using caller-supplied units and suffix. */
bool infiltratr_format_scaled_quantity(long double value,
                                        const char *const *units,
                                        size_t unit_count,
                                        const char *suffix,
                                        const InfiltratrScaleOptions *options,
                                        char *buffer, size_t buffer_size);

/** Format bytes with 1024-based traditional B/KB/MB/GB/TB labels. */
char *infiltratr_format_bytes(uint64_t bytes, char *buffer,
                              size_t buffer_size);

/** Format a byte rate with 1024-based traditional labels and `/s`. */
char *infiltratr_format_rate(double bytes_per_second, char *buffer,
                             size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif
