/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/fastcgi_response.c
 *
 * PURPOSE:
 *   Enforce the worker-output boundary and publish complete HTTP metadata and body together.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/fastcgi_response.h"
#include "connection_internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct WorkerHeaders {
    UmiWebResponse response;
    size_t declaredLength;
    bool hasStatus;
    bool hasLength;
    bool hasType;
    bool hasLocation;
} WorkerHeaders;

static bool Space(unsigned char byte)
{
    return byte == ' ' || byte == '\t';
}

static bool ConnectionHeader(const unsigned char *name, size_t length)
{
    static const char *const names[] = {
        "connection", "keep-alive", "proxy-connection", "transfer-encoding",
        "te", "trailer", "upgrade", "proxy-authenticate", "proxy-authorization"
    };
    for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
        if (UmiWebFieldEqual(name, length, names[i])) return true;
    return false;
}

static UmiStatus ReadStatus(WorkerHeaders *headers, const unsigned char *value, size_t length)
{
    if (headers->hasStatus || length < 3U || value[0] < '2' || value[0] > '5' ||
        value[1] < '0' || value[1] > '9' || value[2] < '0' || value[2] > '9' ||
        (length > 3U && value[3] != ' '))
        return UMI_STATUS_PARSE_ERROR;
    headers->response.status = (value[0] - '0') * 100 + (value[1] - '0') * 10 + value[2] - '0';
    headers->hasStatus = true;
    return UMI_STATUS_OK;
}

