/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_request_gate.c
 * PURPOSE: Exercise local browser admission and prove that service refusals cannot fall through to an application route.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/loopback_access.h"
#include "umicom/web/connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
typedef struct GateContext
{
    const char *mode;
    size_t calls, handled;
} GateContext;
static void Add(UmiWebRequest *request, const char *name, const char *value)
{
    CHECK(request->header_count < UMI_WEB_MAX_HEADERS);
    CHECK(umi_web_header_set(&request->headers[request->header_count], name, value) == UMI_STATUS_OK);
    ++request->header_count;
}
static UmiStatus Gate(const UmiWebRequest *request, UmiWebResponse *response, bool *accepted, void *data)
{
    GateContext *context = data;
    ++context->calls;
    CHECK(request != NULL);
    if (strcmp(context->mode, "gate-failure") == 0)
        return UMI_STATUS_IO_ERROR;
    if (strcmp(context->mode, "gate-unset") == 0)
        return umi_web_response_set_text(response, 403, "text/plain", "refused");
    *accepted = strcmp(context->mode, "gate-refuse") != 0;
    return *accepted ? UMI_STATUS_OK : umi_web_response_set_text(response, 403, "text/plain", "refused");
}
static UmiStatus Handle(const UmiWebRequest *request, UmiWebResponse *response, void *data)
{
    GateContext *context = data;
    ++context->handled;
    CHECK(request != NULL);
    return umi_web_response_set_text(response, 200, "text/plain", "accepted");
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *cases[] = {"numeric",
                                            "named",
                                            "mixed-case",
                                            "same-origin",
                                            "navigation",
                                            "without-metadata",
                                            "default-port",
                                            "default-port-explicit",
                                            "wrong-host",
                                            "wrong-port",
                                            "missing-host",
                                            "duplicate-host",
                                            "opaque-origin",
                                            "remote-origin",
                                            "other-local-origin",
                                            "secure-origin",
                                            "origin-path",
                                            "duplicate-origin",
                                            "cross-site",
                                            "same-site",
                                            "duplicate-site",
                                            "unknown-site",
                                            "header-count",
                                            "unterminated-name",
                                            "unterminated-value",
                                            "invalid-policy",
                                            "invalid-arguments",
                                            "gate-accept",
                                            "gate-refuse",
                                            "gate-unset",
                                            "gate-failure",
                                            "gate-clear",
                                            "gate-context",
                                            "closed-response"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    UmiWebRequest *request = calloc(1U, sizeof(*request));
    UmiWebResponse *response = calloc(1U, sizeof(*response));
    CHECK(request != NULL && response != NULL);
    request->method = UMI_HTTP_METHOD_GET;
    strcpy(request->path, "/");
    UmiWebLoopbackAccess access = {8080U};
    bool expected = true, accepted = true;
    const char *host = "127.0.0.1:8080";
    if (strcmp(mode, "named") == 0)
        host = "localhost:8080";
    if (strcmp(mode, "mixed-case") == 0)
        host = "LoCaLhOsT:8080";
    if (strcmp(mode, "wrong-host") == 0)
    {
        host = "example.invalid:8080";
        expected = false;
    }
    if (strcmp(mode, "wrong-port") == 0)
    {
        host = "127.0.0.1:8081";
        expected = false;
    }
    if (strcmp(mode, "default-port") == 0)
    {
        access.port = 80U;
        host = "localhost";
        Add(request, "Origin", "http://localhost:80");
    }
    if (strcmp(mode, "default-port-explicit") == 0)
    {
        access.port = 80U;
        host = "127.0.0.1:80";
        Add(request, "Origin", "http://127.0.0.1");
    }
    if (strcmp(mode, "missing-host") != 0)
        Add(request, strcmp(mode, "mixed-case") == 0 ? "hOsT" : "Host", host);
    else
        expected = false;
    if (strcmp(mode, "same-origin") == 0)
    {
        Add(request, "Origin", "http://127.0.0.1:8080");
        Add(request, "Sec-Fetch-Site", "same-origin");
    }
    if (strcmp(mode, "mixed-case") == 0)
        Add(request, "ORIGIN", "HTTP://LOCALHOST:8080");
    if (strcmp(mode, "navigation") == 0)
        Add(request, "Sec-Fetch-Site", "none");
    if (strcmp(mode, "duplicate-host") == 0)
    {
        Add(request, "HOST", host);
        expected = false;
    }
    if (strcmp(mode, "duplicate-origin") == 0)
    {
        Add(request, "Origin", "http://127.0.0.1:8080");
        Add(request, "origin", "http://127.0.0.1:8080");
        expected = false;
    }
    if (strcmp(mode, "duplicate-site") == 0)
    {
        Add(request, "Sec-Fetch-Site", "none");
        Add(request, "sec-fetch-site", "none");
        expected = false;
    }
    const struct
    {
        const char *mode, *origin;
    } origins[] = {{"opaque-origin", "null"},
                   {"remote-origin", "https://example.invalid"},
                   {"other-local-origin", "http://localhost:8080"},
                   {"secure-origin", "https://127.0.0.1:8080"},
                   {"origin-path", "http://127.0.0.1:8080/"}};
    for (size_t i = 0U; i < sizeof(origins) / sizeof(origins[0]); ++i)
        if (strcmp(mode, origins[i].mode) == 0)
        {
            Add(request, "Origin", origins[i].origin);
            expected = false;
        }
    if (strcmp(mode, "cross-site") == 0 || strcmp(mode, "same-site") == 0 ||
        strcmp(mode, "unknown-site") == 0)
    {
        Add(request, "Sec-Fetch-Site", mode);
        expected = false;
    }
    if (strcmp(mode, "header-count") == 0)
    {
        request->header_count = UMI_WEB_MAX_HEADERS + 1U;
        expected = false;
    }
    if (strcmp(mode, "unterminated-name") == 0)
    {
        memset(request->headers[0].name, 'x', sizeof(request->headers[0].name));
        expected = false;
    }
    if (strcmp(mode, "unterminated-value") == 0)
    {
        memset(request->headers[0].value, 'x', sizeof(request->headers[0].value));
        expected = false;
    }
    if (strcmp(mode, "invalid-policy") == 0)
    {
        access.port = 0U;
        CHECK(UmiWebLoopbackRequestGate(request, response, &accepted, &access) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              !accepted);
        access.port = 8080U;
    }
    if (strcmp(mode, "invalid-arguments") == 0)
    {
        CHECK(UmiWebLoopbackRequestGate(NULL, response, &accepted, &access) == UMI_STATUS_INVALID_ARGUMENT &&
              !accepted);
        CHECK(UmiWebLoopbackRequestGate(request, NULL, &accepted, &access) == UMI_STATUS_INVALID_ARGUMENT &&
              !accepted);
        CHECK(UmiWebLoopbackRequestGate(request, response, NULL, &access) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiWebLoopbackRequestGate(request, response, &accepted, NULL) == UMI_STATUS_INVALID_ARGUMENT &&
              !accepted);
    }
    CHECK(UmiWebLoopbackRequestGate(request, response, &accepted, &access) == UMI_STATUS_OK);
    CHECK(accepted == expected);
    if (!accepted)
        CHECK(response->status == 403 && response->body_length != 0U);
    if (strncmp(mode, "gate-", 5U) == 0 || strcmp(mode, "closed-response") == 0)
    {
        UmiWebService *service = NULL;
        CHECK(umi_web_service_create(&service) == UMI_STATUS_OK);
        GateContext context = {mode, 0U, 0U}, replacement = {mode, 0U, 0U};
        CHECK(UmiWebRouterSetFallback(umi_web_service_router(service), Handle, &context) == UMI_STATUS_OK);
        CHECK(UmiWebServiceSetRequestGate(service, Gate, &context) == UMI_STATUS_OK);
        if (strcmp(mode, "gate-context") == 0)
            CHECK(UmiWebServiceSetRequestGate(service, Gate, &replacement) == UMI_STATUS_OK);
        if (strcmp(mode, "gate-clear") == 0)
            CHECK(UmiWebServiceSetRequestGate(service, NULL, &context) == UMI_STATUS_OK);
        if (strcmp(mode, "closed-response") == 0)
        {
            CHECK(UmiWebServiceSetRequestGate(service, UmiWebLoopbackRequestGate, &access) == UMI_STATUS_OK);
            Add(request, "Origin", "https://example.invalid");
        }
        UmiStatus status = umi_web_service_handle(service, request, response);
        bool refusal = strcmp(mode, "gate-refuse") == 0 || strcmp(mode, "gate-unset") == 0 ||
                       strcmp(mode, "closed-response") == 0;
        bool failure = strcmp(mode, "gate-failure") == 0;
        CHECK(status == (failure ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK));
        CHECK(context.handled == (refusal || failure ? 0U : 1U));
        if (refusal)
            CHECK(response->status == 403);
        CHECK(umi_web_service_metrics(service)->requests == 1U);
        if (strcmp(mode, "gate-context") == 0)
            CHECK(context.calls == 0U && replacement.calls == 1U);
        else if (strcmp(mode, "gate-clear") == 0 || strcmp(mode, "closed-response") == 0)
            CHECK(context.calls == 0U);
        else
            CHECK(context.calls == 1U);
        if (strcmp(mode, "closed-response") == 0)
        {
            char wire[2048];
            size_t length = 0U;
            CHECK(UmiWebResponseFormatClosed(response, false, wire, sizeof(wire) - 1U, &length) ==
                  UMI_STATUS_OK);
            wire[length] = '\0';
            CHECK(strncmp(wire, "HTTP/1.1 403 ", 13U) == 0 &&
                  strstr(wire, "Access-Control-Allow-Origin") == NULL);
        }
        CHECK(UmiWebServiceSetRequestGate(NULL, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        umi_web_service_destroy(service);
    }
    free(request);
    free(response);
    return 0;
}
