/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/types.c
 *
 * PURPOSE:
 *   Implement common HTTP method, status and bounded text helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Most web modules need the same safe text-copy and enum conversion logic. Centralising it avoids slightly different parsing rules across the server.
 */

#include "umicom/web/types.h"

#include <string.h>

UmiHttpMethod umi_http_method_from_text(const char *text)
{
    if (text == NULL) return UMI_HTTP_METHOD_UNKNOWN;
    if (strcmp(text, "GET") == 0) return UMI_HTTP_METHOD_GET;
    if (strcmp(text, "POST") == 0) return UMI_HTTP_METHOD_POST;
    if (strcmp(text, "PUT") == 0) return UMI_HTTP_METHOD_PUT;
    if (strcmp(text, "PATCH") == 0) return UMI_HTTP_METHOD_PATCH;
    if (strcmp(text, "DELETE") == 0) return UMI_HTTP_METHOD_DELETE;
    if (strcmp(text, "OPTIONS") == 0) return UMI_HTTP_METHOD_OPTIONS;
    if (strcmp(text, "HEAD") == 0) return UMI_HTTP_METHOD_HEAD;
    return UMI_HTTP_METHOD_UNKNOWN;
}

const char *umi_http_method_text(UmiHttpMethod method)
{
    switch (method) {
        case UMI_HTTP_METHOD_GET: return "GET";
        case UMI_HTTP_METHOD_POST: return "POST";
        case UMI_HTTP_METHOD_PUT: return "PUT";
        case UMI_HTTP_METHOD_PATCH: return "PATCH";
        case UMI_HTTP_METHOD_DELETE: return "DELETE";
        case UMI_HTTP_METHOD_OPTIONS: return "OPTIONS";
        case UMI_HTTP_METHOD_HEAD: return "HEAD";
        default: return "UNKNOWN";
    }
}

const char *umi_http_status_reason(int status)
{
    switch (status) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 409: return "Conflict";
        case 413: return "Payload Too Large";
        case 500: return "Internal Server Error";
        case 503: return "Service Unavailable";
        default: return "Unknown";
    }
}

UmiStatus umi_web_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
