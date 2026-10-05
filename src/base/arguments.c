/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/arguments.c
 * PURPOSE: Keep program arguments identical across direct launches and debugger launches.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/arguments.h"
#include <ctype.h>
#include <string.h>

/* Build and language tools share this grammar so switching between Run and
 * Debug cannot change argument boundaries. Parsing remains independent of the
 * operating system's shell and does not execute any part of the input. */
UmiStatus UmiArgumentsParse(const char *text, UmiArguments *out)
{
    UmiStatus status = UMI_STATUS_OK;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const char *cursor = text;
    while (*cursor != '\0') {
        while (isspace((unsigned char)*cursor)) ++cursor;
        if (*cursor == '\0') break;
        if (out->count == UMI_ARGUMENTS_CAPACITY) {
            status = UMI_STATUS_CAPACITY_EXCEEDED; break;
        }
        size_t used = 0U;
        char quote = '\0';
        char *value = out->storage[out->count];
        while (*cursor != '\0') {
            char next = *cursor;
            if (quote == '\0' && isspace((unsigned char)next)) break;
            if (next == '\'' || next == '"') {
                if (quote == '\0') { quote = next; ++cursor; continue; }
                if (quote == next) { quote = '\0'; ++cursor; continue; }
            }
            if (next == '\\' && (cursor[1] == '\'' || cursor[1] == '"' ||
                cursor[1] == '\\' || isspace((unsigned char)cursor[1]))) {
                next = cursor[1]; cursor += 2;
            } else ++cursor;
            if (used + 1U >= UMI_ARGUMENT_TEXT_CAPACITY) {
                status = UMI_STATUS_CAPACITY_EXCEEDED; break;
            }
            value[used++] = next;
        }
        if (status != UMI_STATUS_OK) break;
        if (quote != '\0') { status = UMI_STATUS_PARSE_ERROR; break; }
        value[used] = '\0';
        out->values[out->count++] = value;
    }
    if (status != UMI_STATUS_OK) memset(out, 0, sizeof(*out));
    return status;
}

/* Measure before writing, so a truncated command is never offered for launch.
 * Every argument is quoted. Escaping both quote styles keeps formatting an
 * exact inverse even when the caller supplies mixed quotes or a trailing slash. */
UmiStatus UmiArgumentsFormat(const char *const *values, size_t count,
    char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out[0] = '\0';
    if (count > UMI_ARGUMENTS_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (values == NULL && count != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    size_t needed = 1U;
    for (size_t index = 0U; index < count; ++index) {
        if (values[index] == NULL) return UMI_STATUS_INVALID_ARGUMENT;
        needed += 2U + (index != 0U ? 1U : 0U);
        size_t length = 0U;
        while (values[index][length] != '\0') {
            if (length + 1U >= UMI_ARGUMENT_TEXT_CAPACITY)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            const char c = values[index][length++];
            needed += (c == '\\' || c == '"' || c == '\'') ? 2U : 1U;
        }
    }
    if (needed > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t used = 0U;
    for (size_t index = 0U; index < count; ++index) {
        if (index != 0U) out[used++] = ' ';
        out[used++] = '"';
        for (const char *cursor = values[index]; *cursor != '\0'; ++cursor) {
            if (*cursor == '\\' || *cursor == '"' || *cursor == '\'') out[used++] = '\\';
            out[used++] = *cursor;
        }
        out[used++] = '"';
    }
    out[used] = '\0';
    return UMI_STATUS_OK;
}
