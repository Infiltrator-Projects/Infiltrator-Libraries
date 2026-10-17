// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/temporal_calendar.h"

#include <string.h>

#if defined(_WIN32)
#define TEMPORAL_DATE_FIXTURE_EXPORT __declspec(dllexport)
#else
#define TEMPORAL_DATE_FIXTURE_EXPORT __attribute__((visibility("default")))
#endif

static bool fixture_fields(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    InfiltratrTemporalCalendarFields *fields)
{
    (void)provider;
    if (fields == NULL || jdn != INT64_C(2461324)) {
        return false;
    }
    *fields = (InfiltratrTemporalCalendarFields){
        .year = 2026,
        .month = 10,
        .day = 10,
        .auxiliary = 0,
        .special = false
    };
    return true;
}

static bool fixture_format(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    InfiltratrTemporalCalendarDatePart part,
    char *buffer,
    size_t capacity,
    size_t *length)
{
    const char *text;
    size_t size;

    (void)provider;
    if (jdn != INT64_C(2461324) ||
        (part != INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_SHORT &&
         part != INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_FULL)) {
        return false;
    }
    text = part == INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_FULL
        ? "中文日期 full" : "中文日期";
    size = strlen(text);
    if (buffer == NULL || capacity <= size) {
        return false;
    }
    memcpy(buffer, text, size + 1U);
    if (length != NULL) {
        *length = size;
    }
    return true;
}

static bool fixture_period_start(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    int64_t *result)
{
    (void)provider;
    if (result == NULL) return false;
    *result = jdn;
    return true;
}

static bool fixture_add(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    int32_t amount,
    int64_t *result)
{
    (void)provider;
    if (result == NULL) return false;
    *result = jdn + amount;
    return true;
}

TEMPORAL_DATE_FIXTURE_EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void);
TEMPORAL_DATE_FIXTURE_EXPORT const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_calendar_provider_v1(void);

TEMPORAL_DATE_FIXTURE_EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void)
{
    static const InfiltratrTemporalResourceMetadata metadata = {
        .struct_size = sizeof(InfiltratrTemporalResourceMetadata),
        .abi_version = INFILTRATR_TEMPORAL_RESOURCE_METADATA_ABI,
        .kind = INFILTRATR_TEMPORAL_RESOURCE_CALENDAR,
        .id = "chinese",
        .name = "Fixture Chinese Calendar",
        .supports_seconds = false,
        .requires_latitude = false,
        .requires_longitude = false
    };
    return &metadata;
}

TEMPORAL_DATE_FIXTURE_EXPORT const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_calendar_provider_v1(void)
{
    static const InfiltratrTemporalCalendarProvider provider = {
        .struct_size = sizeof(InfiltratrTemporalCalendarProvider),
        .abi_version = INFILTRATR_TEMPORAL_CALENDAR_PROVIDER_ABI,
        .calendar_id = "chinese",
        .fields_from_jdn = fixture_fields,
        .format = fixture_format,
        .period_start = fixture_period_start,
        .add_periods = fixture_add,
        .add_years = fixture_add
    };
    return &provider;
}
