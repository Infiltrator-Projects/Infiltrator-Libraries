// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 1993-2026 Shannon Smith
 *
 * Portable formatting for the complete system-wide clock catalogue.
 *
 * Canonical instants remain Unix microseconds and internal elapsed-time
 * accounting remains SI/monotonic. This module is presentation only: it turns
 * one canonical instant plus explicit civil offset/location policy into the
 * clock representation selected by System Settings.
 */
#include "infiltratr/temporal.h"

#include "infiltratr/arithmetic.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MICROSECONDS_PER_SECOND INT64_C(1000000)
#define SECONDS_PER_MINUTE INT64_C(60)
#define MINUTES_PER_HOUR INT64_C(60)
#define SECONDS_PER_HOUR INT64_C(3600)
#define SECONDS_PER_DAY INT64_C(86400)
#define MICROSECONDS_PER_DAY (SECONDS_PER_DAY * MICROSECONDS_PER_SECOND)
#define INTERNET_TICKS_PER_DAY UINT64_C(1000)
#define HEX_TICKS_PER_DAY UINT64_C(65536)
#define JULIAN_DATE_UNIX_EPOCH 2440587.5L
#define JULIAN_DATE_J2000 2451545.0L
#define INFILTRATR_PI 3.141592653589793238462643383279502884L

static bool write_text(char *buffer, size_t capacity, size_t *length,
                       const char *text)
{
    size_t needed;

    if (buffer == NULL || capacity == 0U || text == NULL) {
        return false;
    }
    needed = strlen(text);
    if (needed >= capacity) {
        return false;
    }
    memcpy(buffer, text, needed + 1U);
    if (length != NULL) {
        *length = needed;
    }
    return true;
}

static bool write_printf(char *buffer, size_t capacity, size_t *length,
                         const char *format, ...)
{
    va_list arguments;
    int written;

    if (buffer == NULL || capacity == 0U || format == NULL) {
        return false;
    }
    va_start(arguments, format);
    written = vsnprintf(buffer, capacity, format, arguments);
    va_end(arguments);
    if (written < 0 || (size_t)written >= capacity) {
        buffer[0] = '\0';
        return false;
    }
    if (length != NULL) {
        *length = (size_t)written;
    }
    return true;
}

static int64_t floor_divide(int64_t value, int64_t divisor)
{
    int64_t quotient = 0;

    if (!infiltratr_i64_floor_divmod(value, divisor, &quotient, NULL)) {
        return 0;
    }
    return quotient;
}

static int64_t positive_modulo(int64_t value, int64_t modulus)
{
    int64_t remainder = 0;

    if (!infiltratr_i64_floor_divmod(value, modulus, NULL, &remainder)) {
        return 0;
    }
    return remainder;
}

static uint64_t day_tick(int64_t microseconds, uint64_t ticks_per_day)
{
    uint64_t tick = 0U;

    if (microseconds < 0 || ticks_per_day == 0U ||
        !infiltratr_cycle_partition_u64((uint64_t)microseconds,
                                        (uint64_t)MICROSECONDS_PER_DAY,
                                        ticks_per_day, &tick, NULL)) {
        return 0U;
    }
    return tick;
}

