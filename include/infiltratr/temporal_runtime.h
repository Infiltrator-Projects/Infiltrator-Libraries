// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file temporal_runtime.h
 * @brief Validated runtime catalogue for native temporal resource modules.
 *
 * Candidate discovery remains owned by temporal_resource.h. This layer owns
 * executable temporal-resource validation and lifetime. Calendar applications,
 * file managers, shells and settings surfaces consume the same Common-owned
 * calendar provider contract instead of loading one another's runtimes.
 */
#ifndef INFILTRATR_COMMON_TEMPORAL_RUNTIME_H
#define INFILTRATR_COMMON_TEMPORAL_RUNTIME_H

#include "infiltratr/temporal.h"
#include "infiltratr/temporal_calendar_resource.h"
#include "infiltratr/temporal_resource.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INFILTRATR_TEMPORAL_RESOURCE_METADATA_ABI 1U
#define INFILTRATR_TEMPORAL_RESOURCE_METADATA_SYMBOL \
    "infiltratr_temporal_resource_metadata_v1"

#ifndef INFILTRATR_TEMPORAL_NATIVE_CALENDAR_DIRECTORY
#define INFILTRATR_TEMPORAL_NATIVE_CALENDAR_DIRECTORY \
    "/System/Resources/Calendars"
#endif
#ifndef INFILTRATR_TEMPORAL_NATIVE_CLOCK_DIRECTORY
#define INFILTRATR_TEMPORAL_NATIVE_CLOCK_DIRECTORY \
    "/System/Resources/Clocks"
#endif

/** Product-neutral identity exported by one native temporal resource module. */
typedef struct InfiltratrTemporalResourceMetadata {
    size_t struct_size;
    uint32_t abi_version;
    InfiltratrTemporalResourceKind kind;
    const char *id;
    const char *name;
    bool supports_seconds;
    bool requires_latitude;
    bool requires_longitude;
} InfiltratrTemporalResourceMetadata;

typedef const InfiltratrTemporalResourceMetadata *
(*InfiltratrTemporalResourceMetadataEntry)(void);

typedef bool (*InfiltratrTemporalRuntimeVisitor)(
    const InfiltratrTemporalResourceMetadata *metadata,
    const char *module_path,
    void *user_data);

/**
 * Inspect direct file resources in one explicit directory.
 *
 * Each file is opened only long enough to resolve and validate the generic
 * metadata symbol. Non-modules, modules without the symbol, malformed metadata
 * and resources of another kind are ignored independently. Directory bundles
 * remain reserved for a later resource ABI and are not executable in ABI 1.
 */
bool infiltratr_temporal_runtime_scan_directory(
    const char *directory,
    InfiltratrTemporalResourceKind kind,
    InfiltratrTemporalRuntimeVisitor visitor,
    void *user_data,
    size_t *visited);

/** Scan the canonical InfiltratorOS native resource directory for a kind. */
bool infiltratr_temporal_runtime_scan_native(
    InfiltratrTemporalResourceKind kind,
    InfiltratrTemporalRuntimeVisitor visitor,
    void *user_data,
    size_t *visited);

/**
 * One explicitly loaded executable calendar module.
 *
 * Common owns the dynamic-library handle, validates both metadata and behaviour
 * ABI, and keeps borrowed module strings/function pointers alive until close.
 */
typedef struct InfiltratrTemporalCalendarModule
    InfiltratrTemporalCalendarModule;

InfiltratrTemporalCalendarModule *
infiltratr_temporal_calendar_module_open(const char *module_path);
void infiltratr_temporal_calendar_module_close(
    InfiltratrTemporalCalendarModule *module);
const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_calendar_module_metadata(
    const InfiltratrTemporalCalendarModule *module);
const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_calendar_module_provider(
    const InfiltratrTemporalCalendarModule *module);

/**
 * Common-owned executable native calendar catalogue.
 *
 * Modules are validated once and retained for process lifetime so consumers can
 * safely keep borrowed provider pointers. Duplicate IDs are deterministic:
 * the first validated native module wins.
 */
size_t infiltratr_temporal_installed_calendar_provider_count(void);
const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_installed_calendar_provider_at(size_t index);
const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_installed_calendar_provider_find(const char *id);

/**
 * Merged compiled-base + validated native Calendar metadata catalogue.
 *
 * The compiled base is retained during migration so existing consumers remain
 * stable while implementations move one-by-one into native resources. Native
 * resources are appended in deterministic filesystem order.
 */
size_t infiltratr_temporal_installed_calendar_count(void);
const InfiltratrTemporalCalendarInfo *
infiltratr_temporal_installed_calendar_at(size_t index);
const InfiltratrTemporalCalendarInfo *
infiltratr_temporal_installed_calendar_find(const char *id);

/** Merged metadata catalogue for clocks. */
size_t infiltratr_temporal_installed_clock_mode_count(void);
const InfiltratrTemporalClockModeInfo *
infiltratr_temporal_installed_clock_mode_at(size_t index);
const InfiltratrTemporalClockModeInfo *
infiltratr_temporal_installed_clock_mode_find(const char *id);

#ifdef __cplusplus
}
#endif

#endif
