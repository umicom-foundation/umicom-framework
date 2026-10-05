/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/loopback_access.c
 * PURPOSE: Keep local Host and Origin admission independent of application routes and native socket implementation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/loopback_access.h"
#include <stdio.h>
#include <string.h>

/* Header matching is ASCII-only; locale rules cannot change HTTP names or the
 * two literal local hosts. Full, terminated fixed fields are checked first. */
static int SameText(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0')
    {
        unsigned char a = (unsigned char)*left++, b = (unsigned char)*right++;
        if (a >= 'A' && a <= 'Z')
            a = (unsigned char)(a + ('a' - 'A'));
        if (b >= 'A' && b <= 'Z')
            b = (unsigned char)(b + ('a' - 'A'));
        if (a != b)
            return 0;
    }
    return *left == *right;
}
static UmiStatus Refuse(UmiWebResponse *response)
{
    UmiStatus status = umi_web_response_set_text(response, 403, "text/plain; charset=utf-8",
                                                 "Use this service's local browser address.\n");
    if (status == UMI_STATUS_OK)
        status = umi_web_response_set_header(response, "Cache-Control", "no-store");
    if (status == UMI_STATUS_OK)
        status = umi_web_response_set_header(response, "X-Content-Type-Options", "nosniff");
    return status;
}
UmiStatus UmiWebLoopbackRequestGate(const UmiWebRequest *request, UmiWebResponse *response, bool *accepted,
                                    void *context)
{
    if (accepted != NULL)
        *accepted = false;
    if (request == NULL || response == NULL || accepted == NULL || context == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiWebLoopbackAccess *access = context;
    if (access->port == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (request->header_count > UMI_WEB_MAX_HEADERS)
        return Refuse(response);
    const char *host = NULL, *origin = NULL, *site = NULL;
    for (size_t i = 0U; i < request->header_count; ++i)
    {
        const UmiWebHeader *field = &request->headers[i];
        if (memchr(field->name, '\0', sizeof(field->name)) == NULL ||
            memchr(field->value, '\0', sizeof(field->value)) == NULL)
            return Refuse(response);
        if (SameText(field->name, "Host"))
        {
            if (host != NULL)
                return Refuse(response);
            host = field->value;
        }
        else if (SameText(field->name, "Origin"))
        {
            if (origin != NULL)
                return Refuse(response);
            origin = field->value;
        }
        else if (SameText(field->name, "Sec-Fetch-Site"))
        {
            if (site != NULL)
                return Refuse(response);
            site = field->value;
        }
    }
    char numeric[32], named[32], expected_origin[48];
    int count = snprintf(numeric, sizeof(numeric), "127.0.0.1:%u", (unsigned)access->port);
    if (count < 0 || (size_t)count >= sizeof(numeric))
        return UMI_STATUS_INTERNAL_ERROR;
    count = snprintf(named, sizeof(named), "localhost:%u", (unsigned)access->port);
    if (count < 0 || (size_t)count >= sizeof(named))
        return UMI_STATUS_INTERNAL_ERROR;
    if (host == NULL)
        return Refuse(response);
    if (!SameText(host, numeric) && !SameText(host, named) &&
        !(access->port == 80U && (SameText(host, "127.0.0.1") || SameText(host, "localhost"))))
        return Refuse(response);
    if (origin != NULL)
    {
        count = snprintf(expected_origin, sizeof(expected_origin), "http://%s", host);
        if (count < 0 || (size_t)count >= sizeof(expected_origin))
            return UMI_STATUS_INTERNAL_ERROR;
        /* Default-port origins normally omit :80. Compare both canonical
         * spellings without admitting another host, scheme or port. */
        if (!SameText(origin, expected_origin))
        {
            if (access->port != 80U)
                return Refuse(response);
            const char *name =
                SameText(host, numeric) || SameText(host, "127.0.0.1") ? "127.0.0.1" : "localhost";
            count = snprintf(expected_origin, sizeof(expected_origin), "http://%s%s", name,
                             strchr(host, ':') != NULL ? "" : ":80");
            if (count < 0 || (size_t)count >= sizeof(expected_origin))
                return UMI_STATUS_INTERNAL_ERROR;
            if (!SameText(origin, expected_origin))
                return Refuse(response);
        }
    }
    if (site != NULL && strcmp(site, "none") != 0 && strcmp(site, "same-origin") != 0)
        return Refuse(response);
    *accepted = true;
    return UMI_STATUS_OK;
}
