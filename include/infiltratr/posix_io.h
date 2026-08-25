// SPDX-License-Identifier: GPL-3.0-or-later
/** @file posix_io.h @brief Exact EINTR-safe POSIX descriptor I/O. */
#ifndef INFILTRATR_COMMON_POSIX_IO_H
#define INFILTRATR_COMMON_POSIX_IO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int infiltratr_read_full(int descriptor, void *buffer, size_t count);
int infiltratr_write_full(int descriptor, const void *buffer, size_t count);
int infiltratr_pread_full(int descriptor, void *buffer, size_t count,
                          uint64_t offset);
int infiltratr_pwrite_full(int descriptor, const void *buffer, size_t count,
                           uint64_t offset);

#ifdef __cplusplus
}
#endif
#endif
