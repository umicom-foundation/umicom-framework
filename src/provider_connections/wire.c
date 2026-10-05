/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connections/wire.c
 * PURPOSE: Encode portable connection records and reject ambiguous persisted data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Fixed field order and hexadecimal UTF-8 bytes keep separators unambiguous.
 * This is encoding, not encryption: only public settings and references belong
 * in this format. Decode into temporary storage before publishing a value. */
static bool Append(char **cursor, size_t *remaining, const char *text)
{
    size_t length = strlen(text);
    if (length >= *remaining) return false;
    memcpy(*cursor, text, length + 1U);
    *cursor += length;
    *remaining -= length;
    return true;
}
static bool HexWrite(char **cursor, size_t *remaining, const char *text)
{
    static const char digits[] = "0123456789abcdef";
    size_t length = strlen(text);
    if (length > (*remaining > 1U ? (*remaining - 2U) / 2U : 0U) || *remaining < 2U) return false;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char value = (unsigned char)text[i];
        *(*cursor)++ = digits[value >> 4U];
        *(*cursor)++ = digits[value & 15U];
    }
    *(*cursor)++ = '\n';
    **cursor = '\0';
    *remaining -= length * 2U + 1U;
    return true;
}
static int HexDigit(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}
static bool HexRead(const char **cursor, char *out, size_t capacity)
{
    size_t length = 0U;
    while (**cursor != '\n') {
        int high = HexDigit((*cursor)[0]);
        if (high < 0) return false;
        int low = HexDigit((*cursor)[1]);
        if (low < 0 || (high == 0 && low == 0) || length + 1U >= capacity) return false;
        out[length++] = (char)((unsigned)high * 16U + (unsigned)low);
        *cursor += 2;
    }
    out[length] = '\0';
    ++*cursor;
    return true;
}
/* Read decimal values without locale, signs, leading zeroes or wraparound. */
static bool NumberRead(const char **cursor, uint64_t *out)
{
    const char *p = *cursor;
    if (*p < '0' || *p > '9') return false;
    if (*p == '0' && p[1] != '\n') return false;
    uint64_t value = 0U;
    while (*p >= '0' && *p <= '9') {
        unsigned digit = (unsigned)(*p++ - '0');
        if (value > (UINT64_MAX - digit) / 10U) return false;
        value = value * 10U + digit;
    }
    if (*p != '\n') return false;
    *cursor = p + 1;
    *out = value;
    return true;
}
UmiStatus UmiProviderConnectionEncode(const UmiProviderConnection *connection, char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U || UmiProviderConnectionValidate(connection) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    char *cursor = out;
    size_t remaining = capacity;
    char numbers[64];
    int length = snprintf(numbers, sizeof(numbers), "connection\n%u\n%" PRIu32 "\n%u\n",
        (unsigned)connection->route, connection->timeout_ms, connection->enabled ? 1U : 0U);
    if (length < 0 || (size_t)length >= sizeof(numbers) || !Append(&cursor, &remaining, numbers))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *fields[] = {connection->id, connection->provider_id, connection->label,
        connection->endpoint, connection->model, connection->secret_reference};
    for (size_t i = 0U; i < sizeof(fields) / sizeof(fields[0]); ++i)
        if (!HexWrite(&cursor, &remaining, fields[i])) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderConnectionDecode(const char *wire, UmiProviderConnection *out)
{
    if (wire == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strncmp(wire, "connection\n", 11U) != 0) return UMI_STATUS_PARSE_ERROR;
    const char *cursor = wire + 11;
    uint64_t route, timeout, enabled;
    UmiProviderConnection value = {0};
    if (!NumberRead(&cursor, &route) || route < 1U || route > 2U ||
        !NumberRead(&cursor, &timeout) || timeout > UINT32_MAX ||
        !NumberRead(&cursor, &enabled) || enabled > 1U) return UMI_STATUS_PARSE_ERROR;
    value.route = (UmiProviderConnectionRoute)route;
    value.timeout_ms = (uint32_t)timeout;
    value.enabled = enabled != 0U;
    if (!HexRead(&cursor, value.id, sizeof(value.id)) ||
        !HexRead(&cursor, value.provider_id, sizeof(value.provider_id)) ||
        !HexRead(&cursor, value.label, sizeof(value.label)) ||
        !HexRead(&cursor, value.endpoint, sizeof(value.endpoint)) ||
        !HexRead(&cursor, value.model, sizeof(value.model)) ||
        !HexRead(&cursor, value.secret_reference, sizeof(value.secret_reference)) ||
        *cursor != '\0' || UmiProviderConnectionValidate(&value) != UMI_STATUS_OK)
        return UMI_STATUS_PARSE_ERROR;
    *out = value;
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderConnectionIndexEncode(const UmiProviderConnectionSnapshot *snapshot, char *out, size_t capacity)
{
    if (snapshot == NULL || out == NULL || snapshot->count > UMI_PROVIDER_CONNECTION_LIMIT || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    char numbers[80];
    int length = snprintf(numbers, sizeof(numbers), "catalogue\n%" PRIu64 "\n%zu\n", snapshot->revision, snapshot->count);
    char *cursor = out;
    size_t remaining = capacity;
    if (length < 0 || (size_t)length >= sizeof(numbers) || !Append(&cursor, &remaining, numbers))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < snapshot->count; ++i) {
        if (!UmiProviderConnectionIdValid(snapshot->items[i].id, sizeof(snapshot->items[i].id)))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (!Append(&cursor, &remaining, snapshot->items[i].id) || !Append(&cursor, &remaining, "\n"))
            return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderConnectionIndexDecode(const char *wire, UmiProviderConnectionSnapshot *out)
{
    if (wire == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strncmp(wire, "catalogue\n", 10U) != 0) return UMI_STATUS_PARSE_ERROR;
    const char *cursor = wire + 10;
    uint64_t revision, count;
    if (!NumberRead(&cursor, &revision) || revision == 0U ||
        !NumberRead(&cursor, &count) || count > UMI_PROVIDER_CONNECTION_LIMIT) return UMI_STATUS_PARSE_ERROR;
    out->revision = revision;
    out->count = (size_t)count;
    for (size_t i = 0U; i < out->count; ++i) {
        const char *end = strchr(cursor, '\n');
        if (end == NULL || (size_t)(end - cursor) >= sizeof(out->items[i].id)) return UMI_STATUS_PARSE_ERROR;
        memcpy(out->items[i].id, cursor, (size_t)(end - cursor));
        out->items[i].id[end - cursor] = '\0';
        if (!UmiProviderConnectionIdValid(out->items[i].id, sizeof(out->items[i].id))) return UMI_STATUS_PARSE_ERROR;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(out->items[j].id, out->items[i].id) == 0) return UMI_STATUS_PARSE_ERROR;
        cursor = end + 1;
    }
    return *cursor == '\0' ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
