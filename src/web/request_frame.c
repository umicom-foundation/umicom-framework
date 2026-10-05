/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/request_frame.c
 * PURPOSE: Validate HTTP framing before a byte stream can reach a reusable service handler.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/connection.h"
#include "connection_internal.h"
#include <stdlib.h>
#include <string.h>
int UmiWebFieldToken(unsigned char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
           (c != 0U && strchr("!#$%&'*+-.^_`|~", (int)c) != NULL);
}
int UmiWebFieldEqual(const unsigned char *bytes, size_t length, const char *word)
{
    size_t n = strlen(word);
    if (length != n)
        return 0;
    for (size_t i = 0U; i < n; ++i)
    {
        unsigned char c = bytes[i];
        if (c >= 'A' && c <= 'Z')
            c = (unsigned char)(c + ('a' - 'A'));
        if (c != (unsigned char)word[i])
            return 0;
    }
    return 1;
}
/* Locate a CRLF without looking beyond the supplied span. An incomplete CR
 * remains incomplete; bare LF or any other use of CR is malformed. */
static UmiStatus Line(const unsigned char *bytes, size_t length, size_t begin, size_t *end)
{
    for (size_t i = begin; i < length; ++i)
    {
        if (bytes[i] == '\n')
            return UMI_STATUS_PARSE_ERROR;
        if (bytes[i] == '\r')
        {
            if (i + 1U == length)
                return UMI_STATUS_BUSY;
            if (bytes[i + 1U] != '\n')
                return UMI_STATUS_PARSE_ERROR;
            *end = i;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_BUSY;
}
/* A bounded two-pass parser shares all rules. The first pass admits a whole
 * frame; only then is a temporary request allocated and filled. No partial
 * header set can replace the caller's previous request. */
static UmiStatus Frame(const unsigned char *bytes, size_t length, size_t maximum, UmiWebRequestFrame *frame,
                       UmiWebRequest *request)
{
    size_t end = 0U;
    UmiStatus status = Line(bytes, length, 0U, &end);
    if (status != UMI_STATUS_OK)
        return status;
    const unsigned char *first = memchr(bytes, ' ', end);
    if (first == NULL)
        return UMI_STATUS_PARSE_ERROR;
    size_t method_bytes = (size_t)(first - bytes), target_begin = method_bytes + 1U;
    const unsigned char *second = memchr(bytes + target_begin, ' ', end - target_begin);
    if (second == NULL)
        return UMI_STATUS_PARSE_ERROR;
    size_t target_bytes = (size_t)(second - (bytes + target_begin)),
           version_begin = (size_t)(second - bytes) + 1U;
    if (method_bytes == 0U || method_bytes >= UMI_WEB_METHOD_CAPACITY || target_bytes == 0U ||
        bytes[target_begin] != '/')
        return UMI_STATUS_PARSE_ERROR;
    if (end - version_begin != 8U || memcmp(bytes + version_begin, "HTTP/1.1", 8U) != 0)
        return UMI_STATUS_NOT_IMPLEMENTED;
    for (size_t i = 0U; i < method_bytes; ++i)
        if (!UmiWebFieldToken(bytes[i]))
            return UMI_STATUS_PARSE_ERROR;
    for (size_t i = target_begin; i < target_begin + target_bytes; ++i)
        if (bytes[i] <= 0x20U || bytes[i] >= 0x7fU || bytes[i] == '#' || bytes[i] == '\\')
            return UMI_STATUS_PARSE_ERROR;
    if (target_bytes >= UMI_WEB_PATH_CAPACITY + UMI_WEB_QUERY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const unsigned char *query = memchr(bytes + target_begin, '?', target_bytes);
    size_t path_bytes = query != NULL ? (size_t)(query - (bytes + target_begin)) : target_bytes;
    if (path_bytes >= UMI_WEB_PATH_CAPACITY ||
        (query != NULL && target_bytes - path_bytes - 1U >= UMI_WEB_QUERY_CAPACITY))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char method[UMI_WEB_METHOD_CAPACITY], target[UMI_WEB_PATH_CAPACITY + UMI_WEB_QUERY_CAPACITY];
    memcpy(method, bytes, method_bytes);
    method[method_bytes] = '\0';
    UmiHttpMethod verb = umi_http_method_from_text(method);
    if (verb == UMI_HTTP_METHOD_UNKNOWN)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (request != NULL)
    {
        memcpy(target, bytes + target_begin, target_bytes);
        target[target_bytes] = '\0';
        request->method = verb;
        memcpy(request->version, "HTTP/1.1", 9U);
        status = umi_web_request_set_target(request, target);
        if (status != UMI_STATUS_OK)
            return status;
    }
    size_t position = end + 2U, headers = 0U, body_bytes = 0U;
    int hosts = 0, lengths = 0;
    for (;;)
    {
        status = Line(bytes, length, position, &end);
        if (status != UMI_STATUS_OK)
            return status;
        if (end == position)
        {
            position = end + 2U;
            break;
        }
        if (headers == UMI_WEB_MAX_HEADERS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        const unsigned char *colon = memchr(bytes + position, ':', end - position);
        if (colon == NULL)
            return UMI_STATUS_PARSE_ERROR;
        size_t name_bytes = (size_t)(colon - (bytes + position));
        if (name_bytes == 0U || name_bytes >= UMI_WEB_HEADER_NAME_CAPACITY)
            return UMI_STATUS_PARSE_ERROR;
        for (size_t i = position; i < position + name_bytes; ++i)
            if (!UmiWebFieldToken(bytes[i]))
                return UMI_STATUS_PARSE_ERROR;
        size_t value_begin = (size_t)(colon - bytes) + 1U, value_end = end;
        while (value_begin < value_end && (bytes[value_begin] == ' ' || bytes[value_begin] == '\t'))
            ++value_begin;
        while (value_end > value_begin && (bytes[value_end - 1U] == ' ' || bytes[value_end - 1U] == '\t'))
            --value_end;
        size_t value_bytes = value_end - value_begin;
        if (value_bytes >= UMI_WEB_HEADER_VALUE_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t i = value_begin; i < value_end; ++i)
            if ((bytes[i] < 0x20U && bytes[i] != '\t') || bytes[i] == 0x7fU)
                return UMI_STATUS_PARSE_ERROR;
        const unsigned char *name = bytes + position;
        if (UmiWebFieldEqual(name, name_bytes, "host"))
        {
            if (++hosts != 1 || value_bytes == 0U)
                return UMI_STATUS_PARSE_ERROR;
            for (size_t i = value_begin; i < value_end; ++i)
                if (bytes[i] <= 0x20U || bytes[i] >= 0x7fU || strchr("/@?#\\", (int)bytes[i]) != NULL)
                    return UMI_STATUS_PARSE_ERROR;
        }
        if (UmiWebFieldEqual(name, name_bytes, "content-length"))
        {
            if (++lengths != 1 || value_bytes == 0U)
                return UMI_STATUS_PARSE_ERROR;
            for (size_t i = value_begin; i < value_end; ++i)
            {
                if (bytes[i] < '0' || bytes[i] > '9')
                    return UMI_STATUS_PARSE_ERROR;
                size_t digit = (size_t)(bytes[i] - '0');
                if (digit > maximum || body_bytes > (maximum - digit) / 10U)
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                body_bytes = body_bytes * 10U + digit;
            }
        }
        if (UmiWebFieldEqual(name, name_bytes, "transfer-encoding") ||
            UmiWebFieldEqual(name, name_bytes, "expect") || UmiWebFieldEqual(name, name_bytes, "upgrade"))
            return UMI_STATUS_NOT_IMPLEMENTED;
        if (request != NULL)
        {
            UmiWebHeader *header = &request->headers[headers];
            memcpy(header->name, name, name_bytes);
            header->name[name_bytes] = '\0';
            memcpy(header->value, bytes + value_begin, value_bytes);
            header->value[value_bytes] = '\0';
        }
        ++headers;
        position = end + 2U;
    }
    if (hosts != 1)
        return UMI_STATUS_PARSE_ERROR;
    if (body_bytes >= UMI_WEB_BODY_CAPACITY || position > maximum || body_bytes > maximum - position)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t complete = position + body_bytes;
    if (length < complete)
        return UMI_STATUS_BUSY;
    if (length != complete)
        return UMI_STATUS_PARSE_ERROR;
    if (request != NULL)
    {
        request->header_count = headers;
        request->body_length = body_bytes;
        memcpy(request->body, bytes + position, body_bytes);
        request->body[body_bytes] = '\0';
    }
    *frame = (UmiWebRequestFrame){position, body_bytes, complete};
    return UMI_STATUS_OK;
}
UmiStatus UmiWebRequestFrameRead(const void *bytes, size_t length, size_t maximum, UmiWebRequestFrame *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if ((bytes == NULL && length != 0U) || maximum == 0U || maximum > UMI_WEB_CONNECTION_REQUEST_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > maximum)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U)
        return UMI_STATUS_BUSY;
    UmiStatus status = Frame(bytes, length, maximum, out, NULL);
    if (status == UMI_STATUS_BUSY && length == maximum)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return status;
}
UmiStatus UmiWebRequestParseStrict(const void *bytes, size_t length, size_t maximum, UmiWebRequest *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiWebRequestFrame frame;
    UmiStatus status = UmiWebRequestFrameRead(bytes, length, maximum, &frame);
    if (status != UMI_STATUS_OK)
        return status;
    UmiWebRequest *candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = Frame(bytes, length, maximum, &frame, candidate);
    if (status == UMI_STATUS_OK)
        *out = *candidate;
    free(candidate);
    return status;
}
