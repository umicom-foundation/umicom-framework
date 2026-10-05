/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/closed_response.c
 * PURPOSE: Serialize one close-delimited connection response without accepting conflicting framing from handlers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/connection.h"
#include "connection_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus Append(char *out, size_t *used, const void *bytes, size_t length)
{
    if (length > UMI_WEB_CONNECTION_RESPONSE_LIMIT - *used)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length != 0U)
        memcpy(out + *used, bytes, length);
    *used += length;
    return UMI_STATUS_OK;
}
UmiStatus UmiWebResponseFormatClosed(const UmiWebResponse *response, bool head, void *out, size_t capacity,
                                     size_t *out_length)
{
    if (out_length != NULL)
        *out_length = 0U;
    if (response == NULL || out == NULL || out_length == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (response->status < 200 || response->status > 599 || response->header_count > UMI_WEB_MAX_HEADERS ||
        response->body_length >= UMI_WEB_BODY_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < response->header_count; ++i)
    {
        const UmiWebHeader *h = &response->headers[i];
        const char *name_end = memchr(h->name, '\0', sizeof(h->name)),
                   *value_end = memchr(h->value, '\0', sizeof(h->value));
        if (name_end == NULL || value_end == NULL || name_end == h->name)
            return UMI_STATUS_INVALID_ARGUMENT;
        size_t name_bytes = (size_t)(name_end - h->name);
        for (size_t j = 0U; j < name_bytes; ++j)
            if (!UmiWebFieldToken((unsigned char)h->name[j]))
                return UMI_STATUS_INVALID_ARGUMENT;
        for (const char *p = h->value; p != value_end; ++p)
            if (((unsigned char)*p < 0x20U && *p != '\t') || (unsigned char)*p == 0x7fU)
                return UMI_STATUS_INVALID_ARGUMENT;
        if (UmiWebFieldEqual((const unsigned char *)h->name, name_bytes, "content-length") ||
            UmiWebFieldEqual((const unsigned char *)h->name, name_bytes, "transfer-encoding") ||
            UmiWebFieldEqual((const unsigned char *)h->name, name_bytes, "connection"))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Build separately so invalid capacity never leaves a partly serialized
     * response in caller-owned output. This also bounds the worker stack. */
    char *wire = malloc(UMI_WEB_CONNECTION_RESPONSE_LIMIT);
    if (wire == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t used = 0U;
    char line[128];
    int n = snprintf(line, sizeof(line), "HTTP/1.1 %d %s\r\n", response->status,
                     umi_http_status_reason(response->status));
    UmiStatus status =
        n < 0 || (size_t)n >= sizeof(line) ? UMI_STATUS_INTERNAL_ERROR : Append(wire, &used, line, (size_t)n);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < response->header_count; ++i)
    {
        const UmiWebHeader *h = &response->headers[i];
        status = Append(wire, &used, h->name, strlen(h->name));
        if (status == UMI_STATUS_OK)
            status = Append(wire, &used, ": ", 2U);
        if (status == UMI_STATUS_OK)
            status = Append(wire, &used, h->value, strlen(h->value));
        if (status == UMI_STATUS_OK)
            status = Append(wire, &used, "\r\n", 2U);
    }
    bool body_forbidden = response->status == 204 || response->status == 304;
    if (status == UMI_STATUS_OK && !body_forbidden)
    {
        n = snprintf(line, sizeof(line), "Content-Length: %zu\r\n", response->body_length);
        status = n < 0 || (size_t)n >= sizeof(line) ? UMI_STATUS_INTERNAL_ERROR
                                                    : Append(wire, &used, line, (size_t)n);
    }
    if (status == UMI_STATUS_OK)
        status = Append(wire, &used, "Connection: close\r\n\r\n", 21U);
    if (status == UMI_STATUS_OK && !head && !body_forbidden)
        status = Append(wire, &used, response->body, response->body_length);
    if (status == UMI_STATUS_OK && used > capacity)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
    {
        memcpy(out, wire, used);
        *out_length = used;
    }
    free(wire);
    return status;
}