static void split_clock_seconds(int64_t whole_seconds,
                                int *hour, int *minute, int *second)
{
    const int64_t normalised =
        positive_modulo(whole_seconds, SECONDS_PER_DAY);

    *hour = (int)(normalised / SECONDS_PER_HOUR);
    *minute = (int)((normalised / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR);
    *second = (int)(normalised % SECONDS_PER_MINUTE);
}

static bool write_clock_fields(char *buffer, size_t capacity, size_t *length,
                               int hour, int minute, int second,
                               bool show_seconds, bool vertical,
                               const char *suffix)
{
    const char *separator = vertical ? "\n" : ":";
    const char *suffix_separator = vertical ? "\n" : " ";

    if (show_seconds) {
        return write_printf(buffer, capacity, length,
                            "%02d%s%02d%s%02d%s%s",
                            hour, separator, minute, separator, second,
                            suffix_separator, suffix);
    }
    return write_printf(buffer, capacity, length,
                        "%02d%s%02d%s%s",
                        hour, separator, minute, suffix_separator, suffix);
}

/*
 * Convert a Unix day number to Gregorian day-of-year with exact 400-year
 * cycle arithmetic. Keeping this independent of time_t makes astronomical
 * formatting deterministic on 32-bit and 64-bit hosts alike.
 */
static bool gregorian_day_of_year(int64_t unix_microseconds,
                                  int *day_of_year)
{
    static const int cumulative_days[12] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
    };
    int64_t unix_seconds;
    int64_t days;
    int64_t z;
    int64_t era;
    int64_t doe;
    int64_t yoe;
    int64_t doy_march;
    int64_t mp;
    int64_t year;
    int month;
    int day;
    bool leap;

    if (day_of_year == NULL) {
        return false;
    }
    unix_seconds = floor_divide(unix_microseconds, MICROSECONDS_PER_SECOND);
    days = floor_divide(unix_seconds, SECONDS_PER_DAY);
    if (!infiltratr_i64_add_checked(days, INT64_C(719468), &z)) {
        return false;
    }

    era = floor_divide(z, INT64_C(146097));
    doe = z - era * INT64_C(146097);
    yoe = (doe - doe / INT64_C(1460) + doe / INT64_C(36524) -
           doe / INT64_C(146096)) / INT64_C(365);
    year = yoe + era * INT64_C(400);
    doy_march = doe - (INT64_C(365) * yoe + yoe / INT64_C(4) -
                       yoe / INT64_C(100));
    mp = (INT64_C(5) * doy_march + INT64_C(2)) / INT64_C(153);
    day = (int)(doy_march - (INT64_C(153) * mp + INT64_C(2)) /
                INT64_C(5) + INT64_C(1));
    month = (int)(mp + (mp < 10 ? 3 : -9));
    if (month <= 2) {
        year++;
    }

    leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    if (month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }
    *day_of_year = cumulative_days[month - 1] + day +
                   (leap && month > 2 ? 1 : 0);
    return true;
}

/*
 * Compact equation-of-time and solar-declination approximation shared by the
 * apparent-solar and seasonal clock families. Both values come from the same
 * fractional-year phase so all location-aware modes remain internally
 * consistent.
 */
static bool solar_terms(int64_t unix_microseconds,
                        double *equation_minutes,
                        double *declination_radians)
{
    const int64_t utc_phase =
        positive_modulo(unix_microseconds, MICROSECONDS_PER_DAY);
    const long double fractional_hour =
        (long double)utc_phase /
        ((long double)SECONDS_PER_HOUR * MICROSECONDS_PER_SECOND);
    int day_of_year;
    long double gamma;

    if (!gregorian_day_of_year(unix_microseconds, &day_of_year)) {
        return false;
    }
    gamma = 2.0L * INFILTRATR_PI / 365.0L *
            ((long double)day_of_year - 1.0L +
             (fractional_hour - 12.0L) / 24.0L);

    if (equation_minutes != NULL) {
        *equation_minutes = 229.18 *
            (0.000075 + 0.001868 * cos((double)gamma) -
             0.032077 * sin((double)gamma) -
             0.014615 * cos((double)(2.0L * gamma)) -
             0.040849 * sin((double)(2.0L * gamma)));
    }
    if (declination_radians != NULL) {
        *declination_radians =
            0.006918 - 0.399912 * cos((double)gamma) +
            0.070257 * sin((double)gamma) -
            0.006758 * cos((double)(2.0L * gamma)) +
            0.000907 * sin((double)(2.0L * gamma)) -
            0.002697 * cos((double)(3.0L * gamma)) +
            0.001480 * sin((double)(3.0L * gamma));
    }
    return true;
}

static long double julian_date(int64_t unix_microseconds)
{
    return JULIAN_DATE_UNIX_EPOCH +
           (long double)unix_microseconds / (long double)MICROSECONDS_PER_DAY;
}

static double clamp_longitude(double longitude)
{
    return isfinite(longitude)
        ? infiltratr_clamp_double(longitude, -180.0, 180.0) : 0.0;
}

static double clamp_latitude(double latitude)
{
    return isfinite(latitude)
        ? infiltratr_clamp_double(latitude, -90.0, 90.0) : 0.0;
}

