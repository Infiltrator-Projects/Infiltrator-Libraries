// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file temporal.c
 * @brief Portable temporal presentation policy implementation.
 */
#include "infiltratr/temporal.h"

#include "infiltratr/arithmetic.h"
#include "infiltratr/config.h"
#include "infiltratr/timing.h"

#include <stdio.h>
#include <string.h>

#define MICROSECONDS_PER_SECOND INT64_C(1000000)
#define SECONDS_PER_DAY INT64_C(86400)
#define MICROSECONDS_PER_DAY (SECONDS_PER_DAY * MICROSECONDS_PER_SECOND)
#define DECIMAL_SECONDS_PER_DAY UINT64_C(100000)

typedef struct ClockProfileEntry {
    InfiltratrClockProfile profile;
    const char *id;
    const char *name;
} ClockProfileEntry;

static const ClockProfileEntry clock_profiles[] = {
    { INFILTRATR_CLOCK_PROFILE_SYSTEM, "system", "Follow system" },
    { INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_12, "conventional-12", "12-hour time" },
    { INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_24, "conventional-24", "24-hour time" },
    { INFILTRATR_CLOCK_PROFILE_DECIMAL_10, "decimal-10", "Decimal time (10-hour day)" }
};

static bool profile_valid(InfiltratrClockProfile profile)
{
    return profile >= INFILTRATR_CLOCK_PROFILE_SYSTEM &&
           profile <= INFILTRATR_CLOCK_PROFILE_DECIMAL_10;
}

static int64_t positive_phase(int64_t value, int64_t period)
{
    int64_t remainder = 0;
    if (!infiltratr_i64_floor_divmod(value, period, NULL, &remainder)) {
        return 0;
    }
    return remainder;
}

static int64_t local_microseconds_of_day(int64_t unix_microseconds,
                                         int32_t utc_offset_seconds)
{
    const int64_t instant_phase =
        positive_phase(unix_microseconds, MICROSECONDS_PER_DAY);
    const int64_t offset_microseconds =
        (int64_t)utc_offset_seconds * MICROSECONDS_PER_SECOND;
    const int64_t offset_phase =
        positive_phase(offset_microseconds, MICROSECONDS_PER_DAY);
    return positive_phase(instant_phase + offset_phase, MICROSECONDS_PER_DAY);
}

bool infiltratr_temporal_policy_default(InfiltratrTemporalPolicy *policy)
{
    if (policy == NULL) {
        return false;
    }
    policy->struct_size = (uint32_t)sizeof(*policy);
    policy->version = INFILTRATR_TEMPORAL_POLICY_VERSION;
    policy->clock_profile = INFILTRATR_CLOCK_PROFILE_SYSTEM;
    policy->show_seconds = false;
    return true;
}

bool infiltratr_clock_profile_from_id(const char *id,
                                      InfiltratrClockProfile *profile)
{
    size_t index;
    if (id == NULL || profile == NULL) {
        return false;
    }
    for (index = 0U; index < sizeof(clock_profiles) / sizeof(clock_profiles[0]);
         ++index) {
        if (strcmp(id, clock_profiles[index].id) == 0) {
            *profile = clock_profiles[index].profile;
            return true;
        }
    }
    return false;
}

const char *infiltratr_clock_profile_id(InfiltratrClockProfile profile)
{
    size_t index;
    for (index = 0U; index < sizeof(clock_profiles) / sizeof(clock_profiles[0]);
         ++index) {
        if (clock_profiles[index].profile == profile) {
            return clock_profiles[index].id;
        }
    }
    return NULL;
}

const char *infiltratr_clock_profile_name(InfiltratrClockProfile profile)
{
    size_t index;
    for (index = 0U; index < sizeof(clock_profiles) / sizeof(clock_profiles[0]);
         ++index) {
        if (clock_profiles[index].profile == profile) {
            return clock_profiles[index].name;
        }
    }
    return NULL;
}

size_t infiltratr_clock_profile_count(void)
{
    return sizeof(clock_profiles) / sizeof(clock_profiles[0]);
}

bool infiltratr_clock_profile_at(size_t index, InfiltratrClockProfile *profile)
{
    if (profile == NULL || index >= infiltratr_clock_profile_count()) {
        return false;
    }
    *profile = clock_profiles[index].profile;
    return true;
}

