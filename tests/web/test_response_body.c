/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_response_body.c
 *
 * PURPOSE:
 *   Verify bounded body ownership, canonical asset bytes and closed-connection delivery without a network.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/web/connection.h"
#include "umicom/web/static_files.h"
#include "umicom/web/preview.h"

typedef struct BodyFixture {
    UmiWebStaticFiles files;
    const char *mode;
    const char *request;
    size_t input_position, output_length, fallback_calls;
    unsigned char *output;
    size_t output_capacity;
    int headers_sent;
    UmiCancellationToken *cancel;
} BodyFixture;

static UmiStatus Read(void *context, void *bytes, size_t capacity, size_t *count)
{
    BodyFixture *f = context;
    *count = 0U;
    size_t remaining = strlen(f->request) - f->input_position;
    if (remaining == 0U) return UMI_STATUS_UNAVAILABLE;
    if (remaining > capacity) remaining = capacity;
    if (remaining > 13U) remaining = 13U;
    memcpy(bytes, f->request + f->input_position, remaining);
    f->input_position += remaining;
    *count = remaining;
    return UMI_STATUS_OK;
}
static UmiStatus Write(void *context, const void *bytes, size_t length, size_t *count)
{
    BodyFixture *f = context;
    *count = 0U;
    if (f->headers_sent && strcmp(f->mode, "write-failure") == 0) return UMI_STATUS_IO_ERROR;
    size_t chunk = length > 997U ? 997U : length;
    if (f->headers_sent && strcmp(f->mode, "partial-failure") == 0 && chunk > 11U) chunk = 11U;
    if (chunk >= f->output_capacity - f->output_length) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(f->output + f->output_length, bytes, chunk);
    f->output_length += chunk;
    f->output[f->output_length] = 0U;
    *count = chunk;
    if (f->headers_sent && strcmp(f->mode, "cancel-body") == 0)
        umi_cancellation_token_request(f->cancel);
    if (f->headers_sent && strcmp(f->mode, "partial-failure") == 0) return UMI_STATUS_IO_ERROR;
    if (!f->headers_sent && strstr((const char *)f->output, "\r\n\r\n") != NULL)
        f->headers_sent = 1;
    return UMI_STATUS_OK;
}
static UmiStatus StaticBody(const UmiWebRequest *request, UmiWebResponse *response,
    UmiWebResponseBody **body, void *context)
{
    BodyFixture *f = context;
    ++f->fallback_calls;
    UmiStatus status = UmiWebStaticFilesHandleBody(request, response, body, &f->files);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(f->mode, "body-failure") == 0) return UMI_STATUS_IO_ERROR;
    if (strcmp(f->mode, "body-conflict") == 0) response->body_length = 1U;
    if (strcmp(f->mode, "invalid-header") == 0)
        return umi_web_response_set_header(response, "Content-Length", "7");
    if (strcmp(f->mode, "no-content") == 0) response->status = 204;
    if (strcmp(f->mode, "not-modified") == 0) response->status = 304;
    return UMI_STATUS_OK;
}
static UmiStatus Refuse(const UmiWebRequest *request, UmiWebResponse *response,
    bool *accepted, void *context)
{
    (void)request; (void)context;
    *accepted = false;
    return umi_web_response_set_text(response, 403, "text/plain", "gate refusal");
}
static UmiStatus Exact(const UmiWebRequest *request, UmiWebResponse *response, void *context)
{
    (void)request; (void)context;
    return umi_web_response_set_text(response, 404, "text/plain", "intentional route result");
}
static const char *Header(const UmiWebResponse *response, const char *name)
{
    for (size_t i = 0U; i < response->header_count; ++i)
        if (umi_web_header_name_equal(&response->headers[i], name)) return response->headers[i].value;
    return "";
}
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *cases[] = {"ownership", "headers", "svg", "ico", "head",
        "empty", "exact-limit", "oversized", "traversal", "missing", "method", "legacy",
        "fallback-clear", "gate", "exact-route", "write-failure", "partial-failure", "cancel-body",
        "body-failure", "body-conflict", "invalid-header", "no-content", "not-modified", "preview", "preview-gate"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0) known = 1;
    CHECK(known);
    if (strcmp(mode, "ownership") == 0) {
        unsigned char bytes[] = {0U, 255U, 7U, 0U};
        UmiWebResponseBody *body = NULL;
        CHECK(UmiWebResponseBodyCreate(bytes, sizeof(bytes), &body) == UMI_STATUS_OK);
        bytes[2] = 9U;
        CHECK(UmiWebResponseBodyLength(body) == 4U &&
            ((const unsigned char *)UmiWebResponseBodyBytes(body))[2] == 7U);
        UmiWebResponseBodyDestroy(body);
        body = NULL;
        CHECK(UmiWebResponseBodyCreate(bytes, UMI_WEB_RESPONSE_BODY_LIMIT + 1U, &body) == UMI_STATUS_CAPACITY_EXCEEDED && body == NULL);
        CHECK(UmiWebResponseBodyCreate(NULL, 1U, &body) == UMI_STATUS_INVALID_ARGUMENT && body == NULL);
        CHECK(UmiWebResponseBodyCreate(bytes, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiWebResponseBodyCreate(NULL, 0U, &body) == UMI_STATUS_OK && UmiWebResponseBodyLength(body) == 0U);
        UmiWebResponseBodyDestroy(body);
        UmiWebResponseBodyDestroy(NULL);
        CHECK(UmiWebResponseBodyBytes(NULL) == NULL && UmiWebResponseBodyLength(NULL) == 0U);
        return 0;
    }
    UmiWebResponse *response = calloc(1U, sizeof(*response));
    CHECK(response != NULL);
    if (strcmp(mode, "headers") == 0) {
        char wire[256]; size_t used = 0U;
        umi_web_response_init(response);
        CHECK(UmiWebResponseFormatClosedHeaders(response, UMI_WEB_RESPONSE_BODY_LIMIT, wire, sizeof(wire)-1U, &used) == UMI_STATUS_OK);
        wire[used] = '\0';
        CHECK(strstr(wire, "Content-Length: 1048576\r\n") != NULL);
        memset(wire, 'Q', sizeof(wire));
        CHECK(UmiWebResponseFormatClosedHeaders(response, 12U, wire, 1U, &used) == UMI_STATUS_CAPACITY_EXCEEDED && used == 0U);
        for (size_t i = 0U; i < sizeof(wire); ++i) CHECK(wire[i] == 'Q');
        CHECK(umi_web_response_set_header(response, "Transfer-Encoding", "chunked") == UMI_STATUS_OK);
        CHECK(UmiWebResponseFormatClosedHeaders(response, 12U, wire, sizeof(wire), &used) == UMI_STATUS_INVALID_ARGUMENT && used == 0U);
        for (size_t i = 0U; i < sizeof(wire); ++i) CHECK(wire[i] == 'Q');
        free(response);
        return 0;
    }
    char root[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    BodyFixture f = {0}; f.mode = mode;
    CHECK(umi_web_static_files_init(&f.files, root) == UMI_STATUS_OK);
    unsigned char *expected = NULL;
    size_t length = 180000U;
    const char *leaf = "asset.bin", *mime = "application/octet-stream";
    if (strcmp(mode, "svg") == 0 || strcmp(mode, "ico") == 0) {
        leaf = strcmp(mode, "svg") == 0 ? "umicom-logo.svg" : "umicom.ico";
        mime = strcmp(mode, "svg") == 0 ? "image/svg+xml" : "image/vnd.microsoft.icon";
        CHECK(UmiRootedFileRead(argv[2], leaf, UMI_WEB_RESPONSE_BODY_LIMIT, &expected, &length) == UMI_STATUS_OK);
        CHECK(length > UMI_WEB_BODY_CAPACITY);
    } else {
        if (strcmp(mode, "empty") == 0) length = 0U;
        if (strcmp(mode, "exact-limit") == 0) length = UMI_WEB_RESPONSE_BODY_LIMIT;
        if (strcmp(mode, "oversized") == 0) length = UMI_WEB_RESPONSE_BODY_LIMIT + 1U;
        expected = malloc(length == 0U ? 1U : length);
        CHECK(expected != NULL);
        for (size_t i = 0U; i < length; ++i) expected[i] = (unsigned char)(i % 251U);
    }
    CHECK(UmiRootedFileWrite(root, leaf, expected, length) == UMI_STATUS_OK);
    UmiWebService *service = NULL;
    CHECK(umi_web_service_create(&service) == UMI_STATUS_OK);
    CHECK(UmiWebRouterSetFallback(umi_web_service_router(service), UmiWebStaticFilesHandle, &f.files) == UMI_STATUS_OK);
    CHECK(UmiWebServiceSetBodyFallback(service, StaticBody, &f) == UMI_STATUS_OK);
    char url[128]; CHECK(snprintf(url, sizeof(url), "/%s", leaf) > 0);
    if (strcmp(mode, "traversal") == 0) strcpy(url, "/%2e%2e/asset.bin");
    if (strcmp(mode, "missing") == 0) strcpy(url, "/absent.bin");
    const char *method = strcmp(mode, "head") == 0 ? "HEAD" : strcmp(mode, "method") == 0 ? "POST" : "GET";
    if (strcmp(mode, "gate") == 0) CHECK(UmiWebServiceSetRequestGate(service, Refuse, &f) == UMI_STATUS_OK);
    if (strcmp(mode, "exact-route") == 0) {
        UmiWebRoute route;
        CHECK(umi_web_route_init(&route, UMI_HTTP_METHOD_GET, url, Exact, NULL) == UMI_STATUS_OK);
        CHECK(umi_web_router_add(umi_web_service_router(service), &route) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "fallback-clear") == 0) CHECK(UmiWebServiceSetBodyFallback(service, NULL, &f) == UMI_STATUS_OK);
    UmiWebRequest *request = calloc(1U, sizeof(*request)); CHECK(request != NULL);
    request->method = strcmp(method, "HEAD") == 0 ? UMI_HTTP_METHOD_HEAD : strcmp(method, "POST") == 0 ? UMI_HTTP_METHOD_POST : UMI_HTTP_METHOD_GET;
    strcpy(request->path, url);
    if (strcmp(mode, "preview") == 0 || strcmp(mode, "preview-gate") == 0) {
        UmiWebPreview *preview = NULL;
        CHECK(UmiWebPreviewCreate(root, 8097U, &preview) == UMI_STATUS_OK);
        const char *host = strcmp(mode, "preview-gate") == 0 ? "example.invalid:8097" : "127.0.0.1:8097";
        char message[512];
        int count = snprintf(message, sizeof(message), "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", url, host);
        CHECK(count > 0 && (size_t)count < sizeof(message));
        CHECK(UmiWebRequestParseStrict(message, (size_t)count, sizeof(message), request) == UMI_STATUS_OK);
        UmiWebResponseBody *body = NULL;
        CHECK(UmiWebPreviewHandleBody(preview, request, response, &body) == UMI_STATUS_OK);
        if (strcmp(mode, "preview-gate") == 0) CHECK(response->status == 403 && body == NULL);
        else CHECK(response->status == 200 && UmiWebResponseBodyLength(body) == length &&
            memcmp(UmiWebResponseBodyBytes(body), expected, length) == 0);
        UmiWebResponseBodyDestroy(body);
        UmiWebPreviewDestroy(preview);
    } else if (strcmp(mode, "legacy") == 0) {
        CHECK(umi_web_service_handle(service, request, response) == UMI_STATUS_OK && response->status == 413);
        CHECK(f.fallback_calls == 0U);
    } else {
        if (strcmp(mode, "svg") == 0 || strcmp(mode, "ico") == 0) {
            UmiWebResponseBody *body = NULL;
            CHECK(UmiWebServiceHandleBody(service, request, response, &body) == UMI_STATUS_OK);
            CHECK(response->status == 200 && response->body_length == 0U);
            CHECK(UmiWebResponseBodyLength(body) == length && memcmp(UmiWebResponseBodyBytes(body), expected, length) == 0);
            CHECK(strcmp(Header(response, "Content-Type"), mime) == 0);
            CHECK(strcmp(Header(response, "X-Content-Type-Options"), "nosniff") == 0);
            UmiWebResponseBodyDestroy(body); f.fallback_calls = 0U;
        }
        char input[512];
        int n = snprintf(input, sizeof(input), "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n", method, url);
        CHECK(n > 0 && (size_t)n < sizeof(input)); f.request = input;
        f.output_capacity = UMI_WEB_RESPONSE_BODY_LIMIT + UMI_WEB_CONNECTION_RESPONSE_LIMIT + 1U;
        f.output = calloc(f.output_capacity, 1U); CHECK(f.output != NULL);
        CHECK(umi_cancellation_token_create(&f.cancel) == UMI_STATUS_OK);
        UmiWebConnectionIo io = {&f, Read, Write}; UmiWebExchangeResult result;
        UmiStatus status = UmiWebConnectionProcess(service, &io, UMI_WEB_CONNECTION_REQUEST_LIMIT, f.cancel, &result);
        CHECK(result.dispatched && result.bytes_sent == f.output_length);
        char *separator = strstr((const char *)f.output, "\r\n\r\n"); CHECK(separator != NULL);
        size_t header_bytes = (size_t)(separator - (char *)f.output) + 4U;
        if (strcmp(mode, "write-failure") == 0 || strcmp(mode, "partial-failure") == 0 || strcmp(mode, "cancel-body") == 0) {
            CHECK(status == (strcmp(mode, "cancel-body") == 0 ? UMI_STATUS_CANCELLED : UMI_STATUS_IO_ERROR));
            CHECK(!result.response_complete && result.response_status == 200 && f.output_length < header_bytes + length);
            if (strcmp(mode, "partial-failure") == 0) CHECK(f.output_length == header_bytes + 11U);
        } else if (strcmp(mode, "body-failure") == 0 || strcmp(mode, "body-conflict") == 0 || strcmp(mode, "invalid-header") == 0) {
            CHECK(status != UMI_STATUS_OK && result.response_complete && result.response_status == 500);
            CHECK(f.output_length < 1024U);
        } else {
            CHECK(status == UMI_STATUS_OK && result.response_complete);
            int code = strcmp(mode, "oversized") == 0 || strcmp(mode, "fallback-clear") == 0 ? 413 :
                strcmp(mode, "traversal") == 0 || strcmp(mode, "gate") == 0 ? 403 :
                strcmp(mode, "missing") == 0 || strcmp(mode, "exact-route") == 0 ? 404 :
                strcmp(mode, "method") == 0 ? 405 : strcmp(mode, "no-content") == 0 ? 204 :
                strcmp(mode, "not-modified") == 0 ? 304 : 200;
            CHECK(result.response_status == code);
            if (code == 200) {
                char length_header[80]; snprintf(length_header, sizeof(length_header), "Content-Length: %zu\r\n", length);
                CHECK(strstr((const char *)f.output, length_header) != NULL);
                if (strcmp(mode, "head") == 0) CHECK(f.output_length == header_bytes);
                else CHECK(f.output_length == header_bytes + length && memcmp(f.output + header_bytes, expected, length) == 0);
            }
            if (code == 204 || code == 304) CHECK(f.output_length == header_bytes && strstr((const char *)f.output, "Content-Length:") == NULL);
            if (strcmp(mode, "gate") == 0 || strcmp(mode, "exact-route") == 0 || strcmp(mode, "fallback-clear") == 0) CHECK(f.fallback_calls == 0U);
            else CHECK(f.fallback_calls == 1U);
        }
        umi_cancellation_token_destroy(f.cancel); free(f.output);
    }
    UmiRootedFileFree(expected);
    umi_web_service_destroy(service); free(response); free(request);
    return 0;
}