static long double local_sidereal_seconds(int64_t unix_microseconds,
                                          double longitude)
{
    const long double days_since_j2000 =
        julian_date(unix_microseconds) - JULIAN_DATE_J2000;
    const long double gmst_hours =
        18.697374558L + 24.06570982441908L * days_since_j2000;
    long double local_hours =
        fmodl(gmst_hours + (long double)clamp_longitude(longitude) / 15.0L,
              24.0L);

    if (local_hours < 0.0L) {
        local_hours += 24.0L;
    }
    return local_hours * (long double)SECONDS_PER_HOUR;
}

static long double mean_solar_seconds(int64_t unix_microseconds,
                                      double longitude)
{
    const int64_t utc_microseconds =
        positive_modulo(unix_microseconds, MICROSECONDS_PER_DAY);
    long double seconds =
        (long double)utc_microseconds / MICROSECONDS_PER_SECOND +
        (long double)clamp_longitude(longitude) * 240.0L;

    seconds = fmodl(seconds, (long double)SECONDS_PER_DAY);
    if (seconds < 0.0L) {
        seconds += (long double)SECONDS_PER_DAY;
    }
    return seconds;
}

static long double apparent_solar_seconds(int64_t unix_microseconds,
                                          double longitude)
{
    double equation_minutes = 0.0;
    long double seconds;

    if (!solar_terms(unix_microseconds, &equation_minutes, NULL)) {
        return mean_solar_seconds(unix_microseconds, longitude);
    }
    seconds = mean_solar_seconds(unix_microseconds, longitude) +
              (long double)equation_minutes * 60.0L;
    seconds = fmodl(seconds, (long double)SECONDS_PER_DAY);
    if (seconds < 0.0L) {
        seconds += (long double)SECONDS_PER_DAY;
    }
    return seconds;
}

static bool solar_day_boundaries(int64_t unix_microseconds,
                                 double latitude,
                                 double solar_depression_degrees,
                                 long double *dawn_seconds,
                                 long double *dusk_seconds)
{
    const double safe_latitude = clamp_latitude(latitude);
    double declination = 0.0;
    double latitude_radians;
    double zenith_radians;
    double cosine_hour_angle;
    long double hour_angle;
    long double half_day_seconds;

    if (dawn_seconds == NULL || dusk_seconds == NULL ||
        !isfinite(solar_depression_degrees) ||
        solar_depression_degrees < 0.0 ||
        solar_depression_degrees >= 90.0 ||
        !solar_terms(unix_microseconds, NULL, &declination)) {
        return false;
    }

    latitude_radians = safe_latitude * (double)INFILTRATR_PI / 180.0;
    zenith_radians =
        (90.0 + solar_depression_degrees) * (double)INFILTRATR_PI / 180.0;
    if (fabs(cos(latitude_radians) * cos(declination)) < 1.0e-12) {
        return false;
    }

    cosine_hour_angle =
        cos(zenith_radians) /
            (cos(latitude_radians) * cos(declination)) -
        tan(latitude_radians) * tan(declination);
    if (cosine_hour_angle < -1.0 || cosine_hour_angle > 1.0) {
        return false;
    }

    hour_angle = acosl((long double)infiltratr_clamp_double(
        cosine_hour_angle, -1.0, 1.0));
    half_day_seconds =
        hour_angle * (long double)SECONDS_PER_DAY / (2.0L * INFILTRATR_PI);
    *dawn_seconds = (long double)SECONDS_PER_DAY / 2.0L - half_day_seconds;
    *dusk_seconds = (long double)SECONDS_PER_DAY / 2.0L + half_day_seconds;
    return true;
}

