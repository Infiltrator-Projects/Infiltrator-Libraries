// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file temporal_calendar_resource.h
 * @brief Product-neutral executable ABI for installed calendar resources.
 *
 * Calendar applications, file managers, shells and settings surfaces consume
 * the same installed calendar capability.  The implementation lives in the
 * resource module; Common owns only this ABI and the loader that validates it.
 */
#ifndef INFILTRATR_COMMON_TEMPORAL_CALENDAR_RESOURCE_H
#define INFILTRATR_COMMON_TEMPORAL_CALENDAR_RESOURCE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INFILTRATR_TEMPORAL_CALENDAR_PROVIDER_ABI 1U
#define INFILTRATR_TEMPORAL_CALENDAR_PROVIDER_SYMBOL \
    "infiltratr_temporal_calendar_provider_v1"
#define INFILTRATR_TEMPORAL_CALENDAR_TEXT_CAPACITY 512U

typedef enum InfiltratrTemporalCalendarDatePart {
    INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_INVALID = 0,
    INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_DAY,
    INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_MONTH,
    INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_YEAR,
    INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_SHORT,
    INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_FULL
} InfiltratrTemporalCalendarDatePart;

typedef struct InfiltratrTemporalCalendarFields {
    int64_t year;
    int32_t month;
    int32_t day;
    int32_t auxiliary;
    bool special;
} InfiltratrTemporalCalendarFields;

typedef struct InfiltratrTemporalCalendarProvider
    InfiltratrTemporalCalendarProvider;

typedef bool (*InfiltratrTemporalCalendarFieldsFromJdnFunc)(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    InfiltratrTemporalCalendarFields *fields);

typedef bool (*InfiltratrTemporalCalendarFormatJdnFunc)(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    InfiltratrTemporalCalendarDatePart part,
    char *buffer,
    size_t capacity,
    size_t *length);

typedef bool (*InfiltratrTemporalCalendarFormatCivilFunc)(
    const InfiltratrTemporalCalendarProvider *provider,
    int32_t gregorian_year,
    int32_t gregorian_month,
    int32_t gregorian_day,
    InfiltratrTemporalCalendarDatePart part,
    char *buffer,
    size_t capacity,
    size_t *length);

typedef bool (*InfiltratrTemporalCalendarNavigateFunc)(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    int32_t amount,
    int64_t *result_jdn);

typedef bool (*InfiltratrTemporalCalendarPeriodStartFunc)(
    const InfiltratrTemporalCalendarProvider *provider,
    int64_t jdn,
    int64_t *result_jdn);

/**
 * Complete executable calendar capability exported by one resource module.
 *
 * The structure deliberately uses only fixed-width C types and caller-owned
 * text buffers.  No GLib, GTK, C++ object, allocator ownership or application
 * private type crosses the system resource boundary.  calendar_id is the same
 * stable identifier exported by the module's generic temporal metadata.
 */
struct InfiltratrTemporalCalendarProvider {
    size_t struct_size;
    uint32_t abi_version;
    const char *calendar_id;
    InfiltratrTemporalCalendarFieldsFromJdnFunc fields_from_jdn;
    InfiltratrTemporalCalendarFormatJdnFunc format_jdn;
    InfiltratrTemporalCalendarFormatCivilFunc format_civil;
    InfiltratrTemporalCalendarPeriodStartFunc period_start;
    InfiltratrTemporalCalendarNavigateFunc add_periods;
    InfiltratrTemporalCalendarNavigateFunc add_years;
};

typedef const InfiltratrTemporalCalendarProvider *
(*InfiltratrTemporalCalendarProviderEntry)(void);

#ifdef __cplusplus
}
#endif

#endif
