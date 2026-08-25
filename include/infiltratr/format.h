// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file format.h
 * @brief Dependency-free formatting helpers shared by Infiltrator programs.
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
 * ABI version 1 requires `struct_size` to cover every field through
 * `unavailable_text`. Trailing structure padding and future appended fields are
 * not required, permitting compatible extension within the same ABI version.
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

bool infiltratr_format_scalar(bool available, long double value,
                              const InfiltratrScalarFormatOptions *options,
                              char *buffer, size_t size);
char *infiltratr_format_memory_gb(uint64_t bytes, char *buffer, size_t size);
char *infiltratr_format_disk_capacity(uint64_t bytes, char *buffer, size_t size);
char *infiltratr_format_network(long double bytes, bool use_bits,
                                bool per_second, char *buffer, size_t size);
char *infiltratr_format_network_pair(long double send_bytes,
                                     long double receive_bytes, bool use_bits,
                                     char *buffer, size_t size);
char *infiltratr_format_link_speed_mbps(double megabits_per_second,
                                        char *buffer, size_t size);
char *infiltratr_format_percent(bool available, double value,
                                char *buffer, size_t size);
char *infiltratr_format_mhz(bool available, double value,
                            char *buffer, size_t size);
char *infiltratr_format_celsius(bool available, double value,
                                char *buffer, size_t size);
char *infiltratr_format_watts(bool available, double value,
                              char *buffer, size_t size);
char *infiltratr_format_duration_clock(uint64_t seconds,
                                       char *buffer, size_t size);
char *infiltratr_format_duration_compact(bool available, uint64_t seconds,
                                         char *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif
