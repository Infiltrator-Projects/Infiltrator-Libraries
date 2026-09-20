// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file temporal.h
 * @brief Portable system-wide temporal presentation policy and clock formatting.
 *
 * Canonical timestamps remain owned by applications and operating systems.
 * This API owns only the user's human-facing clock presentation policy.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 1993-2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_TEMPORAL_H
#define INFILTRATR_COMMON_TEMPORAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INFILTRATR_TEMPORAL_POLICY_VERSION 1U
#define INFILTRATR_TEMPORAL_POLICY_V2_VERSION 2U
#define INFILTRATR_TEMPORAL_POLICY_V3_VERSION 3U
#define INFILTRATR_TEMPORAL_ID_CAPACITY 64U

typedef enum InfiltratrClockProfile {
    INFILTRATR_CLOCK_PROFILE_SYSTEM = 0,
    INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_12,
    INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_24,
    INFILTRATR_CLOCK_PROFILE_DECIMAL_10
} InfiltratrClockProfile;

typedef struct InfiltratrTemporalPolicy {
    uint32_t struct_size;
    uint32_t version;
    InfiltratrClockProfile clock_profile;
    bool show_seconds;
} InfiltratrTemporalPolicy;

typedef struct InfiltratrTemporalClockModeInfo {
    const char *id;
    const char *name;
    bool supports_seconds;
    bool requires_latitude;
    bool requires_longitude;
} InfiltratrTemporalClockModeInfo;

typedef struct InfiltratrTemporalCalendarInfo {
    const char *id;
    const char *name;
} InfiltratrTemporalCalendarInfo;

typedef struct InfiltratrTemporalPolicyV2 {
    uint32_t struct_size;
    uint32_t version;
    char clock_mode[INFILTRATR_TEMPORAL_ID_CAPACITY];
    char primary_calendar[INFILTRATR_TEMPORAL_ID_CAPACITY];
    char secondary_calendar[INFILTRATR_TEMPORAL_ID_CAPACITY];
    bool show_seconds;
    bool location_configured;
    double latitude;
    double longitude;
} InfiltratrTemporalPolicyV2;

/*
 * Version 3 is the current system-wide authority.  It deliberately has one
 * calendar only: the retired secondary-calendar concept is preserved solely
 * in the v2 compatibility API and is never written by current consumers.
 */
typedef struct InfiltratrTemporalPolicyV3 {
    uint32_t struct_size;
    uint32_t version;
    char clock_mode[INFILTRATR_TEMPORAL_ID_CAPACITY];
    char calendar[INFILTRATR_TEMPORAL_ID_CAPACITY];
    bool show_seconds;
    bool location_configured;
    double latitude;
    double longitude;
} InfiltratrTemporalPolicyV3;

/** Initialise a policy to conservative system-following defaults. */
bool infiltratr_temporal_policy_default(InfiltratrTemporalPolicy *policy);

/** Parse a stable clock-profile identifier. Caller output is unchanged on failure. */
bool infiltratr_clock_profile_from_id(const char *id,
                                      InfiltratrClockProfile *profile);

/** Return the stable persistence identifier for a profile, or NULL. */
const char *infiltratr_clock_profile_id(InfiltratrClockProfile profile);

/** Return the stable English presentation name for a profile, or NULL. */
const char *infiltratr_clock_profile_name(InfiltratrClockProfile profile);

/** Number of profiles in the portable catalogue. */
size_t infiltratr_clock_profile_count(void);

/** Return a profile by catalogue index; false when out of range. */
bool infiltratr_clock_profile_at(size_t index, InfiltratrClockProfile *profile);

/**
 * Parse a versioned key=value temporal-policy document.
 *
 * Required key: version=1. Optional recognised keys are clock-profile and
 * show-seconds. Unknown keys are ignored for forward-compatible readers.
 * Caller output is unchanged on malformed or unsupported input.
 */
