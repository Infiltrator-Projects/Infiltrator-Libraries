// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file config.c
 * @brief Allocation-free simple-configuration parsing implementation.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/config.h"

#include <stddef.h>
#include <string.h>

static bool ascii_space(unsigned char value)
{
    return value == ' ' || value == '\t' || value == '\r' ||
           value == '\n' || value == '\f' || value == '\v';
}

static char ascii_lower(char value)
{
    return value >= 'A' && value <= 'Z' ? (char)(value + ('a' - 'A')) : value;
}

static bool ascii_token_equal(const char *text, size_t length,
                              const char *token)
{
    size_t index = 0U;
    for (; index < length && token[index]; index++)
        if (ascii_lower(text[index]) != ascii_lower(token[index])) return false;
    return index == length && token[index] == '\0';
}

static char *trim_ascii_in_place(char *text)
{
    while (*text && ascii_space((unsigned char)*text)) text++;
    char *end = text + strlen(text);
    while (end > text && ascii_space((unsigned char)end[-1])) end--;
    *end = '\0';
    return text;
}

InfiltratrConfigLineStatus infiltratr_config_parse_line(
    char *line, char **key, char **value)
{
    if (key) *key = NULL;
    if (value) *value = NULL;
    if (!line || !key || !value) return INFILTRATR_CONFIG_LINE_INVALID;

    char *start = trim_ascii_in_place(line);
    if (!*start || *start == '#') return INFILTRATR_CONFIG_LINE_IGNORED;

    char *separator = strchr(start, '=');
    if (!separator) return INFILTRATR_CONFIG_LINE_INVALID;
    *separator = '\0';
    char *parsed_key = trim_ascii_in_place(start);
    char *parsed_value = trim_ascii_in_place(separator + 1);
    if (!*parsed_key) return INFILTRATR_CONFIG_LINE_INVALID;

    *key = parsed_key;
    *value = parsed_value;
    return INFILTRATR_CONFIG_LINE_ENTRY;
}

bool infiltratr_config_parse_bool(const char *text, bool *value)
{
    if (!text || !value) return false;
    while (*text && ascii_space((unsigned char)*text)) text++;
    const char *end = text + strlen(text);
    while (end > text && ascii_space((unsigned char)end[-1])) end--;
    const size_t length = (size_t)(end - text);

    bool parsed;
    if ((length == 1U && text[0] == '1') ||
        ascii_token_equal(text, length, "true") ||
        ascii_token_equal(text, length, "yes"))
        parsed = true;
    else if ((length == 1U && text[0] == '0') ||
             ascii_token_equal(text, length, "false") ||
             ascii_token_equal(text, length, "no"))
        parsed = false;
    else
        return false;

    *value = parsed;
    return true;
}