static bool solar_boundary_instants(int64_t unix_microseconds,
                                    double latitude,
                                    double longitude,
                                    double solar_depression_degrees,
                                    int64_t *dawn_microseconds,
                                    int64_t *dusk_microseconds)
{
    const int64_t safety_margin = INT64_C(2) * MICROSECONDS_PER_DAY;
    const double safe_latitude = clamp_latitude(latitude);
    const double safe_longitude = clamp_longitude(longitude);
    int64_t day_index;
    int64_t day_start;
    int64_t noon;
    double equation_minutes = 0.0;
    double declination = 0.0;
    double latitude_radians;
    double zenith_radians;
    double cosine_hour_angle;
    long double hour_angle;
    long double solar_noon_minutes;
    long double hour_angle_minutes;
    long double dawn_value;
    long double dusk_value;

    if (dawn_microseconds == NULL || dusk_microseconds == NULL ||
        unix_microseconds < INT64_MIN + safety_margin ||
        unix_microseconds > INT64_MAX - safety_margin ||
        !isfinite(solar_depression_degrees) ||
        solar_depression_degrees < 0.0 ||
        solar_depression_degrees >= 90.0) {
        return false;
    }

    day_index = floor_divide(unix_microseconds, MICROSECONDS_PER_DAY);
    if (!infiltratr_i64_multiply_checked(
            day_index, MICROSECONDS_PER_DAY, &day_start) ||
        !infiltratr_i64_add_checked(
            day_start, MICROSECONDS_PER_DAY / 2, &noon) ||
        !solar_terms(noon, &equation_minutes, &declination)) {
        return false;
    }

    latitude_radians = safe_latitude * (double)INFILTRATR_PI / 180.0;
    zenith_radians =
        (90.0 + solar_depression_degrees) * (double)INFILTRATR_PI / 180.0;
    if (fabs(cos(latitude_radians) * cos(declination)) < 1.0e-12) {
        return false;
    }

    cosine_hour_angle =
        cos(zenith_radians) /
            (cos(latitude_radians) * cos(declination)) -
        tan(latitude_radians) * tan(declination);
    if (cosine_hour_angle < -1.0 || cosine_hour_angle > 1.0) {
        return false;
    }

    hour_angle = acosl((long double)infiltratr_clamp_double(
        cosine_hour_angle, -1.0, 1.0));
    solar_noon_minutes =
        720.0L - 4.0L * safe_longitude - equation_minutes;
    hour_angle_minutes =
        hour_angle * 180.0L / INFILTRATR_PI * 4.0L;
    dawn_value =
        (long double)day_start +
        (solar_noon_minutes - hour_angle_minutes) *
            60.0L * MICROSECONDS_PER_SECOND;
    dusk_value =
        (long double)day_start +
        (solar_noon_minutes + hour_angle_minutes) *
            60.0L * MICROSECONDS_PER_SECOND;

    if (dawn_value < (long double)INT64_MIN ||
        dawn_value > (long double)INT64_MAX ||
        dusk_value < (long double)INT64_MIN ||
        dusk_value > (long double)INT64_MAX) {
        return false;
    }
    *dawn_microseconds = (int64_t)llroundl(dawn_value);
    *dusk_microseconds = (int64_t)llroundl(dusk_value);
    return true;
}

typedef struct SeasonalPeriod {
    bool daylight;
    unsigned index;
} SeasonalPeriod;

static bool seasonal_period_at(int64_t unix_microseconds,
                               double latitude,
                               double longitude,
                               double solar_depression_degrees,
                               unsigned daylight_parts,
                               unsigned night_parts,
                               SeasonalPeriod *period)
{
    long double dawn;
    long double dusk;
    const long double solar =
        apparent_solar_seconds(unix_microseconds, longitude);
    long double span;
    long double position;
    long double unit;
    unsigned parts;

    if (period == NULL || daylight_parts == 0U || night_parts == 0U ||
        !solar_day_boundaries(unix_microseconds, latitude,
                              solar_depression_degrees, &dawn, &dusk)) {
        return false;
    }

    if (solar >= dawn && solar < dusk) {
        period->daylight = true;
        span = dusk - dawn;
        position = solar - dawn;
        parts = daylight_parts;
    } else {
        period->daylight = false;
        span = (long double)SECONDS_PER_DAY - (dusk - dawn);
        position = solar >= dusk
            ? solar - dusk
            : (long double)SECONDS_PER_DAY - dusk + solar;
        parts = night_parts;
    }

    unit = span / parts;
    period->index = (unsigned)floorl(position / unit);
    if (period->index >= parts) {
        period->index = parts - 1U;
    }
    return true;
}

typedef enum SolarOrigin {
    SOLAR_ORIGIN_SUNRISE,
    SOLAR_ORIGIN_SUNSET
} SolarOrigin;

