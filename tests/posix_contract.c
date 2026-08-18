// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file posix_contract.c
 * @brief Boundary and failure-state coverage for the POSIX provider.
 *
 * The portable API contracts live in portable_contract.c. This suite isolates
 * path, file-I/O and monotonic-clock semantics so portable consumers can run
 * their full contract checks without compiling POSIX code.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#define _POSIX_C_SOURCE 200809L

#include "infiltratr/posix.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void write_all(int descriptor, const char *text, size_t length)
{
    size_t written = 0U;
    while (written < length) {
        const ssize_t amount = write(descriptor, text + written,
                                     length - written);
        assert(amount > 0);
        written += (size_t)amount;
    }
}

static void test_paths(void)
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
}

static void test_text_io(void)
{
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
    write_all(exact_fd, exact_text, sizeof(exact_text) - 1U);
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
    write_all(truncated_fd, truncated_text, sizeof(truncated_text) - 1U);
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
}

static void test_typed_io(void)
{
    char number_name[] = "infiltratr-contract-number-XXXXXX";
    int descriptor = mkstemp(number_name);
    assert(descriptor >= 0);
    static const char number_text[] = "18446744073709551615\n";
    write_all(descriptor, number_text, sizeof(number_text) - 1U);
    assert(close(descriptor) == 0);

    uint64_t unsigned_value = 7U;
    assert(infiltratr_read_u64_file_ex(number_name, &unsigned_value) ==
           INFILTRATR_IO_OK);
    assert(unsigned_value == UINT64_MAX);
    assert(unlink(number_name) == 0);

    char invalid_name[] = "infiltratr-contract-invalid-XXXXXX";
    descriptor = mkstemp(invalid_name);
    assert(descriptor >= 0);
    static const char invalid_text[] = "12x\n";
    write_all(descriptor, invalid_text, sizeof(invalid_text) - 1U);
    assert(close(descriptor) == 0);

    unsigned_value = 99U;
    assert(infiltratr_read_u64_file_ex(invalid_name, &unsigned_value) ==
           INFILTRATR_IO_INVALID_VALUE);
    assert(unsigned_value == 99U);

    double double_value = 9.0;
    assert(infiltratr_read_double_file_ex(invalid_name, &double_value) ==
           INFILTRATR_IO_INVALID_VALUE);
    assert(double_value == 9.0);
    assert(unlink(invalid_name) == 0);
}

static void test_results_and_clock(void)
{
    assert(strcmp(infiltratr_io_result_name(INFILTRATR_IO_OK), "ok") == 0);
    assert(strcmp(infiltratr_io_result_name(INFILTRATR_IO_TRUNCATED),
                  "truncated") == 0);
    assert(strcmp(infiltratr_io_result_name((InfiltratrIoResult)999),
                  "unknown") == 0);

    assert(!infiltratr_monotonic_nanoseconds(NULL));
    uint64_t nanoseconds = 0U;
    assert(infiltratr_monotonic_nanoseconds(&nanoseconds));
    assert(nanoseconds > 0U);
    assert(infiltratr_monotonic_seconds() > 0.0);
}

int main(void)
{
    test_paths();
    test_text_io();
    test_typed_io();
    test_results_and_clock();

    puts("Infiltratr Common POSIX API contract tests passed.");
    return 0;
}
