// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef INFILTRATR_COMMON_POSIX_READ_INTERNAL_H
#define INFILTRATR_COMMON_POSIX_READ_INTERNAL_H

#include "infiltratr/arithmetic.h"
#include "infiltratr/posix.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

static InfiltratrIoResult infiltratr_posix_io_result_from_errno(int error_number)
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

static InfiltratrIoResult infiltratr_posix_read_alloc(const char *path,
                                                      char **text,
                                                      size_t *length)
{
    if (text) *text = NULL;
    if (length) *length = 0U;
    if (!path || !text) return INFILTRATR_IO_INVALID_ARGUMENT;

    const int descriptor = open(path, O_RDONLY | O_CLOEXEC);
    if (descriptor < 0)
        return infiltratr_posix_io_result_from_errno(errno);

    char *buffer = NULL;
    size_t capacity = 0U;
    size_t used = 0U;
    InfiltratrIoResult result = INFILTRATR_IO_OK;

    for (;;) {
        size_t required = 0U;
        if (!infiltratr_size_add_checked(used, 2U, &required) ||
            !infiltratr_array_reserve((void **)&buffer, &capacity,
                                      sizeof(buffer[0]), required, 256U)) {
            result = INFILTRATR_IO_ERROR;
            break;
        }

        size_t request = capacity - used - 1U;
        if (request > (size_t)SSIZE_MAX) request = (size_t)SSIZE_MAX;

        ssize_t amount;
        do {
            amount = read(descriptor, buffer + used, request);
        } while (amount < 0 && errno == EINTR);

        if (amount < 0) {
            result = infiltratr_posix_io_result_from_errno(errno);
            break;
        }
        if (amount == 0) break;
        used += (size_t)amount;
    }

    if (close(descriptor) != 0 && result == INFILTRATR_IO_OK)
        result = infiltratr_posix_io_result_from_errno(errno);

    if (result != INFILTRATR_IO_OK || used == 0U) {
        free(buffer);
        return result != INFILTRATR_IO_OK ? result : INFILTRATR_IO_EMPTY;
    }

    buffer[used] = '\0';
    *text = buffer;
    if (length) *length = used;
    return INFILTRATR_IO_OK;
}

#endif
