// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file temporal.c
 * @brief Portable temporal presentation policy implementation.
 */
#include "infiltratr/temporal.h"

#include "infiltratr/arithmetic.h"
#include "infiltratr/config.h"
#include "infiltratr/core.h"
#include "infiltratr/format.h"
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

static const InfiltratrTemporalClockModeInfo temporal_clock_modes[] = {
    { "standard", "Standard time (OS locale)", true, false, false },
    { "standard-24", "Standard time (24-hour)", true, false, false },
    { "standard-12", "Standard time (12-hour)", true, false, false },
    { "internet", "Internet Time (@000 to @999)", true, false, false },
    { "unix", "Unix time (epoch seconds)", false, false, false },
    { "binary", "Binary clock", true, false, false },
    { "hexadecimal", "Hexadecimal time (0000 to FFFF)", false, false, false },
    { "julian", "Astronomical Julian Date (JD)", true, false, false },
    { "modified-julian", "Modified Julian Date (MJD)", true, false, false },
    { "sidereal", "Local sidereal time", true, false, true },
    { "solar", "Apparent solar time", true, false, true },
    { "mean-solar", "Local mean solar time", true, false, true },
    { "decimal", "French Republican decimal time (10-hour day)", true, false, false },
    { "chinese-time", "Traditional Chinese double-hours", false, false, false },
    { "chinese-ke", "Chinese hundred-kè time", false, false, false },
    { "roman-temporal", "Roman temporal time", false, true, true },
    { "japanese-temporal", "Edo Japanese seasonal time", false, true, true },
    { "italian-hours", "Italian hours (from sunset)", true, true, true },
    { "babylonian-hours", "Babylonian hours (from sunrise)", true, true, true },
    { "indian-ghati", "Indian ghaṭī time (from sunrise)", false, true, true },
    { "nuremberg-hours", "Nuremberg hours (sunrise/sunset reset)", true, true, true }
};

static const InfiltratrTemporalCalendarInfo temporal_calendars[] = {
    { "none", "None" },
    { "gregorian", "Gregorian" },
    { "iso-week", "ISO week calendar" },
    { "julian", "Julian" },
    { "revised-julian", "Revised Julian" },
    { "hebrew", "Hebrew" },
    { "islamic-umalqura", "Islamic (Umm al-Qura)" },
    { "islamic-civil", "Islamic (civil/tabular)" },
    { "islamic-tbla", "Islamic (tabular, astronomical epoch)" },
    { "islamic", "Islamic (astronomical approximation)" },
    { "persian", "Persian (Solar Hijri)" },
    { "bahai", "Bahá’í (Badíʿ)" },
    { "buddhist", "Buddhist" },
    { "coptic", "Coptic" },
    { "ethiopian", "Ethiopian" },
    { "ethiopic-amete-alem", "Ethiopic (Amete Alem)" },
    { "chinese", "Chinese traditional" },
    { "dangi", "Dangi (traditional Korean)" },
    { "indian", "Indian National (Saka)" },
    { "japanese", "Japanese imperial era" },
    { "minguo", "Minguo (Republic of China)" },
    { "roman", "Roman" },
    { "byzantine", "Byzantine (Anno Mundi)" },
    { "egyptian-nabonassar", "Egyptian civil (Nabonassar era)" },
    { "armenian-traditional", "Armenian traditional (365-day)" },
    { "mayan", "Mayan Long Count" },
    { "french-republican", "French Republican" },
    { "swedish-historical", "Swedish historical (1700–1753)" },
    { "international-fixed", "International Fixed" },
    { "world", "World Calendar" },
    { "positivist", "Positivist" }
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


static bool copy_temporal_id(char destination[INFILTRATR_TEMPORAL_ID_CAPACITY],
                             const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL) {
        return false;
    }
    length = strlen(source);
    if (length == 0U || length >= INFILTRATR_TEMPORAL_ID_CAPACITY) {
        return false;
    }
    memcpy(destination, source, length + 1U);
    return true;
}

bool infiltratr_temporal_policy_v2_default(InfiltratrTemporalPolicyV2 *policy)
{
    if (policy == NULL) {
        return false;
    }
    memset(policy, 0, sizeof(*policy));
    policy->struct_size = (uint32_t)sizeof(*policy);
    policy->version = INFILTRATR_TEMPORAL_POLICY_V2_VERSION;
    return copy_temporal_id(policy->clock_mode, "standard") &&
           copy_temporal_id(policy->primary_calendar, "gregorian") &&
           copy_temporal_id(policy->secondary_calendar, "none");
}

size_t infiltratr_temporal_clock_mode_count(void)
{
    return sizeof(temporal_clock_modes) / sizeof(temporal_clock_modes[0]);
}

