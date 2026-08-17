// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file api_contract.c
 * @brief Boundary and failure-state regression coverage for the public API.
 *
 * This suite concentrates on behaviour that consumers must be able to rely on:
 * exact buffer boundaries, unchanged outputs after rejected parses/arithmetic,
 * explicit I/O result distinctions, ABI validation and invalid-argument paths.
 * It complements the shorter smoke tests rather than duplicating their normal
 * success-path examples.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#define _POSIX_C_SOURCE 200809L

#include "infiltratr/core.h"
#include "infiltratr/format.h"
#include "infiltratr/posix.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static InfiltratrProjectInfo valid_project_info(void)
{
    const InfiltratrProjectInfo info = {
        .struct_size = sizeof(InfiltratrProjectInfo),
        .abi_version = INFILTRATR_PROJECT_INFO_ABI,
        .program_name = "Contract Test",
        .executable_name = "contract-test",
        .application_id = "example.contract.Test",
        .version = "1.0.0",
        .source_id = "contract-test-1.0.0",
        .build_profile = "test",
        .author = "Shannon Smith",
        .website = "https://github.com/The-First-Infiltrator/Infiltrator-Libraries",
        .license_id = "GPL-3.0-or-later",
        .comments = "Public API contract regression suite",
        .icon_name = "contract-test",
        .copyright_text = "Copyright (c) 2026 Shannon Smith"
    };
    return info;
}

static void test_project_info(void)
{
    InfiltratrProjectInfo info = valid_project_info();
    assert(infiltratr_project_info_is_valid(&info));

    info.struct_size = sizeof(info) - 1U;
    assert(!infiltratr_project_info_is_valid(&info));
    info = valid_project_info();
    info.abi_version++;
    assert(!infiltratr_project_info_is_valid(&info));
    info = valid_project_info();
    info.website = "";
    assert(!infiltratr_project_info_is_valid(&info));
    assert(!infiltratr_project_info_is_valid(NULL));

    info = valid_project_info();
    info.author = "Shannon\nSmith";
    FILE *stream = tmpfile();
    assert(stream != NULL);
    assert(infiltratr_project_info_print(stream, &info) == 0);
    rewind(stream);
    char output[1024];
    const size_t amount = fread(output, 1U, sizeof(output) - 1U, stream);
    output[amount] = '\0';
    assert(strstr(output, "author=Shannon Smith\n") != NULL);
    assert(fclose(stream) == 0);
    assert(infiltratr_project_info_print(NULL, &info) == -1);
}

static void test_strings(void)
{
    char text[5] = "xxxx";
    infiltratr_copy_string(text, sizeof(text), "abcd");
    assert(strcmp(text, "abcd") == 0);
    infiltratr_copy_string(text, sizeof(text), "abcde");
    assert(strcmp(text, "abcd") == 0);
    infiltratr_copy_string(text, sizeof(text), NULL);
    assert(strcmp(text, "") == 0);

    char single = 'Q';
    infiltratr_copy_string(&single, 0U, "x");
    assert(single == 'Q');
    infiltratr_copy_string(NULL, 8U, "x");

    char whitespace[] = "\t  value \r\n";
    infiltratr_trim(whitespace);
    assert(strcmp(whitespace, "value") == 0);
    char only_whitespace[] = " \t\n";
    infiltratr_trim(only_whitespace);
    assert(strcmp(only_whitespace, "") == 0);
    infiltratr_trim(NULL);

    char line[] = "value\r\n\n";
    infiltratr_trim_line_end(line);
    assert(strcmp(line, "value") == 0);
    infiltratr_trim_line_end(NULL);

    assert(infiltratr_string_equal(NULL, NULL));
    assert(!infiltratr_string_equal(NULL, ""));
    assert(infiltratr_string_starts_with("abc", ""));
    assert(infiltratr_string_ends_with("abc", ""));
    assert(!infiltratr_string_starts_with(NULL, ""));
    assert(!infiltratr_string_ends_with("abc", NULL));
}

