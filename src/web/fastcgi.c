/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/fastcgi.c
 *
 * PURPOSE:
 *   Serialize bounded FastCGI responder records and CGI parameters for
 *   Framework-managed web application transports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/fastcgi.h"
#include <string.h>

#define FASTCGI_VERSION 1U
#define FASTCGI_HEADER_BYTES 8U

static UmiStatus Encode(uint8_t type, uint16_t requestId, const void *body,
    size_t bodyBytes, uint8_t *buffer, size_t capacity, size_t *written)
{
    if (buffer == NULL || written == NULL || requestId == 0U || (body == NULL && bodyBytes != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (bodyBytes > UMI_WEB_FASTCGI_CONTENT_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t padding = (8U - (bodyBytes % 8U)) % 8U;
    const size_t required = FASTCGI_HEADER_BYTES + bodyBytes + padding;
    if (capacity < required) return UMI_STATUS_CAPACITY_EXCEEDED;
    const uint8_t header[FASTCGI_HEADER_BYTES] = {FASTCGI_VERSION, type,
        (uint8_t)(requestId >> 8U), (uint8_t)requestId,
        (uint8_t)(bodyBytes >> 8U), (uint8_t)bodyBytes, (uint8_t)padding, 0U};
    memcpy(buffer, header, FASTCGI_HEADER_BYTES);
    if (bodyBytes != 0U) memcpy(buffer + FASTCGI_HEADER_BYTES, body, bodyBytes);
    memset(buffer + FASTCGI_HEADER_BYTES + bodyBytes, 0, padding);
    *written = required;
    return UMI_STATUS_OK;
}

UmiStatus UmiWebFastCgiBegin(uint16_t requestId, uint8_t *buffer, size_t capacity,
    size_t *written)
{
    /* Big-endian responder role 1. The zero flag keeps ownership of each
     * request separate from the transport's future connection policy. */
    const uint8_t begin[] = {0U, 1U, 0U, 0U, 0U, 0U, 0U, 0U};
    return Encode(UMI_WEB_FASTCGI_BEGIN_REQUEST, requestId, begin, sizeof begin, buffer, capacity, written);
}

UmiStatus UmiWebFastCgiInput(UmiWebFastCgiType type, uint16_t requestId,
    const void *body, size_t bodyBytes, uint8_t *buffer, size_t capacity, size_t *written)
{
    if (type != UMI_WEB_FASTCGI_PARAMS && type != UMI_WEB_FASTCGI_STDIN)
        return UMI_STATUS_INVALID_ARGUMENT;
    return Encode((uint8_t)type, requestId, body, bodyBytes, buffer, capacity, written);
}

static size_t EncodeLength(size_t length, uint8_t output[4])
{
    if (length < 128U) {
        output[0] = (uint8_t)length;
        return 1U;
    }
    output[0] = (uint8_t)((length >> 24U) | 0x80U);
    output[1] = (uint8_t)(length >> 16U);
    output[2] = (uint8_t)(length >> 8U);
    output[3] = (uint8_t)length;
    return 4U;
}

UmiStatus UmiWebFastCgiParameter(const char *name, size_t nameBytes, const void *value,
    size_t valueBytes, uint8_t *buffer, size_t capacity, size_t *written)
{
    if (name == NULL || nameBytes == 0U || buffer == NULL || written == NULL ||
        (value == NULL && valueBytes != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (nameBytes > UMI_WEB_FASTCGI_CONTENT_LIMIT || valueBytes > UMI_WEB_FASTCGI_CONTENT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < nameBytes; ++i) {
        const char ch = name[i];
        if (ch != '_' && !(ch >= 'A' && ch <= 'Z') && !(i > 0U && ch >= '0' && ch <= '9'))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (valueBytes != 0U && memchr(value, 0, valueBytes) != NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Assemble both prefixes before changing caller storage. This also
     * checks the single-record budget including the length bytes. */
    uint8_t prefixes[8];
    const size_t namePrefix = EncodeLength(nameBytes, prefixes);
    const size_t prefixBytes = namePrefix + EncodeLength(valueBytes, prefixes + namePrefix);
    const size_t required = prefixBytes + nameBytes + valueBytes;
    if (required > UMI_WEB_FASTCGI_CONTENT_LIMIT || required > capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(buffer, prefixes, prefixBytes);
    memcpy(buffer + prefixBytes, name, nameBytes);
    if (valueBytes != 0U) memcpy(buffer + prefixBytes + nameBytes, value, valueBytes);
    *written = required;
    return UMI_STATUS_OK;
}

UmiStatus UmiWebFastCgiParseRecord(const void *wire, size_t available, UmiWebFastCgiRecord *record)
{
    if (record == NULL || (wire == NULL && available != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (available < FASTCGI_HEADER_BYTES) return UMI_STATUS_UNAVAILABLE;
    const uint8_t *bytes = wire;
    if (bytes[0] != FASTCGI_VERSION || bytes[1] == 0U) return UMI_STATUS_PARSE_ERROR;
    const size_t bodyBytes = ((size_t)bytes[4] << 8U) | bytes[5];
    const size_t consumed = FASTCGI_HEADER_BYTES + bodyBytes + bytes[6];
    if (available < consumed) return UMI_STATUS_UNAVAILABLE;
    *record = (UmiWebFastCgiRecord){.type = bytes[1],
        .requestId = (uint16_t)(((uint16_t)bytes[2] << 8U) | bytes[3]),
        .body = bytes + FASTCGI_HEADER_BYTES, .bodyBytes = bodyBytes, .consumed = consumed};
    return UMI_STATUS_OK;
}