const InfiltratrTemporalClockModeInfo *
infiltratr_temporal_clock_mode_at(size_t index)
{
    return index < infiltratr_temporal_clock_mode_count()
        ? &temporal_clock_modes[index] : NULL;
}

const InfiltratrTemporalClockModeInfo *
infiltratr_temporal_clock_mode_find(const char *id)
{
    size_t index;
    if (id == NULL) {
        return NULL;
    }
    for (index = 0U; index < infiltratr_temporal_clock_mode_count(); ++index) {
        if (strcmp(id, temporal_clock_modes[index].id) == 0) {
            return &temporal_clock_modes[index];
        }
    }
    return NULL;
}

size_t infiltratr_temporal_calendar_count(void)
{
    return sizeof(temporal_calendars) / sizeof(temporal_calendars[0]);
}

const InfiltratrTemporalCalendarInfo *
infiltratr_temporal_calendar_at(size_t index)
{
    return index < infiltratr_temporal_calendar_count()
        ? &temporal_calendars[index] : NULL;
}

const InfiltratrTemporalCalendarInfo *
infiltratr_temporal_calendar_find(const char *id)
{
    size_t index;
    if (id == NULL) {
        return NULL;
    }
    for (index = 0U; index < infiltratr_temporal_calendar_count(); ++index) {
        if (strcmp(id, temporal_calendars[index].id) == 0) {
            return &temporal_calendars[index];
        }
    }
    return NULL;
}

static bool temporal_document_version(const char *text, unsigned int *version)
{
    const char *cursor;
    if (text == NULL || version == NULL) {
        return false;
    }
    cursor = text;
    while (*cursor != '\0') {
        const char *newline = strchr(cursor, '\n');
        size_t length = newline != NULL ? (size_t)(newline - cursor)
                                        : strlen(cursor);
        char line[256];
        char *key = NULL;
        char *value = NULL;
        InfiltratrConfigLineStatus status;
        uint64_t parsed = 0U;

        if (length >= sizeof(line)) {
            return false;
        }
        memcpy(line, cursor, length);
        line[length] = '\0';
        status = infiltratr_config_parse_line(line, &key, &value);
        if (status == INFILTRATR_CONFIG_LINE_INVALID) {
            return false;
        }
        if (status == INFILTRATR_CONFIG_LINE_ENTRY &&
            strcmp(key, "version") == 0) {
            if (!infiltratr_parse_u64_range(value, 10U, 1U, UINT32_MAX,
                                            &parsed)) {
                return false;
            }
            *version = (unsigned int)parsed;
            return true;
        }
        if (newline == NULL) {
            break;
        }
        cursor = newline + 1;
    }
    return false;
}

static bool migrate_v1_policy(const char *text,
                              InfiltratrTemporalPolicyV2 *policy)
{
    InfiltratrTemporalPolicy legacy;
    const char *mode = "standard";

    if (!infiltratr_temporal_policy_parse(text, &legacy) ||
        !infiltratr_temporal_policy_v2_default(policy)) {
        return false;
    }

    switch (legacy.clock_profile) {
    case INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_12:
        mode = "standard-12";
        break;
    case INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_24:
        mode = "standard-24";
        break;
    case INFILTRATR_CLOCK_PROFILE_DECIMAL_10:
        mode = "decimal";
        break;
    case INFILTRATR_CLOCK_PROFILE_SYSTEM:
    default:
        mode = "standard";
        break;
    }

    policy->show_seconds = legacy.show_seconds;
    return copy_temporal_id(policy->clock_mode, mode);
}

