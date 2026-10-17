// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/temporal_calendar.h"

#include <stdio.h>
#include <string.h>

static bool fixture_fields(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    InfiltratrTemporalCalendarFields *fields)
{
    (void)provider;
    if (fields == NULL || jdn != INT64_C(2461331)) {
        return false;
    }
    *fields = (InfiltratrTemporalCalendarFields){
        INT64_C(5787), 1, 6, 0, false
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
    if (jdn != INT64_C(2461331) || buffer == NULL || length == NULL) {
        return false;
    }
    text = part == INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_FULL
        ? "Fixture day 6" : "6 Fixture 5787";
    size = strlen(text);
    if (capacity <= size) {
        return false;
    }
    memcpy(buffer, text, size + 1U);
    *length = size;
    return true;
}

static bool fixture_period_start(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    int64_t *result)
{
    (void)provider;
    if (result == NULL || jdn != INT64_C(2461331)) {
        return false;
    }
    *result = INT64_C(2461326);
    return true;
}

static bool fixture_add_periods(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    int32_t amount,
    int64_t *result)
{
    (void)provider;
    if (result == NULL) {
        return false;
    }
    *result = jdn + (int64_t)amount * INT64_C(30);
    return true;
}

static bool fixture_add_years(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    int32_t amount,
    int64_t *result)
{
    (void)provider;
    if (result == NULL) {
        return false;
    }
    *result = jdn + (int64_t)amount * INT64_C(365);
    return true;
}

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

static const InfiltratrTemporalCalendarProvider provider = {
    sizeof(InfiltratrTemporalCalendarProvider),
    INFILTRATR_TEMPORAL_CALENDAR_PROVIDER_ABI,
    "fixture-calendar",
    fixture_fields,
    fixture_format,
    fixture_period_start,
    fixture_add_periods,
    fixture_add_years
};

#ifdef _WIN32
#define FIXTURE_EXPORT __declspec(dllexport)
#else
#define FIXTURE_EXPORT __attribute__((visibility("default")))
#endif

FIXTURE_EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void);
FIXTURE_EXPORT const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_calendar_provider_v1(void);

FIXTURE_EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void)
{
    return &metadata;
}

FIXTURE_EXPORT const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_calendar_provider_v1(void)
{
    return &provider;
}
