// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file temporal_runtime.c
 * @brief Validated executable temporal-resource discovery and catalogues.
 */
#include "infiltratr/temporal_runtime.h"

#include "infiltratr/dynlib.h"

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

struct InfiltratrTemporalCalendarModule {
    InfiltratrDynlib library;
    const InfiltratrTemporalResourceMetadata *metadata;
    const InfiltratrTemporalCalendarProvider *provider;
};

typedef struct RuntimeScanContext {
    InfiltratrTemporalResourceKind kind;
    InfiltratrTemporalRuntimeVisitor visitor;
    void *user_data;
    size_t emitted;
} RuntimeScanContext;

typedef struct RuntimeCalendarEntry {
    char *id;
    char *name;
    InfiltratrTemporalCalendarInfo info;
    InfiltratrTemporalCalendarModule *module;
    const InfiltratrTemporalCalendarProvider *provider;
} RuntimeCalendarEntry;

typedef struct RuntimeClockEntry {
    char *id;
    char *name;
    InfiltratrTemporalClockModeInfo info;
} RuntimeClockEntry;

static RuntimeCalendarEntry *runtime_calendars;
static size_t runtime_calendar_count;
static size_t runtime_calendar_capacity;
static RuntimeClockEntry *runtime_clocks;
static size_t runtime_clock_count;
static size_t runtime_clock_capacity;

#ifdef _WIN32
static volatile LONG catalogue_state;
#else
static int catalogue_state;
#endif

enum {
    CATALOGUE_UNINITIALISED = 0,
    CATALOGUE_INITIALISING = 1,
    CATALOGUE_READY = 2
};

static bool catalogue_is_ready(void)
{
#ifdef _WIN32
    return InterlockedCompareExchange(
        &catalogue_state, CATALOGUE_READY, CATALOGUE_READY) ==
        CATALOGUE_READY;
#else
    return __atomic_load_n(&catalogue_state, __ATOMIC_ACQUIRE) ==
        CATALOGUE_READY;
#endif
}

static bool catalogue_try_begin_initialisation(void)
{
#ifdef _WIN32
    return InterlockedCompareExchange(
        &catalogue_state,
        CATALOGUE_INITIALISING,
        CATALOGUE_UNINITIALISED) == CATALOGUE_UNINITIALISED;
#else
    int expected = CATALOGUE_UNINITIALISED;
    return __atomic_compare_exchange_n(
        &catalogue_state,
        &expected,
        CATALOGUE_INITIALISING,
        false,
        __ATOMIC_ACQ_REL,
        __ATOMIC_ACQUIRE);
#endif
}

static void catalogue_mark_ready(void)
{
#ifdef _WIN32
    (void)InterlockedExchange(&catalogue_state, CATALOGUE_READY);
#else
    __atomic_store_n(&catalogue_state, CATALOGUE_READY, __ATOMIC_RELEASE);
#endif
}

static void catalogue_wait_until_ready(void)
{
    while (!catalogue_is_ready()) {
#ifdef _WIN32
        (void)SwitchToThread();
#else
        __asm__ __volatile__("" ::: "memory");
#endif
    }
}

static char *copy_text(const char *text)
{
    size_t length;
    char *copy;

    if (text == NULL) return NULL;
    length = strlen(text);
    if (length == SIZE_MAX) return NULL;
    copy = malloc(length + 1U);
    if (copy == NULL) return NULL;
    memcpy(copy, text, length + 1U);
    return copy;
}

static bool resource_id_valid(const char *id)
{
    size_t length = 0U;

    if (id == NULL || id[0] == '\0') return false;
    while (id[length] != '\0') {
        const unsigned char value = (unsigned char)id[length];
        const bool valid =
            (value >= (unsigned char)'a' && value <= (unsigned char)'z') ||
            (value >= (unsigned char)'0' && value <= (unsigned char)'9') ||
            (value == (unsigned char)'-' && length != 0U);
        if (!valid || length + 1U >= INFILTRATR_TEMPORAL_ID_CAPACITY) {
            return false;
        }
        length++;
    }
    return length != 0U && id[length - 1U] != '-';
}