bool infiltratr_temporal_policy_v2_parse(const char *text,
                                         InfiltratrTemporalPolicyV2 *policy)
{
    InfiltratrTemporalPolicyV2 parsed;
    const char *cursor;
    unsigned int version = 0U;

    if (text == NULL || policy == NULL ||
        !temporal_document_version(text, &version)) {
        return false;
    }
    if (version == 1U) {
        return migrate_v1_policy(text, policy);
    }
    if (version != INFILTRATR_TEMPORAL_POLICY_V2_VERSION ||
        !infiltratr_temporal_policy_v2_default(&parsed)) {
        return false;
    }

    cursor = text;
    while (*cursor != '\0') {
        const char *newline = strchr(cursor, '\n');
        size_t length = newline != NULL ? (size_t)(newline - cursor)
                                        : strlen(cursor);
        char line[256];
        char *key = NULL;
        char *value = NULL;
        InfiltratrConfigLineStatus status;

        if (length >= sizeof(line)) {
            return false;
        }
        memcpy(line, cursor, length);
        line[length] = '\0';
        status = infiltratr_config_parse_line(line, &key, &value);
        if (status == INFILTRATR_CONFIG_LINE_INVALID) {
            return false;
        }
        if (status == INFILTRATR_CONFIG_LINE_ENTRY) {
            if (strcmp(key, "version") == 0) {
                if (strcmp(value, "2") != 0) {
                    return false;
                }
            } else if (strcmp(key, "clock-mode") == 0) {
                if (infiltratr_temporal_clock_mode_find(value) == NULL ||
                    !copy_temporal_id(parsed.clock_mode, value)) {
                    return false;
                }
            } else if (strcmp(key, "primary-calendar") == 0) {
                if (strcmp(value, "none") == 0 ||
                    infiltratr_temporal_calendar_find(value) == NULL ||
                    !copy_temporal_id(parsed.primary_calendar, value)) {
                    return false;
                }
            } else if (strcmp(key, "secondary-calendar") == 0) {
                if (infiltratr_temporal_calendar_find(value) == NULL ||
                    !copy_temporal_id(parsed.secondary_calendar, value)) {
                    return false;
                }
            } else if (strcmp(key, "show-seconds") == 0) {
                if (!infiltratr_config_parse_bool(value,
                                                  &parsed.show_seconds)) {
                    return false;
                }
            } else if (strcmp(key, "location-configured") == 0) {
                if (!infiltratr_config_parse_bool(value,
                                                  &parsed.location_configured)) {
                    return false;
                }
            } else if (strcmp(key, "latitude") == 0) {
                if (!infiltratr_parse_double_range(value, -90.0, 90.0,
                                                   &parsed.latitude)) {
                    return false;
                }
            } else if (strcmp(key, "longitude") == 0) {
                if (!infiltratr_parse_double_range(value, -180.0, 180.0,
                                                   &parsed.longitude)) {
                    return false;
                }
            }
        }
        if (newline == NULL) {
            break;
        }
        cursor = newline + 1;
    }

    if (infiltratr_temporal_clock_mode_find(parsed.clock_mode) == NULL ||
        infiltratr_temporal_calendar_find(parsed.primary_calendar) == NULL ||
        strcmp(parsed.primary_calendar, "none") == 0 ||
        infiltratr_temporal_calendar_find(parsed.secondary_calendar) == NULL) {
        return false;
    }

    *policy = parsed;
    return true;
}

