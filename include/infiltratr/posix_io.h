// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file posix_io.h
 * @brief Exact positioned POSIX descriptor I/O shared by storage consumers.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_POSIX_IO_H
#define INFILTRATR_COMMON_POSIX_IO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read exactly count bytes from a descriptor at an unsigned absolute offset.
 *
 * EINTR is retried. A premature EOF is reported as EIO. An offset that cannot
 * be represented by a signed 64-bit off_t-compatible position is reported as
 * EOVERFLOW. A NULL buffer is accepted only when count is zero.
 *
 * @return 0 on complete success, -1 on failure with errno set.
 */
int infiltratr_pread_full(int descriptor, void *buffer, size_t count,
                          uint64_t offset);

/**
 * Write exactly count bytes to a descriptor at an unsigned absolute offset.
 *
 * EINTR is retried. A zero-length write before completion is reported as EIO.
 * The same offset and buffer rules as infiltratr_pread_full apply.
 *
 * @return 0 on complete success, -1 on failure with errno set.
 */
int infiltratr_pwrite_full(int descriptor, const void *buffer, size_t count,
                           uint64_t offset);

#ifdef __cplusplus
}
#endif

#endif