bool infiltratr_temporal_policy_parse(const char *text,
                                      InfiltratrTemporalPolicy *policy);

/**
 * Serialise a temporal policy into deterministic key=value text.
 *
 * On insufficient capacity the destination remains unchanged. @length may be
 * NULL; when provided it receives the byte count excluding the trailing NUL.
 */
bool infiltratr_temporal_policy_serialize(const InfiltratrTemporalPolicy *policy,
                                          char *buffer,
                                          size_t capacity,
                                          size_t *length);

/**
 * Format a local civil clock from a canonical Unix-microsecond instant.
 *
 * Explicit 12-hour, 24-hour and decimal-10 profiles are supported. SYSTEM is
 * deliberately not formatted here because locale/platform policy belongs to
 * the platform adapter. UTC offset is supplied by the caller and includes DST.
 *
 * Decimal time divides one civil day into 10 hours, each 100 minutes, each 100
 * seconds. Formatting uses exact integer/rational partition arithmetic.
 */
bool infiltratr_temporal_format_clock(InfiltratrClockProfile profile,
                                      int64_t unix_microseconds,
                                      int32_t utc_offset_seconds,
                                      bool show_seconds,
                                      char *buffer,
                                      size_t capacity,
                                      size_t *length);

/**
 * Initialise the extensible system-wide temporal policy.
 *
 * The default authority is conventional OS-locale time, Gregorian primary
 * calendar, no secondary calendar, no seconds and no configured location.
 * There is deliberately no "follow system" clock identity in this catalogue:
 * System Settings is the authority. Individual applications may separately
 * offer "Follow System Settings" as an application-local behaviour.
 */
bool infiltratr_temporal_policy_v2_default(InfiltratrTemporalPolicyV2 *policy);

/** Complete system-wide clock catalogue shared by Settings and Calendar. */
size_t infiltratr_temporal_clock_mode_count(void);
const InfiltratrTemporalClockModeInfo *
infiltratr_temporal_clock_mode_at(size_t index);
const InfiltratrTemporalClockModeInfo *
infiltratr_temporal_clock_mode_find(const char *id);

/**
 * Complete calendar-system catalogue.
 *
 * The legacy "none" entry remains at index 0 only for v2 compatibility.
 * Current v3 policy never exposes it as a selectable system calendar.
 */
size_t infiltratr_temporal_calendar_count(void);
const InfiltratrTemporalCalendarInfo *
infiltratr_temporal_calendar_at(size_t index);
const InfiltratrTemporalCalendarInfo *
infiltratr_temporal_calendar_find(const char *id);

/**
 * Parse version-2 temporal policy. Version-1 policy documents are accepted and
 * migrated in memory to equivalent v2 clock identities.
 */
bool infiltratr_temporal_policy_v2_parse(const char *text,
                                         InfiltratrTemporalPolicyV2 *policy);

/** Serialize the complete temporal authority as deterministic version-2 text. */
bool infiltratr_temporal_policy_v2_serialize(
    const InfiltratrTemporalPolicyV2 *policy,
    char *buffer,
    size_t capacity,
    size_t *length);

/** Initialise the current one-calendar system-wide temporal authority. */
bool infiltratr_temporal_policy_v3_default(InfiltratrTemporalPolicyV3 *policy);

/**
 * Parse current temporal policy. Version-1 and version-2 documents are
 * accepted and migrated in memory. A retired v2 secondary-calendar value is
 * intentionally discarded during migration.
 */
bool infiltratr_temporal_policy_v3_parse(const char *text,
                                         InfiltratrTemporalPolicyV3 *policy);

/** Serialize the current authority as deterministic version-3 text. */
bool infiltratr_temporal_policy_v3_serialize(
    const InfiltratrTemporalPolicyV3 *policy,
    char *buffer,
    size_t capacity,
    size_t *length);

#ifdef __cplusplus
}
#endif

#endif
