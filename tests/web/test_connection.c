/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_connection.c
 * PURPOSE: Exercise service dispatch and partial byte transfers without borrowing a real network or starting a product.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

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
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct Fixture
{
    const char *mode;
    const unsigned char *input;
    size_t input_length, position, read_calls, write_calls, handler_calls, output_length;
    unsigned char output[4096];
    UmiCancellationToken *cancel;
} Fixture;
static UmiStatus Read(void *context, void *bytes, size_t capacity, size_t *out)
{
    Fixture *f = context;
    ++f->read_calls;
    *out = 0U;
    if (strcmp(f->mode, "read-failure") == 0)
        return UMI_STATUS_IO_ERROR;
    if (strcmp(f->mode, "disconnected") == 0)
        return UMI_STATUS_UNAVAILABLE;
    if (strcmp(f->mode, "read-zero") == 0)
        return UMI_STATUS_OK;
    if (strcmp(f->mode, "read-overcount") == 0)
    {
        *out = capacity + 1U;
        return UMI_STATUS_OK;
    }
    if (strcmp(f->mode, "cancel-read") == 0)
        (void)umi_cancellation_token_request(f->cancel);
    if (f->position == f->input_length)
        return UMI_STATUS_UNAVAILABLE;
    size_t count = f->input_length - f->position;
    if (count > capacity)
        count = capacity;
    if (strcmp(f->mode, "fragmented") == 0 && count > 1U)
        count = 1U;
    memcpy(bytes, f->input + f->position, count);
    f->position += count;
    *out = count;
    return UMI_STATUS_OK;
}
static UmiStatus Write(void *context, const void *bytes, size_t length, size_t *out)
{
    Fixture *f = context;
    ++f->write_calls;
    *out = 0U;
    if (strcmp(f->mode, "write-failure") == 0)
        return UMI_STATUS_IO_ERROR;
    if (strcmp(f->mode, "write-zero") == 0)
        return UMI_STATUS_OK;
    if (strcmp(f->mode, "write-overcount") == 0)
    {
        *out = length + 1U;
        return UMI_STATUS_OK;
    }
    if (strcmp(f->mode, "cancel-write") == 0)
        (void)umi_cancellation_token_request(f->cancel);
    size_t count = length > 3U ? 3U : length;
    if (count > sizeof(f->output) - f->output_length)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(f->output + f->output_length, bytes, count);
    f->output_length += count;
    *out = count;
    return UMI_STATUS_OK;
}
static UmiStatus Handler(const UmiWebRequest *request, UmiWebResponse *response, void *context)
{
    Fixture *f = context;
    ++f->handler_calls;
    if (strcmp(f->mode, "handler-failure") == 0)
        return UMI_STATUS_IO_ERROR;
    if (strcmp(f->mode, "cancelled-handler") == 0)
        (void)umi_cancellation_token_request(f->cancel);
    UmiStatus status = umi_web_response_set_text(response, 200, "text/plain", "hello");
    if (status != UMI_STATUS_OK)
        return status;
    if (request->method == UMI_HTTP_METHOD_POST)
    {
        memcpy(response->body, request->body, request->body_length);
        response->body_length = request->body_length;
    }
    if (strcmp(f->mode, "invalid-response") == 0)
        strcpy(response->headers[0].value, "bad\r\nInjected: true");
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"get",
                                            "fragmented",
                                            "head",
                                            "post",
                                            "binary-body",
                                            "missing-route",
                                            "malformed",
                                            "unsupported",
                                            "request-limit",
                                            "read-failure",
                                            "disconnected",
                                            "read-zero",
                                            "read-overcount",
                                            "write-failure",
                                            "write-zero",
                                            "write-overcount",
                                            "cancel-before",
                                            "cancel-read",
                                            "cancel-write",
                                            "handler-failure",
                                            "invalid-response",
                                            "cancelled-handler",
                                            "invalid-io",
                                            "invalid-bound"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    Fixture f = {0};
    f.mode = mode;
    const char *wire = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if (strcmp(mode, "head") == 0)
        wire = "HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if (strcmp(mode, "post") == 0 || strcmp(mode, "binary-body") == 0)
        wire = "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 3\r\n\r\nabc";
    if (strcmp(mode, "missing-route") == 0)
        wire = "GET /missing HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if (strcmp(mode, "malformed") == 0)
        wire = "GET / HTTP/1.1\r\n\r\n";
    if (strcmp(mode, "unsupported") == 0)
        wire = "GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\n\r\n";
    f.input = (const unsigned char *)wire;
    f.input_length = strlen(wire);
    unsigned char binary[256];
    if (strcmp(mode, "binary-body") == 0)
    {
        memcpy(binary, wire, f.input_length);
        binary[f.input_length - 2U] = 0U;
        f.input = binary;
    }
    CHECK(umi_cancellation_token_create(&f.cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel-before") == 0)
        (void)umi_cancellation_token_request(f.cancel);
    UmiWebService *service = NULL;
    CHECK(umi_web_service_create(&service) == UMI_STATUS_OK);
    const UmiHttpMethod methods[] = {UMI_HTTP_METHOD_GET, UMI_HTTP_METHOD_HEAD, UMI_HTTP_METHOD_POST};
    for (size_t i = 0U; i < 3U; ++i)
    {
        UmiWebRoute route;
        CHECK(umi_web_route_init(&route, methods[i], "/", Handler, &f) == UMI_STATUS_OK);
        CHECK(umi_web_router_add(umi_web_service_router(service), &route) == UMI_STATUS_OK);
    }
    UmiWebConnectionIo io = {&f, Read, Write};
    if (strcmp(mode, "invalid-io") == 0)
        io.write = NULL;
    size_t maximum = strcmp(mode, "request-limit") == 0   ? 8U
                     : strcmp(mode, "invalid-bound") == 0 ? 0U
                                                          : 1024U;
    UmiWebExchangeResult result;
    UmiStatus status = UmiWebConnectionProcess(service, &io, maximum, f.cancel, &result),
              expected = UMI_STATUS_OK;
    if (strcmp(mode, "malformed") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "unsupported") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    if (strcmp(mode, "request-limit") == 0)
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(mode, "read-failure") == 0 || strcmp(mode, "write-failure") == 0 ||
        strcmp(mode, "write-zero") == 0 || strcmp(mode, "handler-failure") == 0)
        expected = UMI_STATUS_IO_ERROR;
    if (strcmp(mode, "disconnected") == 0 || strcmp(mode, "read-zero") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "read-overcount") == 0 || strcmp(mode, "write-overcount") == 0)
        expected = UMI_STATUS_INVALID_STATE;
    if (strncmp(mode, "cancel-", 7U) == 0 || strcmp(mode, "cancelled-handler") == 0)
        expected = UMI_STATUS_CANCELLED;
    if (strcmp(mode, "invalid-response") == 0 || strcmp(mode, "invalid-io") == 0 ||
        strcmp(mode, "invalid-bound") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    CHECK(status == expected);
    int refused = strncmp(mode, "read-", 5U) == 0 || strcmp(mode, "disconnected") == 0 ||
                  strcmp(mode, "malformed") == 0 || strcmp(mode, "unsupported") == 0 ||
                  strcmp(mode, "request-limit") == 0 || strcmp(mode, "cancel-before") == 0 ||
                  strcmp(mode, "cancel-read") == 0 || strcmp(mode, "invalid-io") == 0 ||
                  strcmp(mode, "invalid-bound") == 0;
    CHECK(f.handler_calls == ((refused || strcmp(mode, "missing-route") == 0) ? 0U : 1U));
    CHECK(result.dispatched == !refused);
    CHECK(result.bytes_sent == f.output_length);
    if (result.response_complete)
    {
        CHECK(f.output_length < sizeof(f.output));
        f.output[f.output_length] = 0U;
        const char *body = strstr((const char *)f.output, "\r\n\r\n");
        CHECK(body != NULL);
        body += 4;
        size_t emitted = f.output_length - (size_t)(body - (const char *)f.output);
        CHECK(strstr((const char *)f.output, "Connection: close\r\n") != NULL);
        if (strcmp(mode, "head") == 0)
            CHECK(emitted == 0U && strstr((const char *)f.output, "Content-Length: 5\r\n") != NULL);
        if (strcmp(mode, "binary-body") == 0)
            CHECK(emitted == 3U && memcmp(body, "a\0c", 3U) == 0);
        if (strcmp(mode, "post") == 0)
            CHECK(emitted == 3U && memcmp(body, "abc", 3U) == 0);
        if (strcmp(mode, "missing-route") == 0)
            CHECK(result.response_status == 404);
        if (strcmp(mode, "handler-failure") == 0 || strcmp(mode, "invalid-response") == 0)
            CHECK(result.response_status == 500);
        if (strcmp(mode, "malformed") == 0)
            CHECK(result.response_status == 400);
        if (strcmp(mode, "request-limit") == 0)
            CHECK(result.response_status == 413);
        if (strcmp(mode, "unsupported") == 0)
            CHECK(result.response_status == 501);
    }
    else
        CHECK(expected != UMI_STATUS_OK);
    if (strcmp(mode, "fragmented") == 0)
        CHECK(f.read_calls == f.input_length && f.write_calls > 1U);
    umi_web_service_destroy(service);
    umi_cancellation_token_destroy(f.cancel);
    return 0;
}
