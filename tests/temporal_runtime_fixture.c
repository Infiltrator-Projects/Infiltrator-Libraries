// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/temporal_runtime.h"

#include <string.h>

#if defined(_WIN32)
#define TEMPORAL_FIXTURE_EXPORT __declspec(dllexport)
#else
#define TEMPORAL_FIXTURE_EXPORT __attribute__((visibility("default")))
#endif

static bool fixture_fields(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    InfiltratrTemporalCalendarFields *fields)
{
    (void)provider;
    if (fields == NULL) return false;
    fields->year = jdn;
    fields->month = 1;
    fields->day = 1;
    fields->auxiliary = 0;
    fields->special = false;
    return true;
}

static bool fixture_write(const char *text, char *buffer, size_t capacity,
                          size_t *length)
{
    const size_t size = strlen(text);
    if (buffer == NULL || capacity <= size) return false;
    memcpy(buffer, text, size + 1U);
    if (length != NULL) *length = size;
    return true;
}

static bool fixture_format_jdn(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    InfiltratrTemporalCalendarDatePart part,
    char *buffer,
    size_t capacity,
    size_t *length)
{
    (void)provider;
    (void)jdn;
    return fixture_write(
        part == INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_FULL
            ? "Fixture full" : "Fixture",
        buffer, capacity, length);
}

static bool fixture_format_civil(
    const InfiltratrTemporalCalendarProvider *provider,
    int32_t year,
    int32_t month,
    int32_t day,
    InfiltratrTemporalCalendarDatePart part,
    char *buffer,
    size_t capacity,
    size_t *length)
{
    (void)provider;
    if (year != 2026 || month != 10 || day != 17) return false;
    return fixture_write(
        part == INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_FULL
            ? "Fixture civil full" : "Fixture civil",
        buffer, capacity, length);
}

static bool fixture_period_start(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    int64_t *result_jdn)
{
    (void)provider;
    if (result_jdn == NULL) return false;
    *result_jdn = jdn - 1;
    return true;
}

static bool fixture_add(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    int32_t amount,
    int64_t *result_jdn)
{
    (void)provider;
    if (result_jdn == NULL) return false;
    *result_jdn = jdn + amount;
    return true;
}

TEMPORAL_FIXTURE_EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void);
TEMPORAL_FIXTURE_EXPORT const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_calendar_provider_v1(void);

TEMPORAL_FIXTURE_EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void)
{
    static const InfiltratrTemporalResourceMetadata metadata = {
        sizeof(InfiltratrTemporalResourceMetadata),
        INFILTRATR_TEMPORAL_RESOURCE_METADATA_ABI,
        INFILTRATR_TEMPORAL_RESOURCE_CALENDAR,
        "fixture-calendar",
        "Fixture Calendar",
        false,
        false,
        false
    };
    return &metadata;
}

TEMPORAL_FIXTURE_EXPORT const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_calendar_provider_v1(void)
{
    static const InfiltratrTemporalCalendarProvider provider = {
        sizeof(InfiltratrTemporalCalendarProvider),
        INFILTRATR_TEMPORAL_CALENDAR_PROVIDER_ABI,
        "fixture-calendar",
        fixture_fields,
        fixture_format_jdn,
        fixture_format_civil,
        fixture_period_start,
        fixture_add,
        fixture_add
    };
    return &provider;
}
