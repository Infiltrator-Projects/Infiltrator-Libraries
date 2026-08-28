// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file posix.h
 * @brief POSIX file, path and monotonic-clock adapters for the shared C core.
 *
 * The rich `*_ex` file readers preserve meaningful failure categories and are
 * preferred when a caller needs to distinguish missing data, permission
 * failures, truncation and invalid values. The boolean/value helpers preserve
 * the same complete-input semantics while collapsing those distinctions.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_POSIX_H
#define INFILTRATR_COMMON_POSIX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    INFILTRATR_IO_OK = 0,
    INFILTRATR_IO_INVALID_ARGUMENT,
    INFILTRATR_IO_NOT_FOUND,
    INFILTRATR_IO_PERMISSION_DENIED,
    INFILTRATR_IO_EMPTY,
    INFILTRATR_IO_TRUNCATED,
    INFILTRATR_IO_INVALID_VALUE,
    INFILTRATR_IO_ERROR
} InfiltratrIoResult;

typedef enum {
    INFILTRATR_ATOMIC_FILE_PRIVATE = 0,
    INFILTRATR_ATOMIC_FILE_PRESERVE_PERMISSIONS
} InfiltratrAtomicFileMode;

typedef bool (*InfiltratrAtomicFileWriter)(FILE *stream,
                                           const void *user_data);

const char *infiltratr_io_result_name(InfiltratrIoResult result);

bool infiltratr_realpath_copy(const char *path, char *destination, size_t size);

/** Concatenate two byte-exact POSIX path fragments without inserting a separator. */
bool infiltratr_path_concat(char *destination, size_t size,
                            const char *base, const char *suffix);

/**
 * Join two POSIX path fragments at their boundary without normalizing either
 * fragment internally. A nonempty left fragment is copied verbatim; leading
 * slashes are removed from the right fragment, and one slash is inserted only
 * when the preserved left fragment does not already end in one. With an empty
 * left fragment, the right fragment is preserved verbatim (including a leading
 * slash). Source strings must not overlap the writable destination. On an
 * otherwise-valid size failure, destination is cleared.
 */
bool infiltratr_path_join(char *destination, size_t size,
                          const char *left, const char *right);

bool infiltratr_first_readable_path(const char *base,
                                    const char *const *suffixes,
                                    size_t suffix_count,
                                    char *destination, size_t size);

/**
 * Read one complete bounded text file with explicit failure reporting.
 *
 * `size` must be at least two bytes. With otherwise-valid arguments, `buffer`
 * is cleared before opening the path. Reads retry after `EINTR`, reserve one
 * byte for NUL termination and perform an additional one-byte probe when the
 * buffer fills so exact-fit input can be distinguished from truncation.
 * Trailing CR/LF bytes in the retained text are removed.
 *
 * `length`, when non-NULL, is set to zero before argument validation and then
 * receives the retained post-line-ending length. A zero-byte file returns
 * `INFILTRATR_IO_EMPTY`. `INFILTRATR_IO_TRUNCATED` preserves the bounded
 * prefix in `buffer` so rich callers can inspect it.
 */
InfiltratrIoResult infiltratr_read_text_file_ex(const char *path,
                                                char *buffer, size_t size,
                                                size_t *length);

/**
 * Read and parse one complete unsigned decimal file value.
 *
 * The implementation reads to EOF with dynamically grown temporary storage;
 * there is no fixed text-length limit. Embedded NUL bytes are rejected rather
 * than allowing trailing file content to escape validation. I/O status is
 * propagated unchanged and `*value` changes only on successful parsing.
 */
InfiltratrIoResult infiltratr_read_u64_file_ex(const char *path,
                                               uint64_t *value);

/**
 * Read and parse one complete finite ASCII-decimal file value.
 *
 * The implementation reads to EOF with dynamically grown temporary storage;
 * there is no fixed text-length limit. Embedded NUL bytes are rejected.
 * Parsing uses the locale-independent core decimal grammar. I/O status is
 * propagated unchanged and `*value` changes only on success.
 */
InfiltratrIoResult infiltratr_read_double_file_ex(const char *path,
                                                  double *value);

/**
 * Read one complete bounded text file and remove trailing CR/LF bytes.
 *
 * This compatibility wrapper retains the boolean API but not the historical
 * silent-truncation shortcut: oversized, empty or failed reads return false and
 * leave valid output storage empty. Exact-fit input succeeds.
 */
bool infiltratr_read_text_file(const char *path, char *buffer, size_t size);

/** Parse one complete unsigned decimal file value with no fixed text limit. */
bool infiltratr_read_u64_file(const char *path, uint64_t *value);

uint64_t infiltratr_read_u64_or_zero(const char *path);

/** Parse one complete finite decimal file value with no fixed text limit. */
bool infiltratr_read_double_file(const char *path, double *value);

double infiltratr_read_double_or_nan(const char *path);

/**
 * Read the first successfully parsed unsigned attribute from ordered suffixes.
 *
 * Each non-NULL suffix is concatenated directly to `base`; no separator is
 * inserted. Candidate paths are allocated to their exact required length, so
 * the helper imposes no private path-length ceiling beyond the platform and
 * allocator. The first successful value is returned; false means none
 * succeeded or a candidate path could not be represented/allocated.
 */
bool infiltratr_read_first_u64(const char *base,
                               const char *const *suffixes,
                               size_t suffix_count, uint64_t *value);

int infiltratr_atomic_file_write(const char *path,
                                 InfiltratrAtomicFileMode mode,
                                 InfiltratrAtomicFileWriter writer,
                                 const void *user_data);

int infiltratr_atomic_file_write_bytes(const char *path,
                                       InfiltratrAtomicFileMode mode,
                                       const void *data, size_t length);

bool infiltratr_monotonic_nanoseconds(uint64_t *nanoseconds);

double infiltratr_monotonic_seconds(void);

#ifdef __cplusplus
}
#endif

#endif
