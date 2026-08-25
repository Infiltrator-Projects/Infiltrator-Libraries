// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file posix_io.h
 * @brief Exact EINTR-safe POSIX descriptor I/O.
 *
 * These helpers complete the requested byte count or fail; short transfers are
 * retried and EINTR is transparent. A zero-byte transfer before completion is
 * treated as EIO rather than as a successful partial operation. Invalid
 * descriptors/buffers set EINVAL. Positioned offsets above INT64_MAX set
 * EOVERFLOW so uint64 callers never silently wrap an off_t.
 */
#ifndef INFILTRATR_COMMON_POSIX_IO_H
#define INFILTRATR_COMMON_POSIX_IO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Read exactly `count` sequential bytes; return 0 or -1 with errno set. */
int infiltratr_read_full(int descriptor, void *buffer, size_t count);
/** Write exactly `count` sequential bytes; return 0 or -1 with errno set. */
int infiltratr_write_full(int descriptor, const void *buffer, size_t count);
/** Read exactly `count` bytes beginning at `offset` without changing file position. */
int infiltratr_pread_full(int descriptor, void *buffer, size_t count,
                          uint64_t offset);
/** Write exactly `count` bytes beginning at `offset` without changing file position. */
int infiltratr_pwrite_full(int descriptor, const void *buffer, size_t count,
                           uint64_t offset);

#ifdef __cplusplus
}
#endif
#endif
