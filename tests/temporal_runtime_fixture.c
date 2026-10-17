// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/temporal_runtime.h"

#if defined(_WIN32)
#define TEMPORAL_FIXTURE_EXPORT __declspec(dllexport)
#else
#define TEMPORAL_FIXTURE_EXPORT __attribute__((visibility("default")))
#endif

TEMPORAL_FIXTURE_EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void);

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
