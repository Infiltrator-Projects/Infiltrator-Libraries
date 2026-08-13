// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file posix.c
 * @brief POSIX implementation of shared file, path and clock primitives.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include "infiltratr/posix.h"
#include "infiltratr/core.h"

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static InfiltratrIoResult io_result_from_errno(int error_number)
{
    switch (error_number) {
    case ENOENT:
    case ENOTDIR:
        return INFILTRATR_IO_NOT_FOUND;
    case EACCES:
    case EPERM:
        return INFILTRATR_IO_PERMISSION_DENIED;
    default:
        return INFILTRATR_IO_ERROR;
    }
}

const char *infiltratr_io_result_name(InfiltratrIoResult result)
{
    switch (result) {
    case INFILTRATR_IO_OK:
        return "ok";
    case INFILTRATR_IO_INVALID_ARGUMENT:
        return "invalid-argument";
    case INFILTRATR_IO_NOT_FOUND:
        return "not-found";
    case INFILTRATR_IO_PERMISSION_DENIED:
        return "permission-denied";
    case INFILTRATR_IO_EMPTY:
        return "empty";
    case INFILTRATR_IO_TRUNCATED:
        return "truncated";
    case INFILTRATR_IO_INVALID_VALUE:
        return "invalid-value";
    case INFILTRATR_IO_ERROR:
        return "io-error";
    }
    return "unknown";
}

bool infiltratr_realpath_copy(const char *path, char *destination, size_t size)
{
    if (!path || !destination || size == 0U) return false;
    destination[0] = '\0';

    char *resolved = realpath(path, NULL);
    if (!resolved) return false;
    const size_t length = strlen(resolved);
    if (length >= size) {
        free(resolved);
        return false;
    }
    memcpy(destination, resolved, length + 1U);
    free(resolved);
    return true;
}

bool infiltratr_path_concat(char *destination, size_t size,
                            const char *base, const char *suffix)
{
    if (!destination || size == 0U || !base || !suffix) return false;
    const size_t base_length = strlen(base);
    const size_t suffix_length = strlen(suffix);
    if (base_length >= size || suffix_length > size - base_length - 1U) {
        destination[0] = '\0';
        return false;
    }
    memcpy(destination, base, base_length);
    memcpy(destination + base_length, suffix, suffix_length + 1U);
    return true;
}

bool infiltratr_path_join(char *destination, size_t size,
                          const char *left, const char *right)
{
    if (!destination || size == 0U || !left || !right) return false;
    const size_t left_length = strlen(left);
    const bool needs_separator = left_length > 0U && left[left_length - 1U] != '/';
    const char *right_start = right;
    while (*right_start == '/' && left_length > 0U) right_start++;
    const size_t right_length = strlen(right_start);
    const size_t separator_length = needs_separator ? 1U : 0U;
    if (left_length >= size || separator_length > size - left_length - 1U ||
        right_length > size - left_length - separator_length - 1U) {
        destination[0] = '\0';
        return false;
    }
    memcpy(destination, left, left_length);
    size_t offset = left_length;
    if (needs_separator) destination[offset++] = '/';
    memcpy(destination + offset, right_start, right_length + 1U);
    return true;
}

InfiltratrIoResult infiltratr_read_text_file_ex(const char *path,
                                                char *buffer, size_t size,
                                                size_t *length)
{
    if (length) *length = 0U;
    if (!path || !buffer || size < 2U) return INFILTRATR_IO_INVALID_ARGUMENT;
    buffer[0] = '\0';

    const int descriptor = open(path, O_RDONLY | O_CLOEXEC);
    if (descriptor < 0) return io_result_from_errno(errno);

    size_t used = 0U;
    InfiltratrIoResult result = INFILTRATR_IO_OK;
    while (used + 1U < size) {
        ssize_t amount;
        do {
            amount = read(descriptor, buffer + used, size - used - 1U);
        } while (amount < 0 && errno == EINTR);

        if (amount < 0) {
            result = io_result_from_errno(errno);
            break;
        }
        if (amount == 0) break;
        used += (size_t)amount;
    }

    if (result == INFILTRATR_IO_OK && used + 1U == size) {
        char extra = '\0';
        ssize_t amount;
        do {
            amount = read(descriptor, &extra, 1U);
        } while (amount < 0 && errno == EINTR);
        if (amount < 0)
            result = io_result_from_errno(errno);
        else if (amount > 0)
            result = INFILTRATR_IO_TRUNCATED;
    }

    const int close_result = close(descriptor);
    if (result == INFILTRATR_IO_OK && close_result != 0)
        result = io_result_from_errno(errno);

    buffer[used] = '\0';
    infiltratr_trim_line_end(buffer);
    if (length) *length = strlen(buffer);
    if (result != INFILTRATR_IO_OK) return result;
    return used == 0U ? INFILTRATR_IO_EMPTY : INFILTRATR_IO_OK;
}

