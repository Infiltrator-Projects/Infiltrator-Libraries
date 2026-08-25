// SPDX-License-Identifier: GPL-3.0-or-later
#define _POSIX_C_SOURCE 200809L
#include "infiltratr/posix_io.h"

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>

static int valid_descriptor_buffer(int descriptor, const void *buffer,
                                   size_t count)
{
    if (descriptor < 0 || (!buffer && count != 0U)) {
        errno = EINVAL;
        return 0;
    }
    return 1;
}

int infiltratr_read_full(int descriptor, void *buffer, size_t count)
{
    if (!valid_descriptor_buffer(descriptor, buffer, count)) return -1;
    uint8_t *cursor = (uint8_t *)buffer;
    while (count != 0U) {
        ssize_t amount;
        do {
            amount = read(descriptor, cursor, count);
        } while (amount < 0 && errno == EINTR);
        if (amount < 0) return -1;
        if (amount == 0) {
            errno = EIO;
            return -1;
        }
        cursor += (size_t)amount;
        count -= (size_t)amount;
    }
    return 0;
}

int infiltratr_write_full(int descriptor, const void *buffer, size_t count)
{
    if (!valid_descriptor_buffer(descriptor, buffer, count)) return -1;
    const uint8_t *cursor = (const uint8_t *)buffer;
    while (count != 0U) {
        ssize_t amount;
        do {
            amount = write(descriptor, cursor, count);
        } while (amount < 0 && errno == EINTR);
        if (amount < 0) return -1;
        if (amount == 0) {
            errno = EIO;
            return -1;
        }
        cursor += (size_t)amount;
        count -= (size_t)amount;
    }
    return 0;
}

static int offset_representable(uint64_t offset)
{
    if (offset > (uint64_t)INT64_MAX) {
        errno = EOVERFLOW;
        return 0;
    }
    return 1;
}

int infiltratr_pread_full(int descriptor, void *buffer, size_t count,
                          uint64_t offset)
{
    if (!valid_descriptor_buffer(descriptor, buffer, count)) return -1;
    uint8_t *cursor = (uint8_t *)buffer;
    while (count != 0U) {
        if (!offset_representable(offset)) return -1;
        ssize_t amount;
        do {
            amount = pread(descriptor, cursor, count, (off_t)offset);
        } while (amount < 0 && errno == EINTR);
        if (amount < 0) return -1;
        if (amount == 0) {
            errno = EIO;
            return -1;
        }
        const size_t completed = (size_t)amount;
        cursor += completed;
        count -= completed;
        if ((uint64_t)completed > UINT64_MAX - offset) {
            errno = EOVERFLOW;
            return -1;
        }
        offset += (uint64_t)completed;
    }
    return 0;
}

int infiltratr_pwrite_full(int descriptor, const void *buffer, size_t count,
                           uint64_t offset)
{
    if (!valid_descriptor_buffer(descriptor, buffer, count)) return -1;
    const uint8_t *cursor = (const uint8_t *)buffer;
    while (count != 0U) {
        if (!offset_representable(offset)) return -1;
        ssize_t amount;
        do {
            amount = pwrite(descriptor, cursor, count, (off_t)offset);
        } while (amount < 0 && errno == EINTR);
        if (amount < 0) return -1;
        if (amount == 0) {
            errno = EIO;
            return -1;
        }
        const size_t completed = (size_t)amount;
        cursor += completed;
        count -= completed;
        if ((uint64_t)completed > UINT64_MAX - offset) {
            errno = EOVERFLOW;
            return -1;
        }
        offset += (uint64_t)completed;
    }
    return 0;
}
