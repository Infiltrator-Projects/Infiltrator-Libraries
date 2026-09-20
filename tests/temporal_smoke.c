// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/temporal.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

static void test_catalogue(void)
{
    InfiltratrClockProfile profile = INFILTRATR_CLOCK_PROFILE_SYSTEM;
    assert(infiltratr_clock_profile_count() == 4U);
    assert(infiltratr_clock_profile_from_id("decimal-10", &profile));
    assert(profile == INFILTRATR_CLOCK_PROFILE_DECIMAL_10);
    assert(strcmp(infiltratr_clock_profile_id(profile), "decimal-10") == 0);
    assert(strcmp(infiltratr_clock_profile_name(profile),
                  "Decimal time (10-hour day)") == 0);
    assert(!infiltratr_clock_profile_from_id("missing", &profile));
}

static void test_policy_round_trip(void)
{
    InfiltratrTemporalPolicy policy;
    InfiltratrTemporalPolicy parsed;
    char text[256];
    size_t length = 0U;

    assert(infiltratr_temporal_policy_default(&policy));
    policy.clock_profile = INFILTRATR_CLOCK_PROFILE_DECIMAL_10;
    policy.show_seconds = true;
    assert(infiltratr_temporal_policy_serialize(&policy, text, sizeof(text),
                                                &length));
    assert(length == strlen(text));
    assert(infiltratr_temporal_policy_parse(text, &parsed));
    assert(parsed.version == INFILTRATR_TEMPORAL_POLICY_VERSION);
    assert(parsed.clock_profile == INFILTRATR_CLOCK_PROFILE_DECIMAL_10);
    assert(parsed.show_seconds);
    assert(!infiltratr_temporal_policy_parse(
        "version=2\nclock-profile=decimal-10\n", &parsed));
}

static void test_clock_formats(void)
{
    char text[64];

    assert(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_DECIMAL_10,
        INT64_C(0), 0, true, text, sizeof(text), NULL));
    assert(strcmp(text, "0:00:00") == 0);

    assert(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_DECIMAL_10,
        INT64_C(864000), 0, true, text, sizeof(text), NULL));
    assert(strcmp(text, "0:00:01") == 0);

    assert(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_DECIMAL_10,
        INT64_C(43200) * INT64_C(1000000), 0, true,
        text, sizeof(text), NULL));
    assert(strcmp(text, "5:00:00") == 0);

    assert(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_24,
        INT64_C(19) * INT64_C(3600) * INT64_C(1000000) +
            INT64_C(42) * INT64_C(60) * INT64_C(1000000),
        0, false, text, sizeof(text), NULL));
    assert(strcmp(text, "19:42") == 0);

    assert(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_CONVENTIONAL_12,
        INT64_C(19) * INT64_C(3600) * INT64_C(1000000) +
            INT64_C(42) * INT64_C(60) * INT64_C(1000000),
        0, false, text, sizeof(text), NULL));
    assert(strcmp(text, "7:42 PM") == 0);

    assert(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_DECIMAL_10,
        INT64_MIN, INT32_MIN, true, text, sizeof(text), NULL));
    assert(text[0] != '\0');
    assert(infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_DECIMAL_10,
        INT64_MAX, INT32_MAX, true, text, sizeof(text), NULL));
    assert(text[0] != '\0');

    assert(!infiltratr_temporal_format_clock(
        INFILTRATR_CLOCK_PROFILE_SYSTEM,
        0, 0, false, text, sizeof(text), NULL));
}

int main(void)
{
    test_catalogue();
    test_policy_round_trip();
    test_clock_formats();
    return 0;
}