InfiltratrIoResult infiltratr_read_u64_file_ex(const char *path,
                                               uint64_t *value)
{
    if (!value) return INFILTRATR_IO_INVALID_ARGUMENT;
    char buffer[128];
    const InfiltratrIoResult result =
        infiltratr_read_text_file_ex(path, buffer, sizeof(buffer), NULL);
    if (result != INFILTRATR_IO_OK) return result;
    return infiltratr_parse_u64(buffer, 10U, value)
        ? INFILTRATR_IO_OK : INFILTRATR_IO_INVALID_VALUE;
}

InfiltratrIoResult infiltratr_read_double_file_ex(const char *path,
                                                  double *value)
{
    if (!value) return INFILTRATR_IO_INVALID_ARGUMENT;
    char buffer[128];
    const InfiltratrIoResult result =
        infiltratr_read_text_file_ex(path, buffer, sizeof(buffer), NULL);
    if (result != INFILTRATR_IO_OK) return result;
    return infiltratr_parse_double(buffer, value)
        ? INFILTRATR_IO_OK : INFILTRATR_IO_INVALID_VALUE;
}

bool infiltratr_read_text_file(const char *path, char *buffer, size_t size)
{
    if (!path || !buffer || size < 2U) return false;
    buffer[0] = '\0';
    const int descriptor = open(path, O_RDONLY | O_CLOEXEC);
    if (descriptor < 0) return false;

    ssize_t length;
    do {
        length = read(descriptor, buffer, size - 1U);
    } while (length < 0 && errno == EINTR);
    const int saved_errno = errno;
    (void)close(descriptor);
    errno = saved_errno;

    if (length <= 0) return false;
    buffer[(size_t)length] = '\0';
    infiltratr_trim_line_end(buffer);
    return true;
}

bool infiltratr_read_u64_file(const char *path, uint64_t *value)
{
    char buffer[128];
    return value && infiltratr_read_text_file(path, buffer, sizeof(buffer)) &&
           infiltratr_parse_u64(buffer, 10U, value);
}

uint64_t infiltratr_read_u64_or_zero(const char *path)
{
    uint64_t value = 0U;
    (void)infiltratr_read_u64_file(path, &value);
    return value;
}

bool infiltratr_read_double_file(const char *path, double *value)
{
    char buffer[128];
    return value && infiltratr_read_text_file(path, buffer, sizeof(buffer)) &&
           infiltratr_parse_double(buffer, value);
}

double infiltratr_read_double_or_nan(const char *path)
{
    double value = NAN;
    (void)infiltratr_read_double_file(path, &value);
    return value;
}

bool infiltratr_read_first_u64(const char *base,
                               const char *const *suffixes,
                               size_t suffix_count, uint64_t *value)
{
    if (!base || !suffixes || !value) return false;
    char path[512];
    for (size_t index = 0U; index < suffix_count; index++) {
        if (!infiltratr_path_concat(path, sizeof(path), base, suffixes[index]))
            continue;
        if (infiltratr_read_u64_file(path, value)) return true;
    }
    return false;
}

bool infiltratr_monotonic_nanoseconds(uint64_t *nanoseconds)
{
    if (!nanoseconds) return false;
    *nanoseconds = 0U;

    struct timespec timestamp = {0};
    if (clock_gettime(CLOCK_MONOTONIC, &timestamp) != 0 ||
        timestamp.tv_sec < 0 || timestamp.tv_nsec < 0 ||
        timestamp.tv_nsec >= 1000000000L)
        return false;

    const uint64_t seconds = (uint64_t)timestamp.tv_sec;
    const uint64_t fractional = (uint64_t)timestamp.tv_nsec;
    if (seconds > (UINT64_MAX - fractional) / 1000000000U)
        return false;

    *nanoseconds = seconds * 1000000000U + fractional;
    return true;
}

double infiltratr_monotonic_seconds(void)
{
    struct timespec timestamp = {0};
    if (clock_gettime(CLOCK_MONOTONIC, &timestamp) != 0) return 0.0;
    return (double)timestamp.tv_sec + (double)timestamp.tv_nsec / 1000000000.0;
}
