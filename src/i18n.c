// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/i18n.h"

#include <ctype.h>
#include <string.h>

static size_t normalise_locale(char *destination, size_t capacity,
                               const char *source)
{
    size_t out = 0U;
    size_t segment_start = 0U;
    size_t segment_length = 0U;
    size_t i;

    if (destination == NULL || capacity == 0U) return 0U;
    destination[0] = '\0';
    if (source == NULL || source[0] == '\0') return 0U;

    for (i = 0U; source[i] != '\0'; ++i) {
        const unsigned char c = (unsigned char)source[i];
        char mapped;
        if (c == '.' || c == '@') break;
        if (c == '-' || c == '_') {
            if (segment_length == 0U || out + 1U >= capacity) break;
            destination[out++] = '-';
            segment_start = out;
            segment_length = 0U;
            continue;
        }
        if (!isalnum(c)) continue;
        mapped = (char)c;
        if (segment_start == 0U) {
            mapped = (char)tolower(c);
        } else if (segment_length < 2U) {
            mapped = (char)toupper(c);
        }
        if (out + 1U >= capacity) break;
        destination[out++] = mapped;
        ++segment_length;
    }
    while (out > 0U && destination[out - 1U] == '-') --out;
    destination[out] = '\0';
    return out;
}

static size_t language_length(const char *locale)
{
    size_t length = 0U;
    if (locale == NULL) return 0U;
    while (locale[length] != '\0' && locale[length] != '-') ++length;
    return length;
}

static int same_language(const char *left, const char *right)
{
    size_t left_length = language_length(left);
    size_t right_length = language_length(right);
    if (left_length == 0U || left_length != right_length) return 0;
    return strncmp(left, right, left_length) == 0;
}

static const InfiltratrI18nCatalog *find_catalog_exact(
    const InfiltratrI18n *context, const char *locale)
{
    size_t i;
    char normalised[INFILTRATR_I18N_LOCALE_CAPACITY];
    if (context == NULL || locale == NULL) return NULL;
    for (i = 0U; i < context->catalog_count; ++i) {
        normalise_locale(normalised, sizeof(normalised),
                         context->catalogs[i].locale);
        if (strcmp(normalised, locale) == 0) return &context->catalogs[i];
    }
    return NULL;
}

static const InfiltratrI18nCatalog *find_catalog_language(
    const InfiltratrI18n *context, const char *locale)
{
    size_t i;
    char normalised[INFILTRATR_I18N_LOCALE_CAPACITY];
    const InfiltratrI18nCatalog *first_match = NULL;
    if (context == NULL || locale == NULL) return NULL;
    for (i = 0U; i < context->catalog_count; ++i) {
        normalise_locale(normalised, sizeof(normalised),
                         context->catalogs[i].locale);
        if (!same_language(normalised, locale)) continue;
        if (strchr(normalised, '-') == NULL) return &context->catalogs[i];
        if (first_match == NULL) first_match = &context->catalogs[i];
    }
    return first_match;
}

static const char *catalog_lookup(const InfiltratrI18nCatalog *catalog,
                                  const char *key)
{
    size_t i;
    if (catalog == NULL || key == NULL || catalog->entries == NULL) return NULL;
    for (i = 0U; i < catalog->entry_count; ++i) {
        if (catalog->entries[i].key != NULL &&
            strcmp(catalog->entries[i].key, key) == 0)
            return catalog->entries[i].value;
    }
    return NULL;
}

static const char *lookup_locale(const InfiltratrI18n *context,
                                 const char *locale, const char *key)
{
    const InfiltratrI18nCatalog *catalog;
    const char *value;
    catalog = find_catalog_exact(context, locale);
    value = catalog_lookup(catalog, key);
    if (value != NULL) return value;
    catalog = find_catalog_language(context, locale);
    return catalog_lookup(catalog, key);
}

