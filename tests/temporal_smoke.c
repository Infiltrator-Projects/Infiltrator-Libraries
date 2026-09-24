// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/temporal.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) \
    do { \
        if (!(expression)) { \
            fprintf(stderr, "temporal test failed: %s (%s:%d)\n", \
                    #expression, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

static void test_catalogue(void)
{
    InfiltratrClockProfile profile = INFILTRATR_CLOCK_PROFILE_SYSTEM;
    CHECK(infiltratr_clock_profile_count() == 4U);
    CHECK(infiltratr_clock_profile_from_id("decimal-10", &profile));
    CHECK(profile == INFILTRATR_CLOCK_PROFILE_DECIMAL_10);
    CHECK(strcmp(infiltratr_clock_profile_id(profile), "decimal-10") == 0);
    CHECK(strcmp(infiltratr_clock_profile_name(profile),
                  "Decimal time (10-hour day)") == 0);
    CHECK(!infiltratr_clock_profile_from_id("missing", &profile));

    for (size_t index = 0U;
         index < infiltratr_temporal_clock_mode_count(); ++index) {
        const InfiltratrTemporalClockModeInfo *info =
            infiltratr_temporal_clock_mode_at(index);

        CHECK(info != NULL);
        CHECK(info->id != NULL && info->id[0] != '\0');
        CHECK(info->name != NULL && info->name[0] != '\0');
        CHECK(infiltratr_temporal_clock_mode_find(info->id) == info);
        for (size_t other = index + 1U;
             other < infiltratr_temporal_clock_mode_count(); ++other) {
            const InfiltratrTemporalClockModeInfo *candidate =
                infiltratr_temporal_clock_mode_at(other);
            CHECK(candidate != NULL);
            CHECK(strcmp(info->id, candidate->id) != 0);
        }
    }

    for (size_t index = 0U;
         index < infiltratr_temporal_calendar_count(); ++index) {
        const InfiltratrTemporalCalendarInfo *info =
            infiltratr_temporal_calendar_at(index);

        CHECK(info != NULL);
        CHECK(info->id != NULL && info->id[0] != '\0');
        CHECK(info->name != NULL && info->name[0] != '\0');
        CHECK(infiltratr_temporal_calendar_find(info->id) == info);
        for (size_t other = index + 1U;
             other < infiltratr_temporal_calendar_count(); ++other) {
            const InfiltratrTemporalCalendarInfo *candidate =
                infiltratr_temporal_calendar_at(other);
            CHECK(candidate != NULL);
            CHECK(strcmp(info->id, candidate->id) != 0);
        }
    }

    CHECK(infiltratr_temporal_clock_mode_at(
              infiltratr_temporal_clock_mode_count()) == NULL);
    CHECK(infiltratr_temporal_calendar_at(
              infiltratr_temporal_calendar_count()) == NULL);
}

static void test_policy_round_trip(void)
{
    InfiltratrTemporalPolicy policy;
    InfiltratrTemporalPolicy parsed;
    char text[256];
    size_t length = 0U;

    CHECK(infiltratr_temporal_policy_default(&policy));
    policy.clock_profile = INFILTRATR_CLOCK_PROFILE_DECIMAL_10;
    policy.show_seconds = true;
    CHECK(infiltratr_temporal_policy_serialize(&policy, text, sizeof(text),
                                                &length));
    CHECK(length == strlen(text));
    CHECK(infiltratr_temporal_policy_parse(text, &parsed));
    CHECK(parsed.version == INFILTRATR_TEMPORAL_POLICY_VERSION);
    CHECK(parsed.clock_profile == INFILTRATR_CLOCK_PROFILE_DECIMAL_10);
    CHECK(parsed.show_seconds);
    CHECK(!infiltratr_temporal_policy_parse(
        "version=2\nclock-profile=decimal-10\n", &parsed));
}

static void test_policy_v3(void)
{
    InfiltratrTemporalPolicyV3 policy;
    InfiltratrTemporalPolicyV3 parsed;
    char text[1024];
    size_t length = 0U;

    CHECK(infiltratr_temporal_policy_v3_default(&policy));
    CHECK(policy.version == INFILTRATR_TEMPORAL_POLICY_V3_VERSION);
    CHECK(strcmp(policy.clock_mode, "standard") == 0);
    CHECK(strcmp(policy.calendar, "gregorian") == 0);

    strcpy(policy.clock_mode, "decimal");
    strcpy(policy.calendar, "egyptian-nabonassar");
    policy.show_seconds = true;
    policy.location_configured = true;
    policy.latitude = -36.39;
    policy.longitude = 145.36;

    CHECK(infiltratr_temporal_policy_v3_serialize(
        &policy, text, sizeof(text), &length));
    CHECK(length == strlen(text));
    CHECK(strstr(text, "version=3\n") != NULL);
    CHECK(strstr(text, "calendar=egyptian-nabonassar\n") != NULL);
    CHECK(strstr(text, "secondary-calendar=") == NULL);

    CHECK(infiltratr_temporal_policy_v3_parse(text, &parsed));
    CHECK(strcmp(parsed.clock_mode, "decimal") == 0);
    CHECK(strcmp(parsed.calendar, "egyptian-nabonassar") == 0);
    CHECK(parsed.show_seconds);
    CHECK(parsed.location_configured);
    CHECK(parsed.latitude == -36.39);
    CHECK(parsed.longitude == 145.36);

    policy.latitude = 90.000001;
    CHECK(!infiltratr_temporal_policy_v3_serialize(
        &policy, text, sizeof(text), &length));
    policy.latitude = -36.39;
    policy.longitude = 180.000001;
    CHECK(!infiltratr_temporal_policy_v3_serialize(
        &policy, text, sizeof(text), &length));
    policy.longitude = NAN;
    CHECK(!infiltratr_temporal_policy_v3_serialize(
        &policy, text, sizeof(text), &length));
    policy.longitude = 145.36;

    CHECK(infiltratr_temporal_policy_v3_parse(
        "version=2\n"
        "clock-mode=roman-temporal\n"
        "primary-calendar=hebrew\n"
        "secondary-calendar=gregorian\n"
        "show-seconds=true\n"
        "location-configured=false\n"
        "latitude=0.000000\n"
        "longitude=0.000000\n",
        &parsed));
    CHECK(strcmp(parsed.clock_mode, "roman-temporal") == 0);
    CHECK(strcmp(parsed.calendar, "hebrew") == 0);
    CHECK(parsed.show_seconds);

    CHECK(!infiltratr_temporal_policy_v3_parse(
        "version=3\ncalendar=none\n", &parsed));
    CHECK(!infiltratr_temporal_policy_v3_parse(
        "version=3\n"
        "clock-mode=standard\n"
        "calendar=gregorian\n"
        "show-seconds=false\n"
        "location-configured=false\n"
        "latitude=0.000000\n",
        &parsed));
    CHECK(!infiltratr_temporal_policy_v3_parse(
        "version=3\n"
        "clock-mode=standard\n"
        "clock-mode=standard-24\n"
        "calendar=gregorian\n"
        "show-seconds=false\n"
        "location-configured=false\n"
        "latitude=0.000000\n"
        "longitude=0.000000\n",
        &parsed));

    CHECK(infiltratr_temporal_policy_v3_default(&policy));
    memset(policy.clock_mode, 'x', sizeof(policy.clock_mode));
    CHECK(!infiltratr_temporal_policy_v3_serialize(
        &policy, text, sizeof(text), &length));
    CHECK(infiltratr_temporal_policy_v3_default(&policy));
    memset(policy.calendar, 'x', sizeof(policy.calendar));
    CHECK(!infiltratr_temporal_policy_v3_serialize(
        &policy, text, sizeof(text), &length));

    CHECK(infiltratr_temporal_calendar_count() == 30U);
    CHECK(infiltratr_temporal_calendar_find("none") == NULL);
}

static void test_local_microseconds_of_day(void)
{
    CHECK(infiltratr_temporal_local_microseconds_of_day(0, 0) == INT64_C(0));
    CHECK(infiltratr_temporal_local_microseconds_of_day(-1, 0) ==
          INT64_C(86399999999));
    CHECK(infiltratr_temporal_local_microseconds_of_day(0, 3600) ==
          INT64_C(3600000000));
    CHECK(infiltratr_temporal_local_microseconds_of_day(0, -3600) ==
          INT64_C(82800000000));
    CHECK(infiltratr_temporal_local_microseconds_of_day(
              INT64_MIN, INT32_MIN) == INT64_C(60297224192));
    CHECK(infiltratr_temporal_local_microseconds_of_day(
              INT64_MAX, INT32_MAX) == INT64_C(26101775807));
}

static void test_clock_formats(void)
{
    char text[64];

    CHECK(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_DECIMAL_10,
        INT64_C(0), 0, true, text, sizeof(text), NULL));
    CHECK(strcmp(text, "0:00:00") == 0);

    CHECK(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_DECIMAL_10,
        INT64_C(864000), 0, true, text, sizeof(text), NULL));
    CHECK(strcmp(text, "0:00:01") == 0);

    CHECK(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_DECIMAL_10,
        INT64_C(43200) * INT64_C(1000000), 0, true,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "5:00:00") == 0);

    CHECK(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_24,
        INT64_C(19) * INT64_C(3600) * INT64_C(1000000) +
            INT64_C(42) * INT64_C(60) * INT64_C(1000000),
        0, false, text, sizeof(text), NULL));
    CHECK(strcmp(text, "19:42") == 0);

    CHECK(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_12,
        INT64_C(19) * INT64_C(3600) * INT64_C(1000000) +
            INT64_C(42) * INT64_C(60) * INT64_C(1000000),
        0, false, text, sizeof(text), NULL));
    CHECK(strcmp(text, "7:42 PM") == 0);

    CHECK(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_DECIMAL_10,
        INT64_MIN, INT32_MIN, true, text, sizeof(text), NULL));
    CHECK(text[0] != '\0');
    CHECK(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_DECIMAL_10,
        INT64_MAX, INT32_MAX, true, text, sizeof(text), NULL));
    CHECK(text[0] != '\0');

    CHECK(!infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_SYSTEM,
        0, 0, false, text, sizeof(text), NULL));
}


