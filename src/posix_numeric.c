// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/posix_numeric.h"
#include "infiltratr/core.h"

InfiltratrIoResult infiltratr_read_i64_file_ex(const char *path, int64_t *value)
{
    if (!path || !value) return INFILTRATR_IO_INVALID_ARGUMENT;
    char text[128];
    const InfiltratrIoResult status =
        infiltratr_read_text_file_ex(path, text, sizeof(text), NULL);
    if (status != INFILTRATR_IO_OK) return status;
    int64_t parsed = 0;
    if (!infiltratr_parse_i64(text, 10U, &parsed))
        return INFILTRATR_IO_INVALID_VALUE;
    *value = parsed;
    return INFILTRATR_IO_OK;
}

bool infiltratr_read_i64_file(const char *path, int64_t *value)
{
    return infiltratr_read_i64_file_ex(path, value) == INFILTRATR_IO_OK;
}
