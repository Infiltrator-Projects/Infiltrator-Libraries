// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file format.c
 * @brief Dependency-free shared metric formatting implementation.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/format.h"
#include "infiltratr/core.h"

#include <math.h>
#include <stdio.h>

static char *unavailable(char *buffer, size_t size)
{
    if (buffer && size > 0U) (void)snprintf(buffer, size, "N/A");
    return buffer;
}

static InfiltratrScaleOptions fixed_scale(size_t unit, unsigned int decimals)
{
    InfiltratrScaleOptions options = INFILTRATR_SCALE_OPTIONS_INIT;
    options.minimum_unit = unit;
    options.maximum_unit = unit;
    options.decimal_places = decimals;
    options.integer_threshold = 0.0L;
    options.integer_at_minimum_unit = false;
    return options;
}

char *infiltratr_format_memory_gb(uint64_t bytes, char *buffer, size_t size)
{
    static const char *const units[] = {"B", "KB", "MB", "GB", "TB"};
    const InfiltratrScaleOptions options = fixed_scale(3U, 1U);
    (void)infiltratr_format_scaled_quantity((long double)bytes, units,
                                             INFILTRATR_ARRAY_LENGTH(units),
                                             "", &options, buffer, size);
    return buffer;
}

char *infiltratr_format_disk_capacity(uint64_t bytes, char *buffer, size_t size)
{
    return infiltratr_format_bytes(bytes, buffer, size);
}

char *infiltratr_format_network(long double bytes, bool use_bits,
                                bool per_second, char *buffer, size_t size)
{
    static const char *const byte_units[] = {"B", "KB", "MB", "GB", "TB"};
    static const char *const bit_units[] = {"b", "Kb", "Mb", "Gb", "Tb"};
    if (!buffer || size == 0U) return buffer;
    if (!isfinite(bytes) || bytes < 0.0L) bytes = 0.0L;

    InfiltratrScaleOptions options = INFILTRATR_SCALE_OPTIONS_INIT;
    options.minimum_unit = 1U;
    options.decimal_places = 1U;
    options.integer_threshold = 0.0L;
    options.integer_at_minimum_unit = false;
    options.zero_below_minimum_unit = true;

    const long double value = use_bits ? bytes * 8.0L : bytes;
    const char *const *units = use_bits ? bit_units : byte_units;
    (void)infiltratr_format_scaled_quantity(value, units,
                                             INFILTRATR_ARRAY_LENGTH(byte_units),
                                             per_second ? "/s" : "", &options,
                                             buffer, size);
    return buffer;
}

char *infiltratr_format_network_pair(long double send_bytes,
                                     long double receive_bytes, bool use_bits,
                                     char *buffer, size_t size)
{
    static const char *const byte_units[] = {"B", "KB", "MB", "GB", "TB"};
    static const char *const bit_units[] = {"b", "Kb", "Mb", "Gb", "Tb"};
    if (!buffer || size == 0U) return buffer;
    if (!isfinite(send_bytes) || send_bytes < 0.0L) send_bytes = 0.0L;
    if (!isfinite(receive_bytes) || receive_bytes < 0.0L) receive_bytes = 0.0L;

    long double send = use_bits ? send_bytes * 8.0L : send_bytes;
    long double receive = use_bits ? receive_bytes * 8.0L : receive_bytes;
    const long double maximum = fmaxl(send, receive);

    InfiltratrScaleOptions options = INFILTRATR_SCALE_OPTIONS_INIT;
    options.minimum_unit = 1U;
    options.decimal_places = 1U;
    options.integer_threshold = 0.0L;
    options.integer_at_minimum_unit = false;
    options.zero_below_minimum_unit = true;

    long double ignored = 0.0L;
    size_t unit = 1U;
    if (!infiltratr_scale_quantity(maximum, &options,
                                   INFILTRATR_ARRAY_LENGTH(byte_units),
                                   &ignored, &unit)) {
        buffer[0] = '\0';
        return buffer;
    }

    options.minimum_unit = unit;
    options.maximum_unit = unit;
    options.zero_below_minimum_unit = false;
    long double scaled_send = 0.0L;
    long double scaled_receive = 0.0L;
    size_t selected_unit = unit;
    (void)infiltratr_scale_quantity(send, &options,
                                    INFILTRATR_ARRAY_LENGTH(byte_units),
                                    &scaled_send, &selected_unit);
    (void)infiltratr_scale_quantity(receive, &options,
                                    INFILTRATR_ARRAY_LENGTH(byte_units),
                                    &scaled_receive, &selected_unit);

    const char *const *units = use_bits ? bit_units : byte_units;
    (void)snprintf(buffer, size, "S:%.1Lf R:%.1Lf %s/s", scaled_send,
                   scaled_receive, units[unit]);
    return buffer;
}

char *infiltratr_format_link_speed_mbps(double megabits_per_second,
                                        char *buffer, size_t size)
{
    static const char *const units[] = {"b/s", "Kb/s", "Mb/s", "Gb/s", "Tb/s"};
    if (!buffer || size == 0U) return buffer;
    if (!isfinite(megabits_per_second) || megabits_per_second <= 0.0)
        return unavailable(buffer, size);

    InfiltratrScaleOptions options = INFILTRATR_SCALE_OPTIONS_INIT;
    options.minimum_unit = 1U;
    options.decimal_places = 2U;
    options.integer_threshold = 0.0L;
    options.integer_at_minimum_unit = false;

    const long double bits_per_second =
        (long double)megabits_per_second * 1000000.0L;
    (void)infiltratr_format_scaled_quantity(bits_per_second, units,
                                             INFILTRATR_ARRAY_LENGTH(units),
                                             "", &options, buffer, size);
    return buffer;
}

char *infiltratr_format_percent(bool available, double value,
                                char *buffer, size_t size)
{
    if (!available || !isfinite(value)) return unavailable(buffer, size);
    value = fmin(100.0, fmax(0.0, value));
    (void)snprintf(buffer, size, "%.0f%%", value);
    return buffer;
}

char *infiltratr_format_mhz(bool available, double value,
                            char *buffer, size_t size)
{
    if (!available || !isfinite(value)) return unavailable(buffer, size);
    (void)snprintf(buffer, size, "%.0f MHz", value);
    return buffer;
}

char *infiltratr_format_celsius(bool available, double value,
                                char *buffer, size_t size)
{
    if (!available || !isfinite(value)) return unavailable(buffer, size);
    (void)snprintf(buffer, size, "%.0f °C", value);
    return buffer;
}

char *infiltratr_format_watts(bool available, double value,
                              char *buffer, size_t size)
{
    if (!available || !isfinite(value)) return unavailable(buffer, size);
    (void)snprintf(buffer, size, "%.1f W", value);
    return buffer;
}
