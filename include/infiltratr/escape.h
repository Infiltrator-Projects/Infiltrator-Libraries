// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file escape.h
 * @brief Dependency-free output encoders for text formats shared by consumers.
 *
 * These helpers escape string content only; they do not add surrounding JSON
 * quotes, HTML elements/attributes, or URI syntax. Bytes >= 0x80 are preserved
 * for HTML/JSON and percent-encoded byte-for-byte for URI components.
 */
#ifndef INFILTRATR_COMMON_ESCAPE_H
#define INFILTRATR_COMMON_ESCAPE_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

bool infiltratr_escape_html(const char *input, char *output, size_t size,
                            size_t *required_size);
bool infiltratr_escape_json(const char *input, char *output, size_t size,
                            size_t *required_size);
bool infiltratr_escape_uri_component(const char *input, char *output, size_t size,
                                     size_t *required_size);

#ifdef __cplusplus
}
#endif

#endif