static void test_parsers(void)
{
    uint64_t unsigned_value = 777U;
    assert(infiltratr_parse_u64("  18446744073709551615  ", 10U,
                                &unsigned_value));
    assert(unsigned_value == UINT64_MAX);
    unsigned_value = 777U;
    assert(!infiltratr_parse_u64("+1", 10U, &unsigned_value));
    assert(unsigned_value == 777U);
    assert(!infiltratr_parse_u64("1x", 10U, &unsigned_value));
    assert(unsigned_value == 777U);
    assert(!infiltratr_parse_u64("1", 1U, &unsigned_value));
    assert(!infiltratr_parse_u64("1", 37U, &unsigned_value));
    assert(!infiltratr_parse_u64(NULL, 10U, &unsigned_value));
    assert(!infiltratr_parse_u64("1", 10U, NULL));

    int64_t signed_value = 333;
    assert(infiltratr_parse_i64("-9223372036854775808", 10U,
                                &signed_value));
    assert(signed_value == INT64_MIN);
    signed_value = 333;
    assert(!infiltratr_parse_i64("9223372036854775808", 10U,
                                 &signed_value));
    assert(signed_value == 333);

    unsigned_value = 88U;
    assert(!infiltratr_parse_u64_range("5", 10U, 10U, 1U,
                                       &unsigned_value));
    assert(unsigned_value == 88U);
    assert(!infiltratr_parse_u64_range("9", 10U, 10U, 20U,
                                       &unsigned_value));
    assert(unsigned_value == 88U);

    signed_value = 88;
    assert(!infiltratr_parse_i64_range("0", 10U, 1, -1, &signed_value));
    assert(signed_value == 88);

    double decimal = 123.0;
    assert(infiltratr_parse_double("\t-0.125e+2\n", &decimal));
    assert(decimal == -12.5);
    decimal = 123.0;
    assert(!infiltratr_parse_double("nan", &decimal));
    assert(decimal == 123.0);
    assert(!infiltratr_parse_double("inf", &decimal));
    assert(decimal == 123.0);
    assert(!infiltratr_parse_double("1e-500", &decimal));
    assert(decimal == 123.0);
    assert(!infiltratr_parse_double("1e500", &decimal));
    assert(decimal == 123.0);
    assert(!infiltratr_parse_double("1.0 trailing", &decimal));
    assert(decimal == 123.0);

    decimal = 99.0;
    assert(!infiltratr_parse_double_range("5", NAN, 10.0, &decimal));
    assert(decimal == 99.0);
    assert(!infiltratr_parse_double_range("5", 10.0, 1.0, &decimal));
    assert(decimal == 99.0);
}

static void test_arithmetic(void)
{
    assert(infiltratr_clamp_double(-1.0, 0.0, 10.0) == 0.0);
    assert(infiltratr_clamp_double(11.0, 0.0, 10.0) == 10.0);
    assert(infiltratr_clamp_double(5.0, 10.0, 0.0) == 5.0);

    uint64_t result = 42U;
    assert(!infiltratr_u64_add_checked(UINT64_MAX, 1U, &result));
    assert(result == 42U);
    assert(!infiltratr_u64_add_checked(1U, 1U, NULL));
    assert(infiltratr_u64_add_saturating(UINT64_MAX - 1U, 1U) == UINT64_MAX);
    assert(infiltratr_u64_add_saturating(UINT64_MAX, 1U) == UINT64_MAX);
    assert(infiltratr_u64_multiply_saturating(0U, UINT64_MAX) == 0U);
    assert(infiltratr_u64_multiply_saturating(UINT64_MAX, 2U) == UINT64_MAX);

    assert(infiltratr_percent_u64(0U, 0U) == 0.0);
    assert(infiltratr_percent_u64(200U, 100U) == 100.0);

    double rate = 77.0;
    assert(!infiltratr_u64_counter_rate(9U, 10U, 1.0L, 1.0, &rate));
    assert(rate == 0.0);
    rate = 77.0;
    assert(!infiltratr_u64_counter_rate(10U, 9U, -1.0L, 1.0, &rate));
    assert(rate == 0.0);
    assert(!infiltratr_u64_counter_rate(10U, 9U, 1.0L, 0.0, &rate));
    assert(rate == 0.0);
    assert(!infiltratr_u64_counter_rate(10U, 9U, 1.0L, 1.0, NULL));
}

