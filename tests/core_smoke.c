// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file core_smoke.c
 * @brief Smoke tests for Common core and POSIX compatibility helpers.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/core.h"
#include "infiltratr/posix.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char text[64];
    char path[128];
    uint64_t value = 0U;
    int64_t signed_value = 0;
    double decimal = 0.0;
    double rate = 0.0;
    uint64_t sum = 0U;
    uint64_t nanoseconds = 0U;

    infiltratr_copy_string(text, sizeof(text), "  hello world  \n");
    infiltratr_trim(text);
    assert(strcmp(text, "hello world") == 0);
    assert(infiltratr_string_equal("same", "same"));
    assert(!infiltratr_string_equal("same", "different"));
    assert(infiltratr_string_equal(NULL, NULL));
    assert(infiltratr_string_starts_with("calendar-plus", "calendar"));
    assert(!infiltratr_string_starts_with(NULL, "calendar"));
    assert(infiltratr_string_ends_with("calendar-plus", "plus"));
    assert(!infiltratr_string_ends_with("calendar-plus", "calendar"));

    assert(infiltratr_parse_u64("1234", 10U, &value));
    assert(value == 1234U);
    assert(!infiltratr_parse_u64("-1", 10U, &value));
    assert(infiltratr_parse_i64("-1234", 10U, &signed_value));
    assert(signed_value == -1234);
    assert(infiltratr_parse_u64_range("16", 10U, 10U, 20U, &value));
    assert(value == 16U);
    assert(!infiltratr_parse_u64_range("21", 10U, 10U, 20U, &value));
    assert(infiltratr_parse_i64_range("-4", 10U, -5, 5, &signed_value));
    assert(signed_value == -4);
    assert(!infiltratr_parse_i64_range("-6", 10U, -5, 5, &signed_value));
    assert(infiltratr_parse_double("  -12.5e1  ", &decimal));
    assert(decimal == -125.0);
    assert(!infiltratr_parse_double("12,5", &decimal));
    assert(infiltratr_parse_double_range("45.5", 0.0, 90.0, &decimal));
    assert(decimal == 45.5);
    assert(!infiltratr_parse_double_range("91", 0.0, 90.0, &decimal));

    assert(infiltratr_clamp_double(5.0, 0.0, 4.0) == 4.0);
    assert(infiltratr_clamp_double(-1.0, 0.0, 4.0) == 0.0);
    assert(infiltratr_u64_add_checked(1U, 2U, &sum));
    assert(sum == 3U);
    assert(infiltratr_u64_add_saturating(UINT64_MAX, 1U) == UINT64_MAX);
    assert(infiltratr_u64_multiply_saturating(UINT64_MAX, 2U) == UINT64_MAX);
    assert(infiltratr_percent_u64(1U, 4U) == 25.0);
    assert(infiltratr_u64_counter_rate(200U, 100U, 1.0L, 2.0, &rate));
    assert(rate == 50.0);

    infiltratr_format_bytes(1536U, text, sizeof(text));
    assert(strcmp(text, "1.5 KB") == 0);
    infiltratr_format_rate(2048.0, text, sizeof(text));
    assert(strcmp(text, "2.0 KB/s") == 0);

    assert(infiltratr_path_join(path, sizeof(path), "/tmp", "file"));
    assert(strcmp(path, "/tmp/file") == 0);
    assert(infiltratr_path_concat(path, sizeof(path), "/sys", "/class"));
    assert(strcmp(path, "/sys/class") == 0);

    assert(infiltratr_monotonic_nanoseconds(&nanoseconds));
    assert(nanoseconds > 0U);
    assert(infiltratr_monotonic_seconds() > 0.0);

    const InfiltratrProjectInfo info = {
        .struct_size = sizeof(InfiltratrProjectInfo),
        .abi_version = INFILTRATR_PROJECT_INFO_ABI,
        .program_name = "Test Program",
        .executable_name = "test-program",
        .application_id = "example.test.Program",
        .version = "1.2.3",
        .source_id = "test-program-1.2.3",
        .build_profile = "test",
        .author = "Shannon Smith",
        .website = "https://github.com/The-Infiltratr",
        .license_id = "GPL-3.0-or-later",
        .comments = "Shared library test",
        .icon_name = "test-program",
        .copyright_text = "Copyright (c) 2026 Shannon Smith"
    };
    assert(infiltratr_project_info_is_valid(&info));
    FILE *metadata = tmpfile();
    assert(metadata != NULL);
    assert(infiltratr_project_info_print(metadata, &info) == 0);
    rewind(metadata);
    char metadata_text[1024];
    const size_t metadata_size =
        fread(metadata_text, 1U, sizeof(metadata_text) - 1U, metadata);
    metadata_text[metadata_size] = '\0';
    assert(strstr(metadata_text,
                  "common-library=infiltratr-common-1.8.0\n") != NULL);
    assert(fclose(metadata) == 0);

    puts("Infiltratr Common core smoke test passed.");
    return 0;
}