static bool metadata_valid(
    const InfiltratrTemporalResourceMetadata *metadata,
    InfiltratrTemporalResourceKind expected_kind)
{
    if (metadata == NULL ||
        metadata->struct_size < sizeof(*metadata) ||
        metadata->abi_version != INFILTRATR_TEMPORAL_RESOURCE_METADATA_ABI ||
        metadata->kind != expected_kind ||
        !resource_id_valid(metadata->id) ||
        metadata->name == NULL || metadata->name[0] == '\0') {
        return false;
    }
    return strlen(metadata->name) <= 255U;
}

static bool calendar_provider_valid(
    const InfiltratrTemporalCalendarProvider *provider,
    const InfiltratrTemporalResourceMetadata *metadata)
{
    return provider != NULL && metadata != NULL &&
        provider->struct_size >= sizeof(*provider) &&
        provider->abi_version == INFILTRATR_TEMPORAL_CALENDAR_PROVIDER_ABI &&
        resource_id_valid(provider->calendar_id) &&
        strcmp(provider->calendar_id, metadata->id) == 0 &&
        provider->fields_from_jdn != NULL &&
        provider->format_jdn != NULL &&
        provider->format_civil != NULL &&
        provider->period_start != NULL &&
        provider->add_periods != NULL &&
        provider->add_years != NULL;
}

InfiltratrTemporalCalendarModule *
infiltratr_temporal_calendar_module_open(const char *module_path)
{
    InfiltratrTemporalCalendarModule *module;
    InfiltratrTemporalResourceMetadataEntry metadata_entry = NULL;
    InfiltratrTemporalCalendarProviderEntry provider_entry = NULL;

    if (module_path == NULL || module_path[0] == '\0') return NULL;
    module = calloc(1U, sizeof(*module));
    if (module == NULL) return NULL;
    module->library = (InfiltratrDynlib)INFILTRATR_DYNLIB_INIT;

    if (!infiltratr_dynlib_open(&module->library, module_path) ||
        !infiltratr_dynlib_symbol(
            &module->library,
            INFILTRATR_TEMPORAL_RESOURCE_METADATA_SYMBOL,
            &metadata_entry,
            sizeof(metadata_entry)) ||
        metadata_entry == NULL ||
        !infiltratr_dynlib_symbol(
            &module->library,
            INFILTRATR_TEMPORAL_CALENDAR_PROVIDER_SYMBOL,
            &provider_entry,
            sizeof(provider_entry)) ||
        provider_entry == NULL) {
        infiltratr_temporal_calendar_module_close(module);
        return NULL;
    }

    module->metadata = metadata_entry();
    module->provider = provider_entry();
    if (!metadata_valid(
            module->metadata, INFILTRATR_TEMPORAL_RESOURCE_CALENDAR) ||
        !calendar_provider_valid(module->provider, module->metadata)) {
        infiltratr_temporal_calendar_module_close(module);
        return NULL;
    }
    return module;
}

void infiltratr_temporal_calendar_module_close(
    InfiltratrTemporalCalendarModule *module)
{
    if (module == NULL) return;
    infiltratr_dynlib_close(&module->library);
    free(module);
}

const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_calendar_module_metadata(
    const InfiltratrTemporalCalendarModule *module)
{
    return module != NULL ? module->metadata : NULL;
}

const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_calendar_module_provider(
    const InfiltratrTemporalCalendarModule *module)
{
    return module != NULL ? module->provider : NULL;
}

