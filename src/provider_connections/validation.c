/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connections/validation.c
 * PURPOSE: Validate metadata before it reaches persistence or a provider adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <string.h>

static bool Lower(unsigned char value)
{
    return value >= 'a' && value <= 'z';
}
static bool Digit(unsigned char value)
{
    return value >= '0' && value <= '9';
}
/* Bound every scan by the owning field's capacity. A missing terminator is an
 * invalid value, rather than permission to read the next field in a record. */
bool UmiProviderConnectionIdValid(const char *text, size_t capacity)
{
    if (text == NULL || capacity < 2U || !Lower((unsigned char)text[0])) return false;
    for (size_t i = 1U; i < capacity; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == 0U) return true;
        if (!Lower(c) && !Digit(c) && c != '.' && c != '_' && c != '-') return false;
    }
    return false;
}
static bool Bounded(const char *text, size_t capacity, size_t *length)
{
    for (size_t i = 0U; i < capacity; ++i) {
        if (text[i] == '\0') { *length = i; return true; }
    }
    return false;
}
/* Decode Unicode scalar values so labels survive a save/reopen round trip.
 * Reject overlong encodings, controls and noncharacters before display or
 * serialization. This keeps validation independent of the process locale. */
static bool LabelValid(const char *text, size_t capacity)
{
    size_t length = 0U;
    if (!Bounded(text, capacity, &length) || length == 0U) return false;
    bool non_space = false;
    for (size_t i = 0U; i < length;) {
        unsigned char first = (unsigned char)text[i++];
        uint32_t code = first;
        size_t extra = 0U;
        if (first >= 0xC2U && first <= 0xDFU) { code = first & 0x1FU; extra = 1U; }
        else if (first >= 0xE0U && first <= 0xEFU) { code = first & 0x0FU; extra = 2U; }
        else if (first >= 0xF0U && first <= 0xF4U) { code = first & 0x07U; extra = 3U; }
        else if (first >= 0x80U) return false;
        if (extra > length - i) return false;
        for (size_t j = 0U; j < extra; ++j) {
            unsigned char next = (unsigned char)text[i++];
            if ((next & 0xC0U) != 0x80U) return false;
            code = (code << 6U) | (next & 0x3FU);
        }
        if ((extra == 1U && code < 0x80U) || (extra == 2U && code < 0x800U) ||
            (extra == 3U && code < 0x10000U) || code > 0x10FFFFU ||
            (code >= 0xD800U && code <= 0xDFFFU) || code < 0x20U ||
            (code >= 0x7FU && code <= 0x9FU) ||
            (code >= 0xFDD0U && code <= 0xFDEFU) || (code & 0xFFFFU) >= 0xFFFEU) return false;
        if (code != 0x20U) non_space = true;
    }
    return non_space;
}
static bool ModelValid(const char *text)
{
    size_t length = 0U;
    if (!Bounded(text, UMI_PROVIDER_CONNECTION_MODEL_CAPACITY, &length)) return false;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (!Lower(c) && !(c >= 'A' && c <= 'Z') && !Digit(c) &&
            c != '.' && c != '_' && c != '-' && c != '/' && c != ':') return false;
    }
    return true;
}
static bool ReferenceValid(const char *text)
{
    size_t length = 0U;
    if (!Bounded(text, UMI_PROVIDER_CONNECTION_REFERENCE_CAPACITY, &length)) return false;
    if (length == 0U) return true;
    const char *colon = strchr(text, ':');
    if (colon == NULL || (size_t)(colon - text) >= UMI_SECRET_PROVIDER_ID_CAPACITY) return false;
    char provider[UMI_SECRET_PROVIDER_ID_CAPACITY] = {0};
    memcpy(provider, text, (size_t)(colon - text));
    return UmiProviderConnectionIdValid(provider, sizeof(provider)) &&
        UmiProviderConnectionIdValid(colon + 1, 97U);
}
static bool PortValid(const char *begin, const char *end, bool loopback)
{
    if (begin == end || *begin == '0' || end - begin > 5) return false;
    unsigned value = 0U;
    for (const char *p = begin; p != end; ++p) {
        if (!Digit((unsigned char)*p)) return false;
        value = value * 10U + (unsigned)(*p - '0');
    }
    return value <= 65535U && value >= (loopback ? 1024U : 1U);
}
static bool EndpointValid(const UmiProviderConnection *connection)
{
    const char *text = connection->endpoint;
    size_t length = 0U;
    if (!Bounded(text, sizeof(connection->endpoint), &length) || length == 0U) return false;
    bool loopback = connection->route == UMI_PROVIDER_CONNECTION_LOOPBACK;
    const char *prefix = loopback ? "http://127.0.0.1:" : "https://";
    size_t prefix_length = strlen(prefix);
    if (strncmp(text, prefix, prefix_length) != 0) return false;
    const char *authority = text + prefix_length;
    const char *path = strchr(authority, '/');
    const char *end = path != NULL ? path : text + length;
    if (loopback) {
        if (!PortValid(authority, end, true)) return false;
    } else {
        const char *colon = memchr(authority, ':', (size_t)(end - authority));
        const char *host_end = colon != NULL ? colon : end;
        if (authority == host_end || host_end - authority > 253) return false;
        size_t label_length = 0U;
        for (const char *p = authority; p != host_end; ++p) {
            unsigned char c = (unsigned char)*p;
            if (c == '.') {
                if (label_length == 0U || p[-1] == '-') return false;
                label_length = 0U;
            } else {
                if (!Lower(c) && !Digit(c) && c != '-') return false;
                if (label_length == 0U && c == '-') return false;
                if (++label_length > 63U) return false;
            }
        }
        if (label_length == 0U || host_end[-1] == '-') return false;
        if (colon != NULL && !PortValid(colon + 1, end, false)) return false;
    }
    /* Keep credentials out of URL user information and query strings. Narrow
     * paths avoid ambiguous escaping; an adapter may impose stricter rules. */
    if (path != NULL) {
        const char *segment = path + 1;
        for (const char *p = segment;; ++p) {
            unsigned char c = (unsigned char)*p;
            if (c == '/' || c == 0U) {
                size_t size = (size_t)(p - segment);
                if ((size == 1U && segment[0] == '.') ||
                    (size == 2U && segment[0] == '.' && segment[1] == '.')) return false;
                if (size == 0U && c != 0U) return false;
                if (c == 0U) break;
                segment = p + 1;
            } else if (!Lower(c) && !(c >= 'A' && c <= 'Z') && !Digit(c) &&
                c != '.' && c != '_' && c != '-' && c != '~') return false;
        }
    }
    return true;
}
UmiStatus UmiProviderConnectionValidate(const UmiProviderConnection *connection)
{
    if (connection == NULL ||
        !UmiProviderConnectionIdValid(connection->id, sizeof(connection->id)) ||
        !UmiProviderConnectionIdValid(connection->provider_id, sizeof(connection->provider_id)) ||
        !LabelValid(connection->label, sizeof(connection->label)) || !ModelValid(connection->model) ||
        !ReferenceValid(connection->secret_reference) ||
        (connection->route != UMI_PROVIDER_CONNECTION_HTTPS &&
         connection->route != UMI_PROVIDER_CONNECTION_LOOPBACK) ||
        connection->timeout_ms < 100U || connection->timeout_ms > 600000U ||
        (connection->route == UMI_PROVIDER_CONNECTION_LOOPBACK && connection->secret_reference[0] != '\0') ||
        !EndpointValid(connection)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
