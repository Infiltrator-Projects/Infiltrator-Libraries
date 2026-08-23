// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file posix.h
 * @brief POSIX file, path and monotonic-clock adapters for the shared C core.
 *
 * The rich `*_ex` file readers preserve meaningful failure categories and are
 * preferred when a caller needs to distinguish missing data, permission
 * failures, truncation and invalid values. The older boolean/value helpers are
 * intentionally simpler compatibility APIs and collapse those distinctions.
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

/** Detailed result for shared file-reading operations. */
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

/** Permission policy for durable atomic file replacement. */
typedef enum {
    /** Restrict the completed file to its owning user (0600). */
    INFILTRATR_ATOMIC_FILE_PRIVATE = 0,
    /** Preserve an existing regular file's mode; otherwise use 0600. */
    INFILTRATR_ATOMIC_FILE_PRESERVE_PERMISSIONS
} InfiltratrAtomicFileMode;

/** Writer callback used by infiltratr_atomic_file_write(). */
typedef bool (*InfiltratrAtomicFileWriter)(FILE *stream,
                                           const void *user_data);

/**
 * Return a stable machine-readable name for an I/O result.
 *
 * Every declared enum value has a fixed lowercase hyphenated name. Values
 * outside the enumeration return the literal string `unknown`; the returned
 * pointer refers to static storage and must not be freed or modified.
 */
const char *infiltratr_io_result_name(InfiltratrIoResult result);

/**
 * Resolve an existing path with `realpath` into caller-owned bounded storage.
 *
 * A valid destination buffer is cleared before resolution. Resolution failure
 * or insufficient output capacity returns false and leaves an empty string.
 * No allocation escapes the function.
 */
bool infiltratr_realpath_copy(const char *path, char *destination, size_t size);

/**
 * Concatenate two path fragments without inserting a separator.
 *
 * On capacity failure, a valid destination is cleared. Invalid arguments are
 * rejected without a general guarantee that pre-existing destination contents
 * are modified. Source and destination storage must not overlap.
 */
bool infiltratr_path_concat(char *destination, size_t size,
                            const char *base, const char *suffix);

/**
 * Join two path fragments with exactly one separator at their boundary.
 *
 * When `left` is non-empty, a trailing slash on `left` is reused and leading
 * slashes on `right` are removed; otherwise one slash is inserted. With an
 * empty `left`, `right` is copied unchanged. Capacity failure clears a valid
 * destination. Source and destination storage must not overlap.
 */
bool infiltratr_path_join(char *destination, size_t size,
                          const char *left, const char *right);

/**
 * Select the first readable path from ordered suffix candidates.
 *
 * Each non-NULL suffix is concatenated directly to `base`; no separator is
 * inserted. Candidates that do not fit in the caller buffer or are not
 * readable are skipped. On success `destination` receives the first readable
 * candidate. Failure leaves a valid destination buffer empty.
 */
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
 * `INFILTRATR_IO_EMPTY`. `INFILTRATR_IO_TRUNCATED` preserves the bounded prefix
 * in `buffer` rather than discarding it.
 */
InfiltratrIoResult infiltratr_read_text_file_ex(const char *path,
                                                char *buffer, size_t size,
                                                size_t *length);

/**
 * Read and parse one complete unsigned decimal file value.
 *
 * File status is propagated unchanged. Text that is successfully read but is
 * not a complete unsigned base-10 value returns
 * `INFILTRATR_IO_INVALID_VALUE`. `*value` changes only on successful parsing.
 */
InfiltratrIoResult infiltratr_read_u64_file_ex(const char *path,
                                               uint64_t *value);

/**
 * Read and parse one complete finite ASCII-decimal file value.
 *
 * Parsing uses the locale-independent core decimal grammar. File status is
 * propagated unchanged and invalid numeric text returns
 * `INFILTRATR_IO_INVALID_VALUE`. `*value` changes only on success.
 */
InfiltratrIoResult infiltratr_read_double_file_ex(const char *path,
                                                  double *value);

/**
 * Read one bounded pseudo-file value and remove trailing CR/LF bytes.
 *
 * This compatibility helper returns only success/failure and does not probe
 * for data beyond `size - 1`; therefore an oversized file may be silently
 * truncated. Use `infiltratr_read_text_file_ex` when truncation must be known.
 */
bool infiltratr_read_text_file(const char *path, char *buffer, size_t size);

/** Parse a complete unsigned decimal value using the simple text-file reader. */
bool infiltratr_read_u64_file(const char *path, uint64_t *value);

/**
 * Read an unsigned value, mapping every unavailable or invalid state to zero.
 *
 * Use the boolean or rich reader when a genuine stored zero must remain
 * distinguishable from failure.
 */
uint64_t infiltratr_read_u64_or_zero(const char *path);

/** Parse a complete finite floating-point value using the simple text-file reader. */
bool infiltratr_read_double_file(const char *path, double *value);

/**
 * Read a finite floating-point value, mapping every failure to `NAN`.
 *
 * The function is intended for callers where NaN is already the established
 * unavailable-value representation.
 */
double infiltratr_read_double_or_nan(const char *path);

/**
 * Read the first successfully parsed unsigned attribute from ordered suffixes.
 *
 * Each suffix is concatenated directly to `base` in a fixed 512-byte temporary
 * path buffer. Overlong candidates and unreadable/invalid candidates are
 * skipped. The first successful value is returned; false means none succeeded.
 */
bool infiltratr_read_first_u64(const char *base,
                               const char *const *suffixes,
                               size_t suffix_count, uint64_t *value);

/**
 * Durably replace a file with content supplied by a callback.
 *
 * A temporary file is created beside the destination, assigned the requested
 * permission policy, written and fsynced before rename. The containing
 * directory is then fsynced so successful return means the replacement is
 * durable across a power loss. The callback must not close `stream`.
 * Returning false abandons the replacement and preserves the old destination.
 *
 * @return Zero on durable success; otherwise an errno-compatible error value.
 */
int infiltratr_atomic_file_write(const char *path,
                                 InfiltratrAtomicFileMode mode,
                                 InfiltratrAtomicFileWriter writer,
                                 const void *user_data);

/**
 * Durably replace a file with an in-memory byte sequence.
 *
 * `data` may be NULL only when `length` is zero.
 *
 * @return Zero on durable success; otherwise an errno-compatible error value.
 */
int infiltratr_atomic_file_write_bytes(const char *path,
                                       InfiltratrAtomicFileMode mode,
                                       const void *data, size_t length);

/**
 * Return `CLOCK_MONOTONIC` as exact integer nanoseconds when representable.
 *
 * NULL output, clock failure, invalid `timespec` fields or uint64 overflow
 * return false. A valid output is initialised to zero before the clock call and
 * receives the timestamp only on success.
 */
bool infiltratr_monotonic_nanoseconds(uint64_t *nanoseconds);

/**
 * Return `CLOCK_MONOTONIC` in fractional seconds.
 *
 * Clock failure maps to 0.0. Callers that require explicit failure reporting or
 * exact integer time should use `infiltratr_monotonic_nanoseconds`.
 */
double infiltratr_monotonic_seconds(void);

#ifdef __cplusplus
}
#endif

#endif
