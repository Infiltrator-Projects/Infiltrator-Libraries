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

#ifdef __cplusplus
}
#endif

#endif
