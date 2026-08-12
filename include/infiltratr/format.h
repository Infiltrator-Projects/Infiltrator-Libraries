// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file format.h
 * @brief Dependency-free formatting helpers shared by Infiltrator programs.
 *
 * These helpers centralise small presentation-safe conversions that are useful
 * across applications without depending on GLib, GTK or a platform API.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_FORMAT_H
#define INFILTRATR_COMMON_FORMAT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Format bytes as a fixed one-decimal binary GB quantity. */
char *infiltratr_format_memory_gb(uint64_t bytes, char *buffer, size_t size);
/** Format a compact binary capacity using B/KB/MB/GB/TB labels. */
char *infiltratr_format_disk_capacity(uint64_t bytes, char *buffer, size_t size);
/** Format a network quantity using 1024-based byte or bit units. */
char *infiltratr_format_network(long double bytes, bool use_bits,
                                bool per_second, char *buffer, size_t size);
/** Format send and receive byte rates with one shared display unit. */
char *infiltratr_format_network_pair(long double send_bytes,
                                     long double receive_bytes, bool use_bits,
                                     char *buffer, size_t size);
/** Format a negotiated decimal-Mb/s link rate using 1024-based display units. */
char *infiltratr_format_link_speed_mbps(double megabits_per_second,
                                        char *buffer, size_t size);
/** Format an optional percentage, returning N/A when unavailable. */
char *infiltratr_format_percent(bool available, double value,
                                char *buffer, size_t size);
/** Format an optional frequency in MHz, returning N/A when unavailable. */
char *infiltratr_format_mhz(bool available, double value,
                            char *buffer, size_t size);
/** Format an optional Celsius temperature, returning N/A when unavailable. */
char *infiltratr_format_celsius(bool available, double value,
                                char *buffer, size_t size);
/** Format an optional power value in watts, returning N/A when unavailable. */
char *infiltratr_format_watts(bool available, double value,
                              char *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif
