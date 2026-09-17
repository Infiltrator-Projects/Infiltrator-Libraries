// SPDX-License-Identifier: GPL-3.0-or-later
#include <infiltratr/escape.h>

#include <stdint.h>
#include <string.h>

typedef enum {
    INFILTRATR_ESCAPE_MODE_HTML,
    INFILTRATR_ESCAPE_MODE_JSON,
    INFILTRATR_ESCAPE_MODE_URI
} InfiltratrEscapeMode;

static bool size_add(size_t *value, size_t amount)
{
    if (!value || *value > SIZE_MAX - amount) return false;
    *value += amount;
    return true;
}

static bool uri_unreserved(unsigned char value)
{
    return (value >= (unsigned char)'A' && value <= (unsigned char)'Z') ||
           (value >= (unsigned char)'a' && value <= (unsigned char)'z') ||
           (value >= (unsigned char)'0' && value <= (unsigned char)'9') ||
           value == (unsigned char)'-' || value == (unsigned char)'.' ||
           value == (unsigned char)'_' || value == (unsigned char)'~';
}

static size_t encoded_length(InfiltratrEscapeMode mode, unsigned char value)
{
    if (mode == INFILTRATR_ESCAPE_MODE_URI)
        return uri_unreserved(value) ? 1U : 3U;

    if (mode == INFILTRATR_ESCAPE_MODE_HTML) {
        switch (value) {
        case '&': return 5U;
        case '<': return 4U;
        case '>': return 4U;
        case '"': return 6U;
        case '\'': return 5U;
        default: return 1U;
        }
    }

    switch (value) {
    case '"':
    case '\\':
    case '\b':
    case '\f':
    case '\n':
    case '\r':
    case '\t':
        return 2U;
    default:
        return value < 0x20U ? 6U : 1U;
    }
}

static bool measure(InfiltratrEscapeMode mode, const char *input, size_t *required)
{
    if (!input || !required) return false;
    size_t total = 1U;
    for (const unsigned char *p = (const unsigned char *)input; *p; ++p) {
        if (!size_add(&total, encoded_length(mode, *p))) return false;
    }
    *required = total;
    return true;
}

static char hex_digit(unsigned value)
{
    static const char digits[] = "0123456789ABCDEF";
    return digits[value & 0x0fU];
}

static char *emit_html(char *out, unsigned char value)
{
    const char *replacement = NULL;
    switch (value) {
    case '&': replacement = "&amp;"; break;
    case '<': replacement = "&lt;"; break;
    case '>': replacement = "&gt;"; break;
    case '"': replacement = "&quot;"; break;
    case '\'': replacement = "&#39;"; break;
    default:
        *out++ = (char)value;
        return out;
    }
    const size_t length = strlen(replacement);
    memcpy(out, replacement, length);
    return out + length;
}

static char *emit_json(char *out, unsigned char value)
{
    switch (value) {
    case '"': *out++ = '\\'; *out++ = '"'; return out;
    case '\\': *out++ = '\\'; *out++ = '\\'; return out;
    case '\b': *out++ = '\\'; *out++ = 'b'; return out;
    case '\f': *out++ = '\\'; *out++ = 'f'; return out;
    case '\n': *out++ = '\\'; *out++ = 'n'; return out;
    case '\r': *out++ = '\\'; *out++ = 'r'; return out;
    case '\t': *out++ = '\\'; *out++ = 't'; return out;
    default:
        if (value < 0x20U) {
            *out++ = '\\'; *out++ = 'u'; *out++ = '0'; *out++ = '0';
            *out++ = hex_digit(value >> 4U);
            *out++ = hex_digit(value);
            return out;
        }
        *out++ = (char)value;
        return out;
    }
}

static char *emit_uri(char *out, unsigned char value)
{
    if (uri_unreserved(value)) {
        *out++ = (char)value;
        return out;
    }
    *out++ = '%';
    *out++ = hex_digit(value >> 4U);
    *out++ = hex_digit(value);
    return out;
}

static bool transform(InfiltratrEscapeMode mode, const char *input,
                      char *output, size_t size, size_t *required_size)
{
    size_t required = 0U;
    if (!measure(mode, input, &required)) {
        if (required_size) *required_size = 0U;
        if (output && size) output[0] = '\0';
        return false;
    }
    if (required_size) *required_size = required;

    if (!output) return size == 0U;
    if (size < required) {
        if (size) output[0] = '\0';
        return false;
    }

    char *cursor = output;
    for (const unsigned char *p = (const unsigned char *)input; *p; ++p) {
        if (mode == INFILTRATR_ESCAPE_MODE_HTML) cursor = emit_html(cursor, *p);
        else if (mode == INFILTRATR_ESCAPE_MODE_JSON) cursor = emit_json(cursor, *p);
        else cursor = emit_uri(cursor, *p);
    }
    *cursor = '\0';
    return true;
}

bool infiltratr_escape_html(const char *input, char *output, size_t size,
                            size_t *required_size)
{
    return transform(INFILTRATR_ESCAPE_MODE_HTML, input, output, size, required_size);
}

bool infiltratr_escape_json(const char *input, char *output, size_t size,
                            size_t *required_size)
{
    return transform(INFILTRATR_ESCAPE_MODE_JSON, input, output, size, required_size);
}

bool infiltratr_escape_uri_component(const char *input, char *output, size_t size,
                                     size_t *required_size)
{
    return transform(INFILTRATR_ESCAPE_MODE_URI, input, output, size, required_size);
}