bool infiltratr_temporal_policy_parse(const char *text,
                                      InfiltratrTemporalPolicy *policy)
{
    InfiltratrTemporalPolicy parsed;
    const char *cursor;
    bool saw_version = false;

    if (text == NULL || policy == NULL ||
        !infiltratr_temporal_policy_default(&parsed)) {
        return false;
    }

    cursor = text;
    while (*cursor != '\0') {
        const char *newline = strchr(cursor, '\n');
        const size_t line_length =
            newline != NULL ? (size_t)(newline - cursor) : strlen(cursor);
        char line[256];
        char *key = NULL;
        char *value = NULL;
        InfiltratrConfigLineStatus status;

        if (line_length >= sizeof(line)) {
            return false;
        }
        memcpy(line, cursor, line_length);
        line[line_length] = '\0';

        status = infiltratr_config_parse_line(line, &key, &value);
        if (status == INFILTRATR_CONFIG_LINE_INVALID) {
            return false;
        }
        if (status == INFILTRATR_CONFIG_LINE_ENTRY) {
            if (strcmp(key, "version") == 0) {
                if (strcmp(value, "1") != 0) {
                    return false;
                }
                saw_version = true;
            } else if (strcmp(key, "clock-profile") == 0) {
                InfiltratrClockProfile profile;
                if (!infiltratr_clock_profile_from_id(value, &profile)) {
                    return false;
                }
                parsed.clock_profile = profile;
            } else if (strcmp(key, "show-seconds") == 0) {
                bool show_seconds;
                if (!infiltratr_config_parse_bool(value, &show_seconds)) {
                    return false;
                }
                parsed.show_seconds = show_seconds;
            }
        }

        if (newline == NULL) {
            break;
        }
        cursor = newline + 1;
    }

    if (!saw_version) {
        return false;
    }
    *policy = parsed;
    return true;
}

bool infiltratr_temporal_policy_serialize(const InfiltratrTemporalPolicy *policy,
                                          char *buffer,
                                          size_t capacity,
                                          size_t *length)
{
    char temporary[256];
    const char *profile_id;
    int written;

    if (policy == NULL || buffer == NULL ||
        policy->struct_size < sizeof(*policy) ||
        policy->version != INFILTRATR_TEMPORAL_POLICY_VERSION ||
        !profile_valid(policy->clock_profile)) {
        return false;
    }

    profile_id = infiltratr_clock_profile_id(policy->clock_profile);
    if (profile_id == NULL) {
        return false;
    }

    written = snprintf(temporary, sizeof(temporary),
                       "version=1\nclock-profile=%s\nshow-seconds=%s\n",
                       profile_id,
                       policy->show_seconds ? "true" : "false");
    if (written < 0 || (size_t)written >= sizeof(temporary) ||
        capacity <= (size_t)written) {
        return false;
    }

    memcpy(buffer, temporary, (size_t)written + 1U);
    if (length != NULL) {
        *length = (size_t)written;
    }
    return true;
}

bool infiltratr_temporal_format_clock(InfiltratrClockProfile profile,
                                      int64_t unix_microseconds,
                                      int32_t utc_offset_seconds,
                                      bool show_seconds,
                                      char *buffer,
                                      size_t capacity,
                                      size_t *length)
{
    char temporary[64];
    const int64_t local =
        local_microseconds_of_day(unix_microseconds, utc_offset_seconds);
    int written = -1;

    if (buffer == NULL || capacity == 0U || !profile_valid(profile) ||
        profile == INFILTRATR_CLOCK_PROFILE_SYSTEM) {
        return false;
    }

    if (profile == INFILTRATR_CLOCK_PROFILE_DECIMAL_10) {
        uint64_t ticks = 0U;
        const uint64_t ticks_per_day =
            show_seconds ? DECIMAL_SECONDS_PER_DAY : UINT64_C(1000);

        if (!infiltratr_cycle_partition_u64((uint64_t)local,
                                            (uint64_t)MICROSECONDS_PER_DAY,
                                            ticks_per_day,
                                            &ticks,
                                            NULL)) {
            return false;
        }
        if (show_seconds) {
            written = snprintf(temporary, sizeof(temporary),
                               "%llu:%02llu:%02llu",
                               (unsigned long long)(ticks / UINT64_C(10000)),
                               (unsigned long long)((ticks / UINT64_C(100)) % UINT64_C(100)),
                               (unsigned long long)(ticks % UINT64_C(100)));
        } else {
            written = snprintf(temporary, sizeof(temporary),
                               "%llu:%02llu",
                               (unsigned long long)(ticks / UINT64_C(100)),
                               (unsigned long long)(ticks % UINT64_C(100)));
        }
    } else {
        const int64_t whole_seconds = local / MICROSECONDS_PER_SECOND;
        const int hour24 = (int)(whole_seconds / INT64_C(3600));
        const int minute = (int)((whole_seconds / INT64_C(60)) % INT64_C(60));
        const int second = (int)(whole_seconds % INT64_C(60));

        if (profile == INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_24) {
            written = show_seconds
                ? snprintf(temporary, sizeof(temporary), "%02d:%02d:%02d",
                           hour24, minute, second)
                : snprintf(temporary, sizeof(temporary), "%02d:%02d",
                           hour24, minute);
        } else {
            const int hour12 = hour24 % 12 == 0 ? 12 : hour24 % 12;
            const char *suffix = hour24 < 12 ? "AM" : "PM";
            written = show_seconds
                ? snprintf(temporary, sizeof(temporary), "%d:%02d:%02d %s",
                           hour12, minute, second, suffix)
                : snprintf(temporary, sizeof(temporary), "%d:%02d %s",
                           hour12, minute, suffix);
        }
    }

    if (written < 0 || (size_t)written >= sizeof(temporary) ||
        capacity <= (size_t)written) {
        return false;
    }
    memcpy(buffer, temporary, (size_t)written + 1U);
    if (length != NULL) {
        *length = (size_t)written;
    }
    return true;
}