static bool inspect_candidate(
    const InfiltratrTemporalResourceInfo *resource,
    void *user_data)
{
    RuntimeScanContext *context = user_data;
    InfiltratrDynlib library = INFILTRATR_DYNLIB_INIT;
    InfiltratrTemporalResourceMetadataEntry entry = NULL;
    const InfiltratrTemporalResourceMetadata *metadata;
    bool keep_scanning = true;

    if (resource == NULL || context == NULL ||
        resource->form != INFILTRATR_TEMPORAL_RESOURCE_FILE) {
        return true;
    }
    if (!infiltratr_dynlib_open(&library, resource->path)) return true;
    if (!infiltratr_dynlib_symbol(
            &library,
            INFILTRATR_TEMPORAL_RESOURCE_METADATA_SYMBOL,
            &entry,
            sizeof(entry)) ||
        entry == NULL) {
        infiltratr_dynlib_close(&library);
        return true;
    }

    metadata = entry();
    if (metadata_valid(metadata, context->kind)) {
        if (context->visitor != NULL) {
            keep_scanning = context->visitor(
                metadata, resource->path, context->user_data);
        }
        if (keep_scanning) context->emitted++;
    }

    infiltratr_dynlib_close(&library);
    return keep_scanning;
}

bool infiltratr_temporal_runtime_scan_directory(
    const char *directory,
    InfiltratrTemporalResourceKind kind,
    InfiltratrTemporalRuntimeVisitor visitor,
    void *user_data,
    size_t *visited)
{
    RuntimeScanContext context;
    size_t candidates = 0U;

    if (kind != INFILTRATR_TEMPORAL_RESOURCE_CALENDAR &&
        kind != INFILTRATR_TEMPORAL_RESOURCE_CLOCK) {
        return false;
    }

    context.kind = kind;
    context.visitor = visitor;
    context.user_data = user_data;
    context.emitted = 0U;
    if (!infiltratr_temporal_resource_scan_directory(
            directory, kind, inspect_candidate, &context, &candidates)) {
        return false;
    }
    (void)candidates;
    if (visited != NULL) *visited = context.emitted;
    return true;
}

bool infiltratr_temporal_runtime_scan_native(
    InfiltratrTemporalResourceKind kind,
    InfiltratrTemporalRuntimeVisitor visitor,
    void *user_data,
    size_t *visited)
{
    const char *directory;

    if (kind == INFILTRATR_TEMPORAL_RESOURCE_CALENDAR) {
        directory = INFILTRATR_TEMPORAL_NATIVE_CALENDAR_DIRECTORY;
    } else if (kind == INFILTRATR_TEMPORAL_RESOURCE_CLOCK) {
        directory = INFILTRATR_TEMPORAL_NATIVE_CLOCK_DIRECTORY;
    } else {
        return false;
    }
    return infiltratr_temporal_runtime_scan_directory(
        directory, kind, visitor, user_data, visited);
}

static bool native_calendar_duplicate(const char *id)
{
    size_t index;

    for (index = 0U; index < runtime_calendar_count; ++index) {
        if (strcmp(runtime_calendars[index].info.id, id) == 0) return true;
    }
    return false;
}

static bool clock_duplicate(const char *id)
{
    size_t index;

    if (infiltratr_temporal_clock_mode_find(id) != NULL) return true;
    for (index = 0U; index < runtime_clock_count; ++index) {
        if (strcmp(runtime_clocks[index].info.id, id) == 0) return true;
    }
    return false;
}

