// SPDX-License-Identifier: GPL-3.0-or-later
#define _POSIX_C_SOURCE 200809L
#include "infiltratr/posix_numeric.h"
#include "infiltratr/core.h"
#include "posix_read_internal.h"

#include <stdlib.h>
#include <string.h>

InfiltratrIoResult infiltratr_read_i64_file_ex(const char *path, int64_t *value)
{
    if (!path || !value) return INFILTRATR_IO_INVALID_ARGUMENT;

    char *text = NULL;
    size_t length = 0U;
    const InfiltratrIoResult status =
        infiltratr_posix_read_alloc(path, &text, &length);
    if (status != INFILTRATR_IO_OK) return status;

    int64_t parsed = 0;
    const bool valid_text = memchr(text, '\0', length) == NULL;
    const bool parsed_ok = valid_text &&
        infiltratr_parse_i64(text, 10U, &parsed);
    free(text);

    if (!parsed_ok) return INFILTRATR_IO_INVALID_VALUE;
    *value = parsed;
    return INFILTRATR_IO_OK;
}

bool infiltratr_read_i64_file(const char *path, int64_t *value)
{
    return infiltratr_read_i64_file_ex(path, value) == INFILTRATR_IO_OK;
}
