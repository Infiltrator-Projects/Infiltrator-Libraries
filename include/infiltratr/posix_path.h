// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file posix_path.h
 * @brief Small POSIX lexical-path primitives shared by native consumers.
 *
 * These helpers operate on POSIX path syntax only and perform no filesystem
 * access. They are separate from the portable target because slash semantics
 * are part of the POSIX provider contract.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#ifndef INFILTRATR_COMMON_POSIX_PATH_H
#define INFILTRATR_COMMON_POSIX_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the final lexical component of a POSIX path.
 *
 * The returned pointer aliases `path` and must not be freed. NULL maps to the
 * empty string. A trailing slash therefore has an empty final component, which
 * matches the simple `strrchr(path, '/') + 1` semantics used by consumers.
 */
const char *infiltratr_path_basename(const char *path);

#ifdef __cplusplus
}
#endif

#endif
