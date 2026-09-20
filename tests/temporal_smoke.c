// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/temporal.h"

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

static void test_policy_v2(void)
{
    InfiltratrTemporalPolicyV2 policy;
    InfiltratrTemporalPolicyV2 parsed;
    char text[1024];
    size_t length = 0U;
    size_t index;

    CHECK(infiltratr_temporal_clock_mode_count() == 21U);
    CHECK(infiltratr_temporal_calendar_count() == 31U);
    CHECK(infiltratr_temporal_policy_v2_default(&policy));
    CHECK(strcmp(policy.clock_mode, "standard") == 0);
    CHECK(strcmp(policy.primary_calendar, "gregorian") == 0);
    CHECK(strcmp(policy.secondary_calendar, "none") == 0);
    CHECK(!policy.location_configured);

    for (index = 0U; index < infiltratr_temporal_clock_mode_count(); ++index) {
        const InfiltratrTemporalClockModeInfo *info =
            infiltratr_temporal_clock_mode_at(index);
        CHECK(info != NULL);
        CHECK(info->id != NULL && info->id[0] != '\0');
        CHECK(info->name != NULL && info->name[0] != '\0');
        CHECK(infiltratr_temporal_clock_mode_find(info->id) == info);
        CHECK(strcmp(info->id, "system") != 0);
    }

    for (index = 0U; index < infiltratr_temporal_calendar_count(); ++index) {
        const InfiltratrTemporalCalendarInfo *info =
            infiltratr_temporal_calendar_at(index);
        CHECK(info != NULL);
        CHECK(info->id != NULL && info->id[0] != '\0');
        CHECK(info->name != NULL && info->name[0] != '\0');
        CHECK(infiltratr_temporal_calendar_find(info->id) == info);
    }

    strcpy(policy.clock_mode, "roman-temporal");
    strcpy(policy.primary_calendar, "egyptian-nabonassar");
    strcpy(policy.secondary_calendar, "gregorian");
    policy.show_seconds = true;
    policy.location_configured = true;
    policy.latitude = -36.390000;
    policy.longitude = 145.360000;

    CHECK(infiltratr_temporal_policy_v2_serialize(
        &policy, text, sizeof(text), &length));
    CHECK(length == strlen(text));
    CHECK(strstr(text, "version=2\n") != NULL);
    CHECK(strstr(text, "clock-mode=roman-temporal\n") != NULL);
    CHECK(strstr(text, "primary-calendar=egyptian-nabonassar\n") != NULL);
    CHECK(strstr(text, "secondary-calendar=gregorian\n") != NULL);
    CHECK(infiltratr_temporal_policy_v2_parse(text, &parsed));
    CHECK(strcmp(parsed.clock_mode, "roman-temporal") == 0);
    CHECK(strcmp(parsed.primary_calendar, "egyptian-nabonassar") == 0);
    CHECK(strcmp(parsed.secondary_calendar, "gregorian") == 0);
    CHECK(parsed.show_seconds);
    CHECK(parsed.location_configured);
    CHECK(parsed.latitude == -36.39);
    CHECK(parsed.longitude == 145.36);

    CHECK(infiltratr_temporal_policy_v2_parse(
        "version=1\nclock-profile=decimal-10\nshow-seconds=true\n",
        &parsed));
    CHECK(strcmp(parsed.clock_mode, "decimal") == 0);
    CHECK(strcmp(parsed.primary_calendar, "gregorian") == 0);
    CHECK(parsed.show_seconds);

    CHECK(!infiltratr_temporal_policy_v2_parse(
        "version=2\nclock-mode=missing\n", &parsed));
    CHECK(!infiltratr_temporal_policy_v2_parse(
        "version=2\nclock-mode=standard\nprimary-calendar=none\n", &parsed));
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

int main(void)
{
    test_catalogue();
    test_policy_round_trip();
    test_policy_v2();
    test_policy_v3();
    test_clock_formats();
    return 0;
}