bool infiltratr_i18n_init(InfiltratrI18n *context,
                          const InfiltratrI18nCatalog *catalogs,
                          size_t catalog_count,
                          const char *fallback_locale)
{
    if (context == NULL || catalogs == NULL || catalog_count == 0U ||
        fallback_locale == NULL) return false;
    memset(context, 0, sizeof(*context));
    context->catalogs = catalogs;
    context->catalog_count = catalog_count;
    if (normalise_locale(context->fallback_locale,
                         sizeof(context->fallback_locale),
                         fallback_locale) == 0U)
        return false;
    (void)strncpy(context->locale, context->fallback_locale,
                  sizeof(context->locale) - 1U);
    return find_catalog_language(context, context->fallback_locale) != NULL;
}

bool infiltratr_i18n_set_locale(InfiltratrI18n *context, const char *locale)
{
    char normalised[INFILTRATR_I18N_LOCALE_CAPACITY];
    if (context == NULL || locale == NULL) return false;
    if (normalise_locale(normalised, sizeof(normalised), locale) == 0U)
        return false;
    (void)strncpy(context->locale, normalised, sizeof(context->locale) - 1U);
    context->locale[sizeof(context->locale) - 1U] = '\0';
    return find_catalog_language(context, context->locale) != NULL;
}

const char *infiltratr_i18n_locale(const InfiltratrI18n *context)
{
    if (context == NULL) return "";
    return context->locale;
}

const char *infiltratr_i18n_get(const InfiltratrI18n *context,
                                const char *key)
{
    const char *value;
    if (key == NULL) return "";
    if (context == NULL) return key;
    value = lookup_locale(context, context->locale, key);
    if (value != NULL) return value;
    if (strcmp(context->locale, context->fallback_locale) != 0) {
        value = lookup_locale(context, context->fallback_locale, key);
        if (value != NULL) return value;
    }
    return key;
}

static const char *find_argument(const InfiltratrI18nArgument *arguments,
                                 size_t argument_count,
                                 const char *name, size_t name_length)
{
    size_t i;
    if (arguments == NULL || name == NULL) return NULL;
    for (i = 0U; i < argument_count; ++i) {
        if (arguments[i].name == NULL || arguments[i].value == NULL) continue;
        if (strlen(arguments[i].name) == name_length &&
            strncmp(arguments[i].name, name, name_length) == 0)
            return arguments[i].value;
    }
    return NULL;
}

static void append_bytes(char *destination, size_t capacity, size_t *written,
                         const char *source, size_t length)
{
    size_t i;
    if (written == NULL || source == NULL) return;
    for (i = 0U; i < length; ++i) {
        if (destination != NULL && capacity > 0U && *written + 1U < capacity)
            destination[*written] = source[i];
        ++(*written);
    }
}

size_t infiltratr_i18n_format(char *destination, size_t capacity,
                              const char *format,
                              const InfiltratrI18nArgument *arguments,
                              size_t argument_count)
{
    size_t written = 0U;
    size_t i = 0U;
    if (destination != NULL && capacity > 0U) destination[0] = '\0';
    if (format == NULL) return 0U;
    while (format[i] != '\0') {
        if (format[i] == '{') {
            size_t end = i + 1U;
            while (format[end] != '\0' && format[end] != '}') ++end;
            if (format[end] == '}') {
                const char *argument = find_argument(arguments, argument_count,
                                                     format + i + 1U,
                                                     end - i - 1U);
                if (argument != NULL) {
                    append_bytes(destination, capacity, &written, argument,
                                 strlen(argument));
                    i = end + 1U;
                    continue;
                }
                append_bytes(destination, capacity, &written, format + i,
                             end - i + 1U);
                i = end + 1U;
                continue;
            }
        }
        append_bytes(destination, capacity, &written, format + i, 1U);
        ++i;
    }
    if (destination != NULL && capacity > 0U) {
        size_t terminator = written < capacity ? written : capacity - 1U;
        destination[terminator] = '\0';
    }
    return written;
}
