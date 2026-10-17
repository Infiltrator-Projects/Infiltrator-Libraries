// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/temporal_clock_resource.h"
#include <string.h>

static bool render(const InfiltratrTemporalClockProvider *provider,
                   const char *locale, int64_t instant, int32_t offset,
                   bool seconds, bool vertical, double latitude, double longitude,
                   char *buffer, size_t capacity, size_t *length)
{
    const char *text = seconds ? "42:99" : "42";
    size_t size = strlen(text);
    (void)provider; (void)locale; (void)offset; (void)vertical;
    (void)latitude; (void)longitude;
    if (instant != INT64_C(1234567) || capacity <= size || buffer == NULL) return false;
    memcpy(buffer, text, size + 1U);
    if (length != NULL) *length = size;
    return true;
}

static bool next_tick(const InfiltratrTemporalClockProvider *provider,
                      int64_t instant, int32_t offset, bool seconds,
                      double latitude, double longitude, uint32_t *milliseconds)
{
    (void)provider; (void)offset; (void)latitude; (void)longitude;
    if (milliseconds == NULL || instant != INT64_C(1234567)) return false;
    *milliseconds = seconds ? 42U : 99U;
    return true;
}

static const InfiltratrTemporalResourceMetadata metadata = {
    sizeof(InfiltratrTemporalResourceMetadata),
    INFILTRATR_TEMPORAL_RESOURCE_METADATA_ABI,
    INFILTRATR_TEMPORAL_RESOURCE_CLOCK,
    "fixture-clock", "Fixture Clock", true, false, false
};
static const InfiltratrTemporalClockProvider provider = {
    sizeof(InfiltratrTemporalClockProvider),
    INFILTRATR_TEMPORAL_CLOCK_PROVIDER_ABI,
    "fixture-clock", render, next_tick
};
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif
EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void);
EXPORT const InfiltratrTemporalClockProvider *
infiltratr_temporal_clock_provider_v1(void);
EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void) { return &metadata; }
EXPORT const InfiltratrTemporalClockProvider *
infiltratr_temporal_clock_provider_v1(void) { return &provider; }