static bool append_calendar(
    const InfiltratrTemporalResourceMetadata *metadata,
    const char *module_path,
    void *user_data)
{
    RuntimeCalendarEntry *grown;
    RuntimeCalendarEntry *entry;
    InfiltratrTemporalCalendarModule *module;
    const InfiltratrTemporalResourceMetadata *validated_metadata;
    size_t capacity;
    char *id;
    char *name;

    (void)metadata;
    (void)user_data;
    module = infiltratr_temporal_calendar_module_open(module_path);
    if (module == NULL) return true;
    validated_metadata = infiltratr_temporal_calendar_module_metadata(module);
    if (validated_metadata == NULL ||
        native_calendar_duplicate(validated_metadata->id)) {
        infiltratr_temporal_calendar_module_close(module);
        return true;
    }

    if (runtime_calendar_count == runtime_calendar_capacity) {
        capacity = runtime_calendar_capacity == 0U
            ? 8U : runtime_calendar_capacity * 2U;
        if (capacity < runtime_calendar_capacity ||
            capacity > SIZE_MAX / sizeof(*grown)) {
            infiltratr_temporal_calendar_module_close(module);
            return false;
        }
        grown = realloc(runtime_calendars, capacity * sizeof(*grown));
        if (grown == NULL) {
            infiltratr_temporal_calendar_module_close(module);
            return false;
        }
        runtime_calendars = grown;
        runtime_calendar_capacity = capacity;
    }

    id = copy_text(validated_metadata->id);
    name = copy_text(validated_metadata->name);
    if (id == NULL || name == NULL) {
        free(id);
        free(name);
        infiltratr_temporal_calendar_module_close(module);
        return false;
    }
    entry = &runtime_calendars[runtime_calendar_count++];
    entry->id = id;
    entry->name = name;
    entry->info.id = entry->id;
    entry->info.name = entry->name;
    entry->module = module;
    entry->provider = infiltratr_temporal_calendar_module_provider(module);
    return true;
}

static bool append_clock(
    const InfiltratrTemporalResourceMetadata *metadata,
    const char *module_path,
    void *user_data)
{
    RuntimeClockEntry *grown;
    RuntimeClockEntry *entry;
    size_t capacity;
    char *id;
    char *name;

    (void)module_path;
    (void)user_data;
    if (clock_duplicate(metadata->id)) return true;

    if (runtime_clock_count == runtime_clock_capacity) {
        capacity = runtime_clock_capacity == 0U
            ? 8U : runtime_clock_capacity * 2U;
        if (capacity < runtime_clock_capacity ||
            capacity > SIZE_MAX / sizeof(*grown)) return false;
        grown = realloc(runtime_clocks, capacity * sizeof(*grown));
        if (grown == NULL) return false;
        runtime_clocks = grown;
        runtime_clock_capacity = capacity;
    }

    id = copy_text(metadata->id);
    name = copy_text(metadata->name);
    if (id == NULL || name == NULL) {
        free(id);
        free(name);
        return false;
    }
    entry = &runtime_clocks[runtime_clock_count++];
    entry->id = id;
    entry->name = name;
    entry->info.id = entry->id;
    entry->info.name = entry->name;
    entry->info.supports_seconds = metadata->supports_seconds;
    entry->info.requires_latitude = metadata->requires_latitude;
    entry->info.requires_longitude = metadata->requires_longitude;
    return true;
}

static void clear_runtime_calendars(void)
{
    size_t index;
    for (index = 0U; index < runtime_calendar_count; ++index) {
        free(runtime_calendars[index].id);
        free(runtime_calendars[index].name);
        infiltratr_temporal_calendar_module_close(
            runtime_calendars[index].module);
    }
    free(runtime_calendars);
    runtime_calendars = NULL;
    runtime_calendar_count = 0U;
    runtime_calendar_capacity = 0U;
}

static void clear_runtime_clocks(void)
{
    size_t index;
    for (index = 0U; index < runtime_clock_count; ++index) {
        free(runtime_clocks[index].id);
        free(runtime_clocks[index].name);
    }
    free(runtime_clocks);
    runtime_clocks = NULL;
    runtime_clock_count = 0U;
    runtime_clock_capacity = 0U;
}

static void catalogue_initialise(void)
{
    size_t ignored = 0U;

    if (catalogue_is_ready()) return;
    if (!catalogue_try_begin_initialisation()) {
        catalogue_wait_until_ready();
        return;
    }

    if (!infiltratr_temporal_runtime_scan_native(
            INFILTRATR_TEMPORAL_RESOURCE_CALENDAR,
            append_calendar, NULL, &ignored)) {
        clear_runtime_calendars();
    }
    if (!infiltratr_temporal_runtime_scan_native(
            INFILTRATR_TEMPORAL_RESOURCE_CLOCK,
            append_clock, NULL, &ignored)) {
        clear_runtime_clocks();
    }
    catalogue_mark_ready();
}