static void test_duration_mode_formats(void)
{
    char text[192];

    CHECK(infiltratr_temporal_format_duration_mode(
        "standard", UINT64_C(3661) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "01:01:01") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "decimal", UINT64_C(86) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "0:00:99") == 0);
    CHECK(infiltratr_temporal_format_duration_mode(
        "decimal", UINT64_C(87) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "0:01:00") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "internet", UINT64_C(87) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "@001.00") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "unix", UINT64_C(61) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "61 s") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "binary", UINT64_C(61) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "00000:000001:000001") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "hexadecimal", UINT64_C(43200) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "8000") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "julian", UINT64_C(43200) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "JD +0.50000 d") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "modified-julian", UINT64_C(43200) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        false, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "MJD +0.500 d") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "sidereal", UINT64_C(43200) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        true, false, true, -36.39, 145.36,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "12:01:58 LST") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "chinese-time", UINT64_C(7200) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        false, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "時辰 01/12") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "chinese-ke", UINT64_C(864) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        false, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "刻 01/100") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "indian-ghati", UINT64_C(1440) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        false, false, true, -36.39, 145.36,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "GH 01:00") == 0);

    CHECK(infiltratr_temporal_format_duration_mode(
        "roman-temporal", UINT64_C(3661) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        false, false, true, -36.39, 145.36,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "01:01") == 0);
    CHECK(infiltratr_temporal_format_duration_mode(
        "japanese-temporal", UINT64_C(3661) * UINT64_C(1000000),
        INT64_C(1789990000) * INT64_C(1000000),
        false, false, true, -36.39, 145.36,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "01:01") == 0);

    for (size_t index = 0U;
         index < infiltratr_temporal_clock_mode_count(); ++index) {
        const InfiltratrTemporalClockModeInfo *info =
            infiltratr_temporal_clock_mode_at(index);
        CHECK(info != NULL);
        CHECK(infiltratr_temporal_format_duration_mode(
            info->id,
            UINT64_C(98765) * UINT64_C(1000000),
            INT64_C(1789990000) * INT64_C(1000000),
            true, false, true, -36.39, 145.36,
            text, sizeof(text), NULL));
        CHECK(text[0] != '\0');
    }

    CHECK(!infiltratr_temporal_format_duration_mode(
        "missing-mode", UINT64_C(1), INT64_C(0),
        true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
}

static void test_clock_mode_formats(void)
{
    char text[192];
    size_t length = 0U;

    CHECK(!infiltratr_temporal_format_clock_mode(
        "standard", INT64_C(0), 0, true, false, false,
        0.0, 0.0, text, sizeof(text), &length));

    CHECK(infiltratr_temporal_format_clock_mode(
        "standard-24",
        INT64_C(19) * INT64_C(3600) * INT64_C(1000000) +
            INT64_C(42) * INT64_C(60) * INT64_C(1000000),
        0, false, false, false, 0.0, 0.0,
        text, sizeof(text), &length));
    CHECK(strcmp(text, "19:42") == 0);
    CHECK(length == strlen(text));

    CHECK(infiltratr_temporal_format_clock_mode(
        "decimal",
        INT64_C(43200) * INT64_C(1000000),
        0, true, false, false, 0.0, 0.0,
        text, sizeof(text), NULL));
    CHECK(strcmp(text, "5:00:00") == 0);

    for (size_t index = 0U;
         index < infiltratr_temporal_clock_mode_count(); ++index) {
        const InfiltratrTemporalClockModeInfo *info =
            infiltratr_temporal_clock_mode_at(index);
        const bool standard =
            info != NULL && strcmp(info->id, "standard") == 0;

        CHECK(info != NULL);
        CHECK(infiltratr_temporal_format_clock_mode(
                  info->id,
                  INT64_C(1789990000) * INT64_C(1000000),
                  36000, true, false, true, -36.39, 145.36,
                  text, sizeof(text), NULL) != standard);
        if (!standard) {
            CHECK(text[0] != '\0');
        }
        if (info->requires_latitude || info->requires_longitude) {
            CHECK(!infiltratr_temporal_format_clock_mode(
                info->id,
                INT64_C(1789990000) * INT64_C(1000000),
                36000, true, false, false, -36.39, 145.36,
                text, sizeof(text), NULL));
        }
    }

    CHECK(infiltratr_temporal_format_clock_mode(
        "roman-temporal",
        INT64_C(1789990000) * INT64_C(1000000),
        36000, false, false, true, -36.39, 145.36,
        text, sizeof(text), NULL));
    CHECK(strncmp(text, "Hora ", 5U) == 0 ||
          strncmp(text, "Vigilia ", 8U) == 0);

    CHECK(!infiltratr_temporal_format_clock_mode(
        "missing-mode", INT64_C(0), 0, false, false, false,
        0.0, 0.0, text, sizeof(text), NULL));
}

int main(void)
{
    test_catalogue();
    test_policy_round_trip();
    test_policy_v3();
    test_local_microseconds_of_day();
    test_clock_formats();
    test_duration_mode_formats();
    test_clock_mode_formats();
    return 0;
}