bool infiltratr_temporal_policy_v2_serialize(
    const InfiltratrTemporalPolicyV2 *policy,
    char *buffer,
    size_t capacity,
    size_t *length)
{
    char latitude[64];
    char longitude[64];
    char temporary[512];
    int written;

    if (policy == NULL || buffer == NULL ||
        policy->struct_size < sizeof(*policy) ||
        policy->version != INFILTRATR_TEMPORAL_POLICY_V2_VERSION ||
        infiltratr_temporal_clock_mode_find(policy->clock_mode) == NULL ||
        infiltratr_temporal_calendar_find(policy->primary_calendar) == NULL ||
        strcmp(policy->primary_calendar, "none") == 0 ||
        infiltratr_temporal_calendar_find(policy->secondary_calendar) == NULL ||
        !infiltratr_format_fixed_ascii(policy->latitude, 6U,
                                       latitude, sizeof(latitude)) ||
        !infiltratr_format_fixed_ascii(policy->longitude, 6U,
                                       longitude, sizeof(longitude))) {
        return false;
    }

    written = snprintf(temporary, sizeof(temporary),
                       "version=2\n"
                       "clock-mode=%s\n"
                       "primary-calendar=%s\n"
                       "secondary-calendar=%s\n"
                       "show-seconds=%s\n"
                       "location-configured=%s\n"
                       "latitude=%s\n"
                       "longitude=%s\n",
                       policy->clock_mode,
                       policy->primary_calendar,
                       policy->secondary_calendar,
                       policy->show_seconds ? "true" : "false",
                       policy->location_configured ? "true" : "false",
                       latitude,
                       longitude);
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


bool infiltratr_temporal_policy_v3_default(InfiltratrTemporalPolicyV3 *policy)
{
    if (policy == NULL) {
        return false;
    }
    memset(policy, 0, sizeof(*policy));
    policy->struct_size = (uint32_t)sizeof(*policy);
    policy->version = INFILTRATR_TEMPORAL_POLICY_V3_VERSION;
    return copy_temporal_id(policy->clock_mode, "standard") &&
           copy_temporal_id(policy->calendar, "gregorian");
}

static bool temporal_policy_v3_from_v2(
    const InfiltratrTemporalPolicyV2 *legacy,
    InfiltratrTemporalPolicyV3 *policy)
{
    if (legacy == NULL || policy == NULL ||
        !infiltratr_temporal_policy_v3_default(policy) ||
        !copy_temporal_id(policy->clock_mode, legacy->clock_mode) ||
        !copy_temporal_id(policy->calendar, legacy->primary_calendar)) {
        return false;
    }
    policy->show_seconds = legacy->show_seconds;
    policy->location_configured = legacy->location_configured;
    policy->latitude = legacy->latitude;
    policy->longitude = legacy->longitude;
    return true;
}

bool infiltratr_temporal_policy_v3_parse(const char *text,
                                         InfiltratrTemporalPolicyV3 *policy)
{
    InfiltratrTemporalPolicyV3 parsed;
    const char *cursor;
    unsigned int version = 0U;

    if (text == NULL || policy == NULL ||
        !temporal_document_version(text, &version)) {
        return false;
    }

    if (version == INFILTRATR_TEMPORAL_POLICY_VERSION ||
        version == INFILTRATR_TEMPORAL_POLICY_V2_VERSION) {
        InfiltratrTemporalPolicyV2 legacy;
        if (!infiltratr_temporal_policy_v2_parse(text, &legacy)) {
            return false;
        }
        return temporal_policy_v3_from_v2(&legacy, policy);
    }

    if (version != INFILTRATR_TEMPORAL_POLICY_V3_VERSION ||
        !infiltratr_temporal_policy_v3_default(&parsed)) {
        return false;
    }

    cursor = text;
    while (*cursor != '\0') {
        const char *newline = strchr(cursor, '\n');
        size_t line_length = newline != NULL
            ? (size_t)(newline - cursor) : strlen(cursor);
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
                if (strcmp(value, "3") != 0) {
                    return false;
                }
            } else if (strcmp(key, "clock-mode") == 0) {
                if (infiltratr_temporal_clock_mode_find(value) == NULL ||
                    !copy_temporal_id(parsed.clock_mode, value)) {
                    return false;
                }
            } else if (strcmp(key, "calendar") == 0) {
                if (strcmp(value, "none") == 0 ||
                    infiltratr_temporal_calendar_find(value) == NULL ||
                    !copy_temporal_id(parsed.calendar, value)) {
                    return false;
                }
            } else if (strcmp(key, "show-seconds") == 0) {
                if (!infiltratr_config_parse_bool(value,
                                                  &parsed.show_seconds)) {
                    return false;
                }
            } else if (strcmp(key, "location-configured") == 0) {
                if (!infiltratr_config_parse_bool(
                        value, &parsed.location_configured)) {
                    return false;
                }
            } else if (strcmp(key, "latitude") == 0) {
                if (!infiltratr_parse_double_range(
                        value, -90.0, 90.0, &parsed.latitude)) {
                    return false;
                }
            } else if (strcmp(key, "longitude") == 0) {
                if (!infiltratr_parse_double_range(
                        value, -180.0, 180.0, &parsed.longitude)) {
                    return false;
                }
            }
        }

        if (newline == NULL) {
            break;
        }
        cursor = newline + 1;
    }

    if (infiltratr_temporal_clock_mode_find(parsed.clock_mode) == NULL ||
        infiltratr_temporal_calendar_find(parsed.calendar) == NULL ||
        strcmp(parsed.calendar, "none") == 0) {
        return false;
    }

    *policy = parsed;
    return true;
}

bool infiltratr_temporal_policy_v3_serialize(
    const InfiltratrTemporalPolicyV3 *policy,
    char *buffer,
    size_t capacity,
    size_t *length)
{
    char latitude[64];
    char longitude[64];
    char temporary[512];
    int written;

    if (policy == NULL || buffer == NULL ||
        policy->struct_size < sizeof(*policy) ||
        policy->version != INFILTRATR_TEMPORAL_POLICY_V3_VERSION ||
        infiltratr_temporal_clock_mode_find(policy->clock_mode) == NULL ||
        infiltratr_temporal_calendar_find(policy->calendar) == NULL ||
        strcmp(policy->calendar, "none") == 0 ||
        !infiltratr_format_fixed_ascii(policy->latitude, 6U,
                                       latitude, sizeof(latitude)) ||
        !infiltratr_format_fixed_ascii(policy->longitude, 6U,
                                       longitude, sizeof(longitude))) {
        return false;
    }

    written = snprintf(temporary, sizeof(temporary),
                       "version=3\n"
                       "clock-mode=%s\n"
                       "calendar=%s\n"
                       "show-seconds=%s\n"
                       "location-configured=%s\n"
                       "latitude=%s\n"
                       "longitude=%s\n",
                       policy->clock_mode,
                       policy->calendar,
                       policy->show_seconds ? "true" : "false",
                       policy->location_configured ? "true" : "false",
                       latitude,
                       longitude);
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
