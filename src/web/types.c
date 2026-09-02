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

/*
 * Provide the http method from text operation used by this module and its client
 * applications.
 */
UmiHttpMethod umi_http_method_from_text(const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return UMI_HTTP_METHOD_UNKNOWN;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "GET") == 0) return UMI_HTTP_METHOD_GET;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "POST") == 0) return UMI_HTTP_METHOD_POST;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "PUT") == 0) return UMI_HTTP_METHOD_PUT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "PATCH") == 0) return UMI_HTTP_METHOD_PATCH;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "DELETE") == 0) return UMI_HTTP_METHOD_DELETE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "OPTIONS") == 0) return UMI_HTTP_METHOD_OPTIONS;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "HEAD") == 0) return UMI_HTTP_METHOD_HEAD;
    return UMI_HTTP_METHOD_UNKNOWN;
}

/* Provide the http method text operation used by this module and its client applications. */
const char *umi_http_method_text(UmiHttpMethod method)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the http status reason operation used by this module and its client
 * applications.
 */
const char *umi_http_status_reason(int status)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/* Provide the web copy text operation used by this module and its client applications. */
UmiStatus umi_web_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