static UmiStatus ReadLength(WorkerHeaders *headers, const unsigned char *value, size_t length)
{
    if (headers->hasLength || length == 0U) return UMI_STATUS_PARSE_ERROR;
    size_t total = 0U;
    for (size_t i = 0U; i < length; ++i) {
        if (value[i] < '0' || value[i] > '9') return UMI_STATUS_PARSE_ERROR;
        const size_t digit = (size_t)(value[i] - '0');
        if (total > (UMI_WEB_RESPONSE_BODY_LIMIT - digit) / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        total = total * 10U + digit;
    }
    headers->declaredLength = total;
    headers->hasLength = true;
    return UMI_STATUS_OK;
}

static UmiStatus ReadField(WorkerHeaders *headers, const unsigned char *line, size_t bytes)
{
    const unsigned char *colon = memchr(line, ':', bytes);
    if (colon == NULL || colon == line) return UMI_STATUS_PARSE_ERROR;
    const size_t nameBytes = (size_t)(colon - line);
    for (size_t i = 0U; i < nameBytes; ++i)
        if (!UmiWebFieldToken(line[i])) return UMI_STATUS_PARSE_ERROR;
    const unsigned char *value = colon + 1U;
    const unsigned char *end = line + bytes;
    for (const unsigned char *p = value; p < end; ++p)
        if ((*p < 0x20U && *p != '\t') || *p == 0x7fU) return UMI_STATUS_PARSE_ERROR;
    while (value < end && Space(*value)) ++value;
    while (value < end && Space(end[-1])) --end;
    const size_t valueBytes = (size_t)(end - value);
    if (nameBytes >= UMI_WEB_HEADER_NAME_CAPACITY || valueBytes >= UMI_WEB_HEADER_VALUE_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Only Framework controls the connection. Canonical header matching is
     * shared with HTTP parsing and framing, including mixed-case worker fields. */
    if (ConnectionHeader(line, nameBytes)) return UMI_STATUS_PARSE_ERROR;
    if (UmiWebFieldEqual(line, nameBytes, "status")) return ReadStatus(headers, value, valueBytes);
    if (UmiWebFieldEqual(line, nameBytes, "content-length"))
        return ReadLength(headers, value, valueBytes);
    if (UmiWebFieldEqual(line, nameBytes, "content-type")) {
        if (headers->hasType || valueBytes == 0U) return UMI_STATUS_PARSE_ERROR;
        headers->hasType = true;
    }
    if (UmiWebFieldEqual(line, nameBytes, "location")) {
        if (headers->hasLocation || valueBytes == 0U) return UMI_STATUS_PARSE_ERROR;
        headers->hasLocation = true;
    }
    char name[UMI_WEB_HEADER_NAME_CAPACITY], text[UMI_WEB_HEADER_VALUE_CAPACITY];
    memcpy(name, line, nameBytes);
    name[nameBytes] = '\0';
    memcpy(text, value, valueBytes);
    text[valueBytes] = '\0';
    return umi_web_response_set_header(&headers->response, name, text);
}

static UmiStatus ParseHeaders(WorkerHeaders *headers, const unsigned char *bytes, size_t length,
    size_t *bodyOffset)
{
    const size_t scan = length < UMI_WEB_FASTCGI_HEADER_LIMIT ? length : UMI_WEB_FASTCGI_HEADER_LIMIT;
    size_t begin = 0U, lineEnding = 0U, fields = 0U;
    while (begin < scan) {
        const unsigned char *newline = memchr(bytes + begin, '\n', scan - begin);
        if (newline == NULL) break;
        const size_t end = (size_t)(newline - bytes);
        const size_t ending = end > begin && bytes[end - 1U] == '\r' ? 2U : 1U;
        if (lineEnding != 0U && ending != lineEnding) return UMI_STATUS_PARSE_ERROR;
        lineEnding = ending;
        const size_t lineBytes = end + 1U - ending - begin;
        if (lineBytes == 0U) {
            *bodyOffset = end + 1U;
            return UMI_STATUS_OK;
        }
        if (fields == UMI_WEB_MAX_HEADERS) return UMI_STATUS_CAPACITY_EXCEEDED;
        const UmiStatus status = ReadField(headers, bytes + begin, lineBytes);
        if (status != UMI_STATUS_OK) return status;
        ++fields;
        begin = end + 1U;
    }
    return scan == UMI_WEB_FASTCGI_HEADER_LIMIT ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_PARSE_ERROR;
}

static UmiStatus ValidateDocument(const WorkerHeaders *headers, size_t bodyBytes)
{
    const int code = headers->response.status;
    const bool noContent = code == 204 || code == 205 || code == 304;
    const bool locationStatus = code == 201 || code == 301 || code == 302 ||
        code == 303 || code == 307 || code == 308;
    if ((headers->hasLength && headers->declaredLength != bodyBytes) ||
        (noContent && bodyBytes != 0U) ||
        (headers->hasLocation && !locationStatus) ||
        (!noContent && !headers->hasType && !(headers->hasLocation && bodyBytes == 0U)))
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}

UmiStatus UmiWebFastCgiResponseDecode(const UmiWebFastCgiResult *result,
    UmiWebResponse *response, UmiWebResponseBody **outBody)
{
    if (result == NULL || response == NULL || outBody == NULL || *outBody != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!result->reply.complete || result->reply.applicationStatus != 0U ||
        result->reply.protocolStatus != UMI_WEB_FASTCGI_REQUEST_COMPLETE ||
        result->reply.stdoutBytes != result->bodyBytes)
        return UMI_STATUS_INVALID_STATE;
    if (result->bodyBytes == 0U || result->body == NULL) return UMI_STATUS_PARSE_ERROR;
    if (result->bodyBytes > UMI_WEB_FASTCGI_EXCHANGE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* The response is large enough to stage on the heap. Output remains
     * untouched until both the header block and the independent body succeed. */
    WorkerHeaders *headers = calloc(1U, sizeof *headers);
    if (headers == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_web_response_init(&headers->response);
    size_t offset = 0U;
    UmiStatus status = ParseHeaders(headers, result->body, result->bodyBytes, &offset);
    UmiWebResponseBody *body = NULL;
    if (status == UMI_STATUS_OK) {
        const size_t bodyBytes = result->bodyBytes - offset;
        status = ValidateDocument(headers, bodyBytes);
        if (status == UMI_STATUS_OK && bodyBytes != 0U)
            status = UmiWebResponseBodyCreate(result->body + offset, bodyBytes, &body);
    }
    if (status == UMI_STATUS_OK) {
        *response = headers->response;
        *outBody = body;
    }
    free(headers);
    return status;
}