static void test_scaling_and_formatting(void)
{
    static const char *const units[] = {"B", "KB", "MB"};
    InfiltratrScaleOptions scale = INFILTRATR_SCALE_OPTIONS_INIT;
    long double scaled = 77.0L;
    size_t unit = 77U;

    assert(infiltratr_scale_quantity(1024.0L, &scale,
                                     INFILTRATR_ARRAY_LENGTH(units),
                                     &scaled, &unit));
    assert(scaled == 1.0L && unit == 1U);

    InfiltratrScaleOptions invalid = scale;
    invalid.abi_version++;
    scaled = 77.0L;
    unit = 77U;
    assert(!infiltratr_scale_quantity(1.0L, &invalid,
                                      INFILTRATR_ARRAY_LENGTH(units),
                                      &scaled, &unit));
    assert(scaled == 77.0L && unit == 77U);
    invalid = scale;
    invalid.divisor = 1.0L;
    assert(!infiltratr_scale_quantity(1.0L, &invalid,
                                      INFILTRATR_ARRAY_LENGTH(units),
                                      &scaled, &unit));

    char buffer[16] = "unchanged";
    assert(!infiltratr_format_scaled_quantity(NAN, units,
                                               INFILTRATR_ARRAY_LENGTH(units),
                                               "", &scale, buffer,
                                               sizeof(buffer)));
    assert(strcmp(buffer, "") == 0);

    InfiltratrScaleOptions fixed = INFILTRATR_SCALE_OPTIONS_INIT;
    fixed.minimum_unit = 0U;
    fixed.maximum_unit = 0U;
    fixed.decimal_places = 0U;
    char tiny[3] = {'x', 'x', 'x'};
    assert(!infiltratr_format_scaled_quantity(1.0L, units,
                                               INFILTRATR_ARRAY_LENGTH(units),
                                               "", &fixed, tiny,
                                               sizeof(tiny)));
    assert(tiny[sizeof(tiny) - 1U] == '\0');

    InfiltratrScalarFormatOptions scalar = INFILTRATR_SCALAR_FORMAT_OPTIONS_INIT;
    scalar.unavailable_text = "missing";
    assert(infiltratr_format_scalar(false, 0.0L, &scalar,
                                    buffer, sizeof(buffer)));
    assert(strcmp(buffer, "missing") == 0);

    scalar.struct_size = sizeof(scalar) - 1U;
    infiltratr_copy_string(buffer, sizeof(buffer), "old");
    assert(!infiltratr_format_scalar(true, 1.0L, &scalar,
                                     buffer, sizeof(buffer)));
    assert(strcmp(buffer, "") == 0);

    scalar = (InfiltratrScalarFormatOptions)INFILTRATR_SCALAR_FORMAT_OPTIONS_INIT;
    scalar.clamp = true;
    scalar.minimum = 10.0L;
    scalar.maximum = 1.0L;
    assert(!infiltratr_format_scalar(true, 5.0L, &scalar,
                                     buffer, sizeof(buffer)));
    assert(strcmp(buffer, "") == 0);
}