static bool solar_origin_window(int64_t unix_microseconds,
                                double latitude,
                                double longitude,
                                SolarOrigin origin,
                                int64_t *previous)
{
    int64_t best_previous = INT64_MIN;
    int offset;

    if (previous == NULL) {
        return false;
    }

    for (offset = -2; offset <= 2; ++offset) {
        int64_t delta;
        int64_t sample;
        int64_t dawn;
        int64_t dusk;
        int64_t candidate;

        if (!infiltratr_i64_multiply_checked(
                (int64_t)offset, MICROSECONDS_PER_DAY, &delta) ||
            !infiltratr_i64_add_checked(
                unix_microseconds, delta, &sample) ||
            !solar_boundary_instants(
                sample, latitude, longitude, 0.833, &dawn, &dusk)) {
            continue;
        }

        candidate = origin == SOLAR_ORIGIN_SUNRISE ? dawn : dusk;
        if (candidate <= unix_microseconds && candidate > best_previous) {
            best_previous = candidate;
        }
    }

    if (best_previous == INT64_MIN) {
        return false;
    }
    *previous = best_previous;
    return true;
}

static bool format_equal_hours(char *buffer, size_t capacity, size_t *length,
                               int64_t unix_microseconds, bool show_seconds,
                               bool vertical, double latitude, double longitude,
                               SolarOrigin origin, const char *suffix)
{
    int64_t start;
    int64_t elapsed_microseconds;
    int64_t whole_seconds;
    int hour;
    int minute;
    int second;
    const char *separator = vertical ? "\n" : ":";

    if (!solar_origin_window(
            unix_microseconds, latitude, longitude, origin, &start)) {
        return write_printf(buffer, capacity, length,
                            vertical ? "N/A\n%s" : "N/A %s", suffix);
    }

    elapsed_microseconds = unix_microseconds - start;
    whole_seconds =
        floor_divide(elapsed_microseconds, MICROSECONDS_PER_SECOND);
    hour = (int)(whole_seconds / SECONDS_PER_HOUR);
    minute =
        (int)((whole_seconds / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR);
    second = (int)(whole_seconds % SECONDS_PER_MINUTE);

    if (show_seconds) {
        return write_printf(buffer, capacity, length,
                            "%02d%s%02d%s%02d%s%s",
                            hour, separator, minute, separator, second,
                            vertical ? "\n" : " ", suffix);
    }
    return write_printf(buffer, capacity, length,
                        "%02d%s%02d%s%s",
                        hour, separator, minute,
                        vertical ? "\n" : " ", suffix);
}

static bool mode_location_valid(const InfiltratrTemporalClockModeInfo *info,
                                bool location_configured,
                                double latitude,
                                double longitude)
{
    if (info == NULL) {
        return false;
    }
    if ((info->requires_latitude || info->requires_longitude) &&
        !location_configured) {
        return false;
    }
    return (!info->requires_latitude || isfinite(latitude)) &&
           (!info->requires_longitude || isfinite(longitude));
}

bool infiltratr_temporal_format_clock_mode(const char *mode,
                                           int64_t unix_microseconds,
                                           int32_t utc_offset_seconds,
                                           bool show_seconds,
                                           bool vertical,
                                           bool location_configured,
                                           double latitude,
                                           double longitude,
                                           char *buffer,
                                           size_t capacity,
                                           size_t *length)
{
    const InfiltratrTemporalClockModeInfo *info;
    const int64_t local =
        infiltratr_temporal_local_microseconds_of_day(
            unix_microseconds, utc_offset_seconds);
    const char *separator = vertical ? "\n" : ":";
    char temporary[192];

    if (buffer == NULL || capacity == 0U || mode == NULL) {
        return false;
    }
    buffer[0] = '\0';

    info = infiltratr_temporal_clock_mode_find(mode);
    if (info == NULL ||
        !mode_location_valid(
            info, location_configured, latitude, longitude)) {
        return false;
    }

    if (strcmp(mode, "standard") == 0) {
        return false;
    }

    if (strcmp(mode, "standard-24") == 0 ||
        strcmp(mode, "standard-12") == 0 ||
        strcmp(mode, "decimal") == 0) {
        const InfiltratrClockProfile profile =
            strcmp(mode, "standard-24") == 0
                ? INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_24
                : strcmp(mode, "standard-12") == 0
                    ? INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_12
                    : INFILTRATR_CLOCK_PROFILE_DECIMAL_10;
        char *cursor;

        if (!infiltratr_temporal_format_clock(
                profile, unix_microseconds, utc_offset_seconds,
                show_seconds, temporary, sizeof(temporary), NULL)) {
            return false;
        }
        if (vertical) {
            for (cursor = temporary; *cursor != '\0'; ++cursor) {
                if (*cursor == ':') {
                    *cursor = '\n';
                }
            }
        }
        return write_text(buffer, capacity, length, temporary);
    }

    if (strcmp(mode, "internet") == 0) {
        const int64_t phase = positive_modulo(
            positive_modulo(unix_microseconds, MICROSECONDS_PER_DAY) +
                SECONDS_PER_HOUR * MICROSECONDS_PER_SECOND,
            MICROSECONDS_PER_DAY);
        const uint64_t ticks = day_tick(
            phase, show_seconds
                ? INTERNET_TICKS_PER_DAY * UINT64_C(100)
                : INTERNET_TICKS_PER_DAY);

        return show_seconds
            ? write_printf(buffer, capacity, length, "@%03llu.%02llu",
                           (unsigned long long)(ticks / UINT64_C(100)),
                           (unsigned long long)(ticks % UINT64_C(100)))
            : write_printf(buffer, capacity, length, "@%03llu",
                           (unsigned long long)ticks);
    }

    if (strcmp(mode, "unix") == 0) {
        return write_printf(
            buffer, capacity, length, "%lld",
            (long long)floor_divide(
                unix_microseconds, MICROSECONDS_PER_SECOND));
    }

    if (strcmp(mode, "hexadecimal") == 0) {
        return write_printf(
            buffer, capacity, length, "%04llX",
            (unsigned long long)day_tick(local, HEX_TICKS_PER_DAY));
    }

    if (strcmp(mode, "binary") == 0) {
        int hour;
        int minute;
        int second;
        char hour_bits[6];
        char minute_bits[7];
        char second_bits[7];
        unsigned bit;

        split_clock_seconds(
            local / MICROSECONDS_PER_SECOND, &hour, &minute, &second);
        for (bit = 0U; bit < 5U; ++bit) {
            hour_bits[bit] =
                ((unsigned)hour & (1U << (4U - bit))) ? '1' : '0';
        }
        hour_bits[5] = '\0';
        for (bit = 0U; bit < 6U; ++bit) {
            minute_bits[bit] =
                ((unsigned)minute & (1U << (5U - bit))) ? '1' : '0';
            second_bits[bit] =
                ((unsigned)second & (1U << (5U - bit))) ? '1' : '0';
        }
        minute_bits[6] = '\0';
        second_bits[6] = '\0';

        return show_seconds
            ? write_printf(buffer, capacity, length, "%s%s%s%s%s",
                           hour_bits, separator, minute_bits, separator,
                           second_bits)
            : write_printf(buffer, capacity, length, "%s%s%s",
                           hour_bits, separator, minute_bits);
    }

    if (strcmp(mode, "sidereal") == 0 ||
        strcmp(mode, "solar") == 0 ||
        strcmp(mode, "mean-solar") == 0) {
        long double clock_seconds;
        const char *suffix;
        int hour;
        int minute;
        int second;

        if (strcmp(mode, "sidereal") == 0) {
            clock_seconds =
                local_sidereal_seconds(unix_microseconds, longitude);
            suffix = "LST";
        } else if (strcmp(mode, "solar") == 0) {
            clock_seconds =
                apparent_solar_seconds(unix_microseconds, longitude);
            suffix = "SOL";
        } else {
            clock_seconds =
                mean_solar_seconds(unix_microseconds, longitude);
            suffix = "LMT";
        }

        split_clock_seconds(
            (int64_t)floorl(clock_seconds), &hour, &minute, &second);
        return write_clock_fields(
            buffer, capacity, length, hour, minute, second,
            show_seconds, vertical, suffix);
    }

    if (strcmp(mode, "julian") == 0 ||
        strcmp(mode, "modified-julian") == 0) {
        const int digits = show_seconds ? 5 : 3;
        const int64_t scale =
            show_seconds ? INT64_C(100000) : INT64_C(1000);
        const long double date =
            julian_date(unix_microseconds) -
            (strcmp(mode, "modified-julian") == 0 ? 2400000.5L : 0.0L);
        const int64_t whole_days = (int64_t)floorl(date);
        int64_t fraction = (int64_t)floorl(
            (date - (long double)whole_days) * scale + 1.0e-10L);

        if (fraction >= scale) {
            fraction = scale - 1;
        }
        return write_printf(
            buffer, capacity, length,
            strcmp(mode, "modified-julian") == 0
                ? "MJD%s%lld.%0*lld"
                : "JD%s%lld.%0*lld",
            vertical ? "\n" : " ",
            (long long)whole_days, digits, (long long)fraction);
    }

    if (strcmp(mode, "chinese-time") == 0) {
        static const char *const branches[] = {
            "子 Zǐ (Rat)", "丑 Chǒu (Ox)", "寅 Yín (Tiger)",
            "卯 Mǎo (Rabbit)", "辰 Chén (Dragon)", "巳 Sì (Snake)",
            "午 Wǔ (Horse)", "未 Wèi (Goat)", "申 Shēn (Monkey)",
            "酉 Yǒu (Rooster)", "戌 Xū (Dog)", "亥 Hài (Boar)"
        };
        const int64_t shifted = positive_modulo(
            local + SECONDS_PER_HOUR * MICROSECONDS_PER_SECOND,
            MICROSECONDS_PER_DAY);
        const unsigned branch = (unsigned)(shifted /
            (INT64_C(2) * SECONDS_PER_HOUR * MICROSECONDS_PER_SECOND));

        if (!vertical) {
            return write_text(buffer, capacity, length, branches[branch]);
        }
        {
            const char *first = strchr(branches[branch], ' ');
            const char *second_space =
                first != NULL ? strchr(first + 1, ' ') : NULL;

            if (first == NULL || second_space == NULL) {
                return write_text(
                    buffer, capacity, length, branches[branch]);
            }
            return write_printf(
                buffer, capacity, length, "%.*s\n%.*s\n%s",
                (int)(first - branches[branch]), branches[branch],
                (int)(second_space - first - 1), first + 1,
                second_space + 1);
        }
    }

    if (strcmp(mode, "chinese-ke") == 0) {
        const uint64_t ke = day_tick(local, UINT64_C(100));

        return write_printf(
            buffer, capacity, length,
            vertical ? "刻\n%02llu/100" : "刻 %02llu/100",
            (unsigned long long)ke);
    }

    if (strcmp(mode, "roman-temporal") == 0) {
        static const char *const roman[] = {
            "I", "II", "III", "IV", "V", "VI",
            "VII", "VIII", "IX", "X", "XI", "XII"
        };
        SeasonalPeriod period;

        if (!seasonal_period_at(
                unix_microseconds, latitude, longitude,
                0.833, 12U, 4U, &period)) {
            return write_text(
                buffer, capacity, length,
                vertical ? "N/A\nROM" : "N/A ROM");
        }
        return write_printf(
            buffer, capacity, length,
            period.daylight
                ? (vertical ? "Hora\n%s" : "Hora %s")
                : (vertical ? "Vigilia\n%s" : "Vigilia %s"),
            roman[period.index]);
    }

    if (strcmp(mode, "japanese-temporal") == 0) {
        typedef struct JapaneseToki {
            const char *character;
            unsigned number;
            const char *animal;
        } JapaneseToki;
        static const JapaneseToki day_toki[] = {
            { "卯", 6U, "Rabbit" }, { "辰", 5U, "Dragon" },
            { "巳", 4U, "Snake" }, { "午", 9U, "Horse" },
            { "未", 8U, "Goat" }, { "申", 7U, "Monkey" }
        };
        static const JapaneseToki night_toki[] = {
            { "酉", 6U, "Rooster" }, { "戌", 5U, "Dog" },
            { "亥", 4U, "Boar" }, { "子", 9U, "Rat" },
            { "丑", 8U, "Ox" }, { "寅", 7U, "Tiger" }
        };
        const double depression =
            7.0 + 21.0 / 60.0 + 40.0 / 3600.0;
        SeasonalPeriod period;
        const JapaneseToki *toki;

        if (!seasonal_period_at(
                unix_microseconds, latitude, longitude,
                depression, 6U, 6U, &period)) {
            return write_text(
                buffer, capacity, length,
                vertical ? "N/A\n和時" : "N/A 和時");
        }
        toki = period.daylight
            ? &day_toki[period.index]
            : &night_toki[period.index];
        return write_printf(
            buffer, capacity, length,
            vertical ? "%s %u\n%s" : "%s %u %s",
            toki->character, toki->number, toki->animal);
    }

    if (strcmp(mode, "italian-hours") == 0) {
        return format_equal_hours(
            buffer, capacity, length, unix_microseconds,
            show_seconds, vertical, latitude, longitude,
            SOLAR_ORIGIN_SUNSET, "IT");
    }

    if (strcmp(mode, "babylonian-hours") == 0) {
        return format_equal_hours(
            buffer, capacity, length, unix_microseconds,
            show_seconds, vertical, latitude, longitude,
            SOLAR_ORIGIN_SUNRISE, "BAB");
    }

    if (strcmp(mode, "indian-ghati") == 0) {
        int64_t start;
        int64_t elapsed_seconds;
        int ghati;
        int vighati;

        if (!solar_origin_window(
                unix_microseconds, latitude, longitude,
                SOLAR_ORIGIN_SUNRISE, &start)) {
            return write_text(
                buffer, capacity, length,
                vertical ? "N/A\nGH" : "N/A GH");
        }
        elapsed_seconds = floor_divide(
            unix_microseconds - start, MICROSECONDS_PER_SECOND);
        ghati = (int)(elapsed_seconds /
                      (INT64_C(24) * SECONDS_PER_MINUTE));
        vighati =
            (int)((elapsed_seconds / INT64_C(24)) % INT64_C(60));
        return write_printf(
            buffer, capacity, length,
            vertical ? "GH\n%02d:%02d" : "GH %02d:%02d",
            ghati, vighati);
    }

    if (strcmp(mode, "nuremberg-hours") == 0) {
        int64_t best_previous = INT64_MIN;
        bool previous_sunrise = true;
        int offset;
        int64_t elapsed_seconds;
        int hour;
        int minute;
        int second;
        const char *period;

        for (offset = -2; offset <= 2; ++offset) {
            int64_t delta;
            int64_t sample;
            int64_t dawn;
            int64_t dusk;

            if (!infiltratr_i64_multiply_checked(
                    (int64_t)offset, MICROSECONDS_PER_DAY, &delta) ||
                !infiltratr_i64_add_checked(
                    unix_microseconds, delta, &sample) ||
                !solar_boundary_instants(
                    sample, latitude, longitude, 0.833, &dawn, &dusk)) {
                continue;
            }
            if (dawn <= unix_microseconds && dawn > best_previous) {
                best_previous = dawn;
                previous_sunrise = true;
            }
            if (dusk <= unix_microseconds && dusk > best_previous) {
                best_previous = dusk;
                previous_sunrise = false;
            }
        }

        if (best_previous == INT64_MIN) {
            return write_text(
                buffer, capacity, length,
                vertical ? "N/A\nNUR" : "N/A NUR");
        }
        elapsed_seconds = floor_divide(
            unix_microseconds - best_previous, MICROSECONDS_PER_SECOND);
        hour = (int)(elapsed_seconds / SECONDS_PER_HOUR);
        minute =
            (int)((elapsed_seconds / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR);
        second = (int)(elapsed_seconds % SECONDS_PER_MINUTE);
        period = previous_sunrise ? "NUR-D" : "NUR-N";

        if (show_seconds) {
            return write_printf(
                buffer, capacity, length, "%02d%s%02d%s%02d%s%s",
                hour, separator, minute, separator, second,
                vertical ? "\n" : " ", period);
        }
        return write_printf(
            buffer, capacity, length, "%02d%s%02d%s%s",
            hour, separator, minute, vertical ? "\n" : " ", period);
    }

    return false;
}