static bool runtime_calendar_metadata_visible(size_t index)
{
    return index < runtime_calendar_count &&
        infiltratr_temporal_calendar_find(runtime_calendars[index].info.id) == NULL;
}

size_t infiltratr_temporal_installed_calendar_provider_count(void)
{
    catalogue_initialise();
    return runtime_calendar_count;
}

const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_installed_calendar_provider_at(size_t index)
{
    catalogue_initialise();
    return index < runtime_calendar_count
        ? runtime_calendars[index].provider : NULL;
}

const InfiltratrTemporalCalendarProvider *
infiltratr_temporal_installed_calendar_provider_find(const char *id)
{
    size_t index;

    if (id == NULL) return NULL;
    catalogue_initialise();
    for (index = 0U; index < runtime_calendar_count; ++index) {
        if (strcmp(runtime_calendars[index].info.id, id) == 0) {
            return runtime_calendars[index].provider;
        }
    }
    return NULL;
}

size_t infiltratr_temporal_installed_calendar_count(void)
{
    size_t count;
    size_t index;

    catalogue_initialise();
    count = infiltratr_temporal_calendar_count();
    for (index = 0U; index < runtime_calendar_count; ++index) {
        if (runtime_calendar_metadata_visible(index)) count++;
    }
    return count;
}

const InfiltratrTemporalCalendarInfo *
infiltratr_temporal_installed_calendar_at(size_t index)
{
    const size_t builtin_count = infiltratr_temporal_calendar_count();
    size_t runtime_index;

    catalogue_initialise();
    if (index < builtin_count) return infiltratr_temporal_calendar_at(index);
    index -= builtin_count;
    for (runtime_index = 0U; runtime_index < runtime_calendar_count;
         ++runtime_index) {
        if (!runtime_calendar_metadata_visible(runtime_index)) continue;
        if (index == 0U) return &runtime_calendars[runtime_index].info;
        index--;
    }
    return NULL;
}

const InfiltratrTemporalCalendarInfo *
infiltratr_temporal_installed_calendar_find(const char *id)
{
    const InfiltratrTemporalCalendarInfo *builtin;
    size_t index;

    if (id == NULL) return NULL;
    builtin = infiltratr_temporal_calendar_find(id);
    if (builtin != NULL) return builtin;
    catalogue_initialise();
    for (index = 0U; index < runtime_calendar_count; ++index) {
        if (strcmp(runtime_calendars[index].info.id, id) == 0) {
            return &runtime_calendars[index].info;
        }
    }
    return NULL;
}

size_t infiltratr_temporal_installed_clock_mode_count(void)
{
    catalogue_initialise();
    return infiltratr_temporal_clock_mode_count() + runtime_clock_count;
}

const InfiltratrTemporalClockModeInfo *
infiltratr_temporal_installed_clock_mode_at(size_t index)
{
    const size_t builtin_count = infiltratr_temporal_clock_mode_count();

    catalogue_initialise();
    if (index < builtin_count) return infiltratr_temporal_clock_mode_at(index);
    index -= builtin_count;
    if (index >= runtime_clock_count) return NULL;
    return &runtime_clocks[index].info;
}

const InfiltratrTemporalClockModeInfo *
infiltratr_temporal_installed_clock_mode_find(const char *id)
{
    const InfiltratrTemporalClockModeInfo *builtin;
    size_t index;

    if (id == NULL) return NULL;
    builtin = infiltratr_temporal_clock_mode_find(id);
    if (builtin != NULL) return builtin;
    catalogue_initialise();
    for (index = 0U; index < runtime_clock_count; ++index) {
        if (strcmp(runtime_clocks[index].info.id, id) == 0) {
            return &runtime_clocks[index].info;
        }
    }
    return NULL;
}