static void test_paths_and_io(void)
{
    char path[16];
    assert(infiltratr_path_concat(path, sizeof(path), "/sys/", "dev"));
    assert(strcmp(path, "/sys/dev") == 0);

    char too_small[5] = "xxxx";
    assert(!infiltratr_path_concat(too_small, sizeof(too_small),
                                   "/sys", "/device"));
    assert(strcmp(too_small, "") == 0);

    assert(infiltratr_path_join(path, sizeof(path), "/sys/", "/dev"));
    assert(strcmp(path, "/sys/dev") == 0);
    assert(infiltratr_path_join(path, sizeof(path), "", "/dev"));
    assert(strcmp(path, "/dev") == 0);

    char resolved[1] = {'x'};
    assert(!infiltratr_realpath_copy(".", resolved, sizeof(resolved)));
    assert(resolved[0] == '\0');

    char empty_name[] = "infiltratr-contract-empty-XXXXXX";
    const int empty_fd = mkstemp(empty_name);
    assert(empty_fd >= 0);
    assert(close(empty_fd) == 0);

    char read_buffer[8] = "old";
    size_t length = 99U;
    assert(infiltratr_read_text_file_ex(empty_name, read_buffer,
                                        sizeof(read_buffer), &length) ==
           INFILTRATR_IO_EMPTY);
    assert(strcmp(read_buffer, "") == 0);
    assert(length == 0U);
    assert(!infiltratr_read_text_file(empty_name, read_buffer,
                                      sizeof(read_buffer)));
    assert(unlink(empty_name) == 0);

    char exact_name[] = "infiltratr-contract-exact-XXXXXX";
    const int exact_fd = mkstemp(exact_name);
    assert(exact_fd >= 0);
    static const char exact_text[] = "1234567";
    assert(write(exact_fd, exact_text, sizeof(exact_text) - 1U) ==
           (ssize_t)(sizeof(exact_text) - 1U));
    assert(close(exact_fd) == 0);
    assert(infiltratr_read_text_file_ex(exact_name, read_buffer,
                                        sizeof(read_buffer), &length) ==
           INFILTRATR_IO_OK);
    assert(strcmp(read_buffer, exact_text) == 0);
    assert(length == sizeof(exact_text) - 1U);
    assert(unlink(exact_name) == 0);

    char truncated_name[] = "infiltratr-contract-trunc-XXXXXX";
    const int truncated_fd = mkstemp(truncated_name);
    assert(truncated_fd >= 0);
    static const char truncated_text[] = "12345678";
    assert(write(truncated_fd, truncated_text,
                 sizeof(truncated_text) - 1U) ==
           (ssize_t)(sizeof(truncated_text) - 1U));
    assert(close(truncated_fd) == 0);
    assert(infiltratr_read_text_file_ex(truncated_name, read_buffer,
                                        sizeof(read_buffer), &length) ==
           INFILTRATR_IO_TRUNCATED);
    assert(strcmp(read_buffer, "1234567") == 0);
    assert(length == 7U);
    assert(unlink(truncated_name) == 0);

    length = 99U;
    assert(infiltratr_read_text_file_ex("does-not-exist-infiltratr", read_buffer,
                                        sizeof(read_buffer), &length) ==
           INFILTRATR_IO_NOT_FOUND);
    assert(strcmp(read_buffer, "") == 0);
    assert(length == 0U);

    char invalid_buffer = 'Q';
    length = 99U;
    assert(infiltratr_read_text_file_ex("x", &invalid_buffer, 1U, &length) ==
           INFILTRATR_IO_INVALID_ARGUMENT);
    assert(invalid_buffer == 'Q');
    assert(length == 0U);

    assert(strcmp(infiltratr_io_result_name(INFILTRATR_IO_OK), "ok") == 0);
    assert(strcmp(infiltratr_io_result_name((InfiltratrIoResult)999),
                  "unknown") == 0);

    assert(!infiltratr_monotonic_nanoseconds(NULL));
    uint64_t nanoseconds = 0U;
    assert(infiltratr_monotonic_nanoseconds(&nanoseconds));
    assert(nanoseconds > 0U);
}

int main(void)
{
    test_project_info();
    test_strings();
    test_parsers();
    test_arithmetic();
    test_scaling_and_formatting();
    test_paths_and_io();

    puts("Infiltratr Common API contract tests passed.");
    return 0;
}
