// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file format.h
 * @brief Dependency-free formatting helpers shared by Infiltrator programs.
 *
 * These helpers centralise presentation-safe conversions that are useful
 * across applications without depending on GLib, GTK or a platform API. The
 * configurable scaling engine is declared by core.h and is available here so
 * callers can use either the rich policy API or the convenience wrappers.
 *
 * The rich `infiltratr_format_scalar` function reports whether the complete
 * representation fitted. Convenience wrappers preserve the historic
 * buffer-returning interface; callers that must distinguish truncation should
 * use the rich formatter or the core scaled-quantity formatter directly.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_FORMAT_H
#define INFILTRATR_COMMON_FORMAT_H

#include "infiltratr/core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INFILTRATR_SCALAR_FORMAT_OPTIONS_ABI 1U

/**
 * Policy for rendering one optional scalar value.
 *
 * `decimal_places` is restricted to 0..9. When `clamp` is true, `minimum` and
 * `maximum` must not be NaN and must satisfy `minimum <= maximum`. NULL prefix
 * or suffix pointers are treated as empty strings; a NULL `unavailable_text`
 * selects the default `N/A` representation.
 *
 * `struct_size` and `abi_version` are validated before the remaining fields are
 * interpreted, following the same ABI-header convention as the core option
 * structures.
 */
typedef struct {
    size_t struct_size;
    uint32_t abi_version;
    unsigned int decimal_places;
    bool clamp;
    long double minimum;
    long double maximum;
    const char *prefix;
    const char *suffix;
    const char *unavailable_text;
} InfiltratrScalarFormatOptions;

/** Default scalar policy: one decimal place, no clamping, and `N/A` if unavailable. */
#define INFILTRATR_SCALAR_FORMAT_OPTIONS_INIT \
    { .struct_size = sizeof(InfiltratrScalarFormatOptions), \
      .abi_version = INFILTRATR_SCALAR_FORMAT_OPTIONS_ABI, \
      .decimal_places = 1U, \
      .clamp = false, \
      .minimum = 0.0L, \
      .maximum = 0.0L, \
      .prefix = "", \
      .suffix = "", \
      .unavailable_text = "N/A" }

/**
 * Format an optional finite scalar with selected precision, affixes and bounds.
 *
 * A valid non-empty output buffer is cleared before option validation. When
 * `available` is false or `value` is non-finite, the unavailable text is
 * rendered instead of the numeric value. If clamping is enabled, values below
 * or above the configured bounds are clamped before formatting.
 *
 * @param available Whether the measurement is logically available.
 * @param value Scalar value to render when available and finite.
 * @param options Complete ABI-compatible formatting policy.
 * @param buffer Caller-owned output storage.
 * @param size Size of `buffer` in bytes, including the terminator.
 * @return true only when the complete selected representation fits. false is
 *         returned for invalid arguments/options or truncation. `snprintf`
 *         truncation leaves a bounded NUL-terminated prefix in the buffer.
 */
bool infiltratr_format_scalar(bool available, long double value,
                              const InfiltratrScalarFormatOptions *options,
                              char *buffer, size_t size);

/**
 * Format bytes as a fixed one-decimal binary GB quantity.
 *
 * This is a convenience wrapper and always returns `buffer`; it does not expose
 * the rich formatter's completion status.
 */
char *infiltratr_format_memory_gb(uint64_t bytes, char *buffer, size_t size);

/** Format a compact binary capacity using B/KB/MB/GB/TB labels. */
char *infiltratr_format_disk_capacity(uint64_t bytes, char *buffer, size_t size);

/**
 * Format a non-negative network quantity using 1024-based display units.
 *
 * `bytes` is the input quantity even when `use_bits` is true; bit mode first
 * multiplies it by eight. Non-finite or negative input is normalised to zero.
 * Display scaling starts at KB/Kb rather than B/b, and values below that unit
 * render as zero in the minimum display unit. `/s` is appended when
 * `per_second` is true.
 */
char *infiltratr_format_network(long double bytes, bool use_bits,
                                bool per_second, char *buffer, size_t size);

/**
 * Format send and receive byte rates with one shared display unit.
 *
 * The larger of the two non-negative rates selects the common unit so values
 * remain directly comparable. Bit mode converts both byte rates by a factor of
 * eight. The historic presentation is `S:<value> R:<value> <unit>/s`.
 */
char *infiltratr_format_network_pair(long double send_bytes,
                                     long double receive_bytes, bool use_bits,
                                     char *buffer, size_t size);

/**
 * Format a negotiated link rate supplied in decimal megabits per second.
 *
 * Positive input is converted to bits per second using 1,000,000 and then
 * displayed with the library's 1024-based Kb/Mb/Gb/Tb scaling policy. Zero,
 * negative and non-finite input render as `N/A`.
 */
char *infiltratr_format_link_speed_mbps(double megabits_per_second,
                                        char *buffer, size_t size);

/**
 * Format an optional percentage as an integer followed by `%`.
 *
 * Available finite values are clamped to 0..100; unavailable or non-finite
 * values render as `N/A`.
 */
char *infiltratr_format_percent(bool available, double value,
                                char *buffer, size_t size);

/** Format an optional finite frequency as integer MHz, otherwise `N/A`. */
char *infiltratr_format_mhz(bool available, double value,
                            char *buffer, size_t size);

/** Format an optional finite temperature as integer degrees Celsius, otherwise `N/A`. */
char *infiltratr_format_celsius(bool available, double value,
                                char *buffer, size_t size);

/** Format an optional finite power value with one decimal place in watts, otherwise `N/A`. */
char *infiltratr_format_watts(bool available, double value,
                              char *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif
