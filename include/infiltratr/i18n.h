// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file i18n.h
 * @brief Allocation-free localisation catalogue lookup and interpolation.
 *
 * Applications compile their catalogues into constant tables and use one
 * portable lookup engine on every platform. The engine performs BCP-47-like
 * locale normalisation, language fallback, canonical-locale fallback and
 * named {placeholder} interpolation without owning platform UI policy.
 */
#ifndef INFILTRATR_COMMON_I18N_H
#define INFILTRATR_COMMON_I18N_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INFILTRATR_I18N_LOCALE_CAPACITY 32U

typedef struct {
    const char *key;
    const char *value;
} InfiltratrI18nEntry;

typedef struct {
    const char *locale;
    const InfiltratrI18nEntry *entries;
    size_t entry_count;
} InfiltratrI18nCatalog;

typedef struct {
    const char *name;
    const char *value;
} InfiltratrI18nArgument;

typedef struct {
    const InfiltratrI18nCatalog *catalogs;
    size_t catalog_count;
    char locale[INFILTRATR_I18N_LOCALE_CAPACITY];
    char fallback_locale[INFILTRATR_I18N_LOCALE_CAPACITY];
} InfiltratrI18n;

/** Initialise a localisation context over immutable compiled catalogues. */
bool infiltratr_i18n_init(InfiltratrI18n *context,
                          const InfiltratrI18nCatalog *catalogs,
                          size_t catalog_count,
                          const char *fallback_locale);

/**
 * Select a locale. Underscores are accepted and normalised to hyphens.
 * Returns true when at least one catalogue matches the requested language;
 * otherwise lookups continue through the configured fallback locale.
 */
bool infiltratr_i18n_set_locale(InfiltratrI18n *context, const char *locale);

/** Return the normalised locale currently requested by the caller. */
const char *infiltratr_i18n_locale(const InfiltratrI18n *context);

/**
 * Resolve a translation key using requested locale, language fallback and
 * canonical fallback. If no catalogue contains the key, the key itself is
 * returned so missing translations are visible rather than silently blank.
 */
const char *infiltratr_i18n_get(const InfiltratrI18n *context,
                                const char *key);

/**
 * Interpolate named {placeholders} into a UTF-8 template.
 * Unknown placeholders are retained verbatim. The destination is always
 * NUL-terminated when capacity is non-zero. The return value is the number of
 * bytes that would have been written excluding the terminator.
 */
size_t infiltratr_i18n_format(char *destination, size_t capacity,
                              const char *format,
                              const InfiltratrI18nArgument *arguments,
                              size_t argument_count);

#ifdef __cplusplus
}
#endif

#endif
