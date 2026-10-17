// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/temporal_runtime.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifndef TEMPORAL_RUNTIME_FIXTURE_PATH
#error "TEMPORAL_RUNTIME_FIXTURE_PATH must name the runtime metadata fixture"
#endif

static bool capture_fixture(
    const InfiltratrTemporalResourceMetadata *metadata,
    const char *module_path,
    void *user_data)
{
    size_t *seen = user_data;

    assert(metadata != NULL);
    assert(module_path != NULL && module_path[0] != '\0');
    assert(metadata->struct_size >= sizeof(*metadata));
    assert(metadata->abi_version == INFILTRATR_TEMPORAL_RESOURCE_METADATA_ABI);
    assert(metadata->kind == INFILTRATR_TEMPORAL_RESOURCE_CALENDAR);
    assert(strcmp(metadata->id, "fixture-calendar") == 0);
    assert(strcmp(metadata->name, "Fixture Calendar") == 0);
    (*seen)++;
    return true;
}

static void fixture_directory(char *directory, size_t capacity)
{
    const char *path = TEMPORAL_RUNTIME_FIXTURE_PATH;
    const char *slash = strrchr(path, '/');
    const char *backslash = strrchr(path, '\\');
    const char *separator = slash;
    size_t length;

    if (backslash != NULL && (separator == NULL || backslash > separator)) {
        separator = backslash;
    }
    assert(separator != NULL);
    length = (size_t)(separator - path);
    assert(length != 0U && length < capacity);
    memcpy(directory, path, length);
    directory[length] = '\0';
}

int main(void)
{
    char directory[1024];
    char text[64];
    size_t seen = 0U;
    size_t visited = 99U;
    size_t length = 0U;
    int64_t result_jdn = 0;
    InfiltratrTemporalCalendarFields fields = { 0 };
    InfiltratrTemporalCalendarModule *module;
    const InfiltratrTemporalResourceMetadata *metadata;
    const InfiltratrTemporalCalendarProvider *provider;

    fixture_directory(directory, sizeof(directory));
    assert(infiltratr_temporal_runtime_scan_directory(
        directory,
        INFILTRATR_TEMPORAL_RESOURCE_CALENDAR,
        capture_fixture,
        &seen,
        &visited));
    assert(seen == 1U);
    assert(visited == 1U);

    visited = 99U;
    assert(infiltratr_temporal_runtime_scan_directory(
        directory,
        INFILTRATR_TEMPORAL_RESOURCE_CLOCK,
        NULL,
        NULL,
        &visited));
    assert(visited == 0U);

    module = infiltratr_temporal_calendar_module_open(
        TEMPORAL_RUNTIME_FIXTURE_PATH);
    assert(module != NULL);
    metadata = infiltratr_temporal_calendar_module_metadata(module);
    provider = infiltratr_temporal_calendar_module_provider(module);
    assert(metadata != NULL && provider != NULL);
    assert(strcmp(metadata->id, "fixture-calendar") == 0);
    assert(strcmp(provider->calendar_id, metadata->id) == 0);
    assert(provider->fields_from_jdn(provider, 2460000, &fields));
    assert(fields.year == 2460000 && fields.month == 1 && fields.day == 1);
    assert(provider->format_jdn(
        provider, 2460000,
        INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_FULL,
        text, sizeof(text), &length));
    assert(strcmp(text, "Fixture full") == 0 && length == strlen(text));
    assert(provider->format_civil(
        provider, 2026, 10, 17,
        INFILTRATR_TEMPORAL_CALENDAR_DATE_PART_SHORT,
        text, sizeof(text), &length));
    assert(strcmp(text, "Fixture civil") == 0);
    assert(provider->period_start(provider, 100, &result_jdn));
    assert(result_jdn == 99);
    assert(provider->add_periods(provider, 100, 2, &result_jdn));
    assert(result_jdn == 102);
    infiltratr_temporal_calendar_module_close(module);

    assert(infiltratr_temporal_installed_calendar_count() >=
           infiltratr_temporal_calendar_count());
    assert(infiltratr_temporal_installed_clock_mode_count() >=
           infiltratr_temporal_clock_mode_count());
    assert(infiltratr_temporal_installed_calendar_find("gregorian") != NULL);
    assert(infiltratr_temporal_installed_clock_mode_find("standard-24") != NULL);

    puts("Libraries temporal runtime smoke test passed.");
    return 0;
}
