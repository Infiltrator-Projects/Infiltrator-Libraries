// SPDX-License-Identifier: GPL-3.0-or-later
/* Calendar Resource date rendering without an application dependency. */
#define _POSIX_C_SOURCE 200809L

#include "infiltratr/temporal_posix.h"
#include "infiltratr/temporal_calendar.h"
#include "infiltratr/timing.h"

#include <stdlib.h>
#include <string.h>

struct InfiltratrTemporalDateProvider {
    InfiltratrTemporalCalendarModule *module;
    char module_id[INFILTRATR_TEMPORAL_ID_CAPACITY];
    char retry_id[INFILTRATR_TEMPORAL_ID_CAPACITY];
    bool discover;
    uint64_t retry_after_nanoseconds;
};

static int64_t floor_divide(int64_t value, int64_t divisor)
{
    int64_t quotient;
    int64_t remainder;

    quotient = value / divisor;
    remainder = value % divisor;
    if (remainder != 0 && ((remainder < 0) != (divisor < 0))) {
        quotient--;
    }
    return quotient;
}

static bool gregorian_is_leap(int32_t year)
{
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

static bool gregorian_date_valid(int32_t year, int32_t month, int32_t day)
{
    static const int32_t lengths[] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    int32_t maximum;

    (void)year;
    if (month < 1 || month > 12 || day < 1) {
        return false;
    }
    maximum = lengths[month - 1];
    if (month == 2 && gregorian_is_leap(year)) {
        maximum++;
    }
    return day <= maximum;
}

static bool gregorian_to_jdn(int32_t year, int32_t month, int32_t day,
                             int64_t *jdn)
{
    int64_t a;
    int64_t y;
    int64_t m;

    if (jdn == NULL || !gregorian_date_valid(year, month, day)) {
        return false;
    }
    a = (14 - (int64_t)month) / 12;
    y = (int64_t)year + 4800 - a;
    m = (int64_t)month + 12 * a - 3;
    *jdn = (int64_t)day + (153 * m + 2) / 5 + 365 * y +
        floor_divide(y, 4) - floor_divide(y, 100) +
        floor_divide(y, 400) - 32045;
    return true;
}

static bool copy_id(char destination[INFILTRATR_TEMPORAL_ID_CAPACITY],
                    const char *source)
{
    size_t length;

    if (destination == NULL || source == NULL) {
        return false;
    }
    length = strlen(source);
    if (length >= INFILTRATR_TEMPORAL_ID_CAPACITY) {
        return false;
    }
    memcpy(destination, source, length + 1U);
    return true;
}

static void clear_module(InfiltratrTemporalDateProvider *provider)
{
    if (provider == NULL) {
        return;
    }
    infiltratr_temporal_calendar_module_free(provider->module);
    provider->module = NULL;
    provider->module_id[0] = '\0';
}

static bool module_matches(const InfiltratrTemporalDateProvider *provider,
                           const char *calendar_id)
{
    return provider != NULL && provider->module != NULL &&
        calendar_id != NULL &&
        strcmp(provider->module_id, calendar_id) == 0;
}

static bool ensure_module(InfiltratrTemporalDateProvider *provider,
                          const char *calendar_id)
{
    const InfiltratrTemporalResourceMetadata *metadata;
    uint64_t now = 0U;

    if (provider == NULL || calendar_id == NULL || calendar_id[0] == '\0') {
        return false;
    }
    if (module_matches(provider, calendar_id)) {
        return true;
    }
    if (!provider->discover) {
        return false;
    }

    clear_module(provider);
    if (!infiltratr_monotonic_nanoseconds(&now)) {
        return false;
    }
    if (strcmp(provider->retry_id, calendar_id) == 0 &&
        now < provider->retry_after_nanoseconds) {
        return false;
    }
    provider->retry_id[0] = '\0';
    if (!copy_id(provider->retry_id, calendar_id)) {
        return false;
    }
    provider->retry_after_nanoseconds =
        now <= UINT64_MAX - UINT64_C(60000000000)
            ? now + UINT64_C(60000000000)
            : UINT64_MAX;

    provider->module = infiltratr_temporal_calendar_module_open_id(calendar_id);
    if (provider->module == NULL) {
        return false;
    }
    metadata = infiltratr_temporal_calendar_module_metadata(provider->module);
    if (metadata == NULL || metadata->id == NULL ||
        !copy_id(provider->module_id, metadata->id)) {
        clear_module(provider);
        return false;
    }
    provider->retry_id[0] = '\0';
    provider->retry_after_nanoseconds = 0U;
    return module_matches(provider, calendar_id);
}

InfiltratrTemporalDateProvider *infiltratr_temporal_posix_date_provider_new(void)
{
    InfiltratrTemporalDateProvider *provider = calloc(1U, sizeof(*provider));
    if (provider != NULL) {
        provider->discover = true;
    }
    return provider;
}

InfiltratrTemporalDateProvider *infiltratr_temporal_posix_date_provider_new_from(
    const char *library_path)
{
    InfiltratrTemporalDateProvider *provider;
    const InfiltratrTemporalResourceMetadata *metadata;

    if (library_path == NULL || library_path[0] != '/') {
        return NULL;
    }
    provider = calloc(1U, sizeof(*provider));
    if (provider == NULL) {
        return NULL;
    }
    provider->module = infiltratr_temporal_calendar_module_open(library_path);
    if (provider->module == NULL) {
        free(provider);
        return NULL;
    }
    metadata = infiltratr_temporal_calendar_module_metadata(provider->module);
    if (metadata == NULL || metadata->id == NULL ||
        !copy_id(provider->module_id, metadata->id)) {
        infiltratr_temporal_posix_date_provider_free(provider);
        return NULL;
    }
    return provider;
}

void infiltratr_temporal_posix_date_provider_free(
    InfiltratrTemporalDateProvider *provider)
{
    if (provider != NULL) {
        clear_module(provider);
        free(provider);
    }
}

bool infiltratr_temporal_posix_format_date(
    InfiltratrTemporalDateProvider *provider,
    const char *calendar_id,
    int32_t gregorian_year,
    int32_t gregorian_month,
    int32_t gregorian_day,
    const char *part,
    char *buffer,
    size_t capacity,
    size_t *length)
{
    char text[INFILTRATR_TEMPORAL_DATE_CAPACITY] = { 0 };
    InfiltratrTemporalCalendarDatePart calendar_part;
    int64_t jdn;
    size_t size = 0U;

    if (buffer == NULL || capacity == 0U || part == NULL ||
        !gregorian_to_jdn(gregorian_year, gregorian_month, gregorian_day, &jdn)) {
        return false;
    }
    if (strcmp(part, "short") == 0) {
        calendar_part = INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_SHORT;
    } else if (strcmp(part, "full") == 0) {
        calendar_part = INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_FULL;
    } else {
        return false;
    }
    if (!ensure_module(provider, calendar_id) ||
        !infiltratr_temporal_calendar_format(
            provider->module, jdn, calendar_part,
            text, sizeof(text), &size) ||
        size == 0U || size >= capacity) {
        return false;
    }

    memcpy(buffer, text, size + 1U);
    if (length != NULL) {
        *length = size;
    }
    return true;
}
