/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_web_reactive_view.c
 *
 * PURPOSE:
 *   Exercise HTTP state projection, exact wire values and closed failure paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/reactive_view.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "Reactive HTTP, line %d: %s\n", __LINE__, #expression); \
    goto finish; } } while (0)

static bool HasHeader(const UmiWebResponse *response, const char *name, const char *value)
{
    for (size_t i = 0U; i < response->header_count; ++i)
        if (umi_web_header_name_equal(&response->headers[i], name) &&
            strcmp(response->headers[i].value, value) == 0) return true;
    return false;
}

static UmiStatus Gate(const UmiWebRequest *request, UmiWebResponse *response,
    bool *accepted, void *context)
{
    (void)request;
    *accepted = *(const bool *)context;
    return *accepted ? UMI_STATUS_OK : umi_web_response_set_text(response,
        UMI_HTTP_STATUS_FORBIDDEN, "text/plain", "Closed");
}

int main(void)
{
    UmiUiReactiveObservableStore *store = malloc(sizeof *store);
    UmiWebRequest *request = malloc(sizeof *request);
    UmiWebResponse *response = malloc(sizeof *response);
    UmiWebService *service = NULL;
    UmiWebResponseBody *initial = NULL, *delta = NULL;
    int result = EXIT_FAILURE;
    CHECK(store != NULL && request != NULL && response != NULL);
    umi_ui_reactive_observable_store_init(store);
    umi_web_request_init(request);
    request->method = UMI_HTTP_METHOD_GET;
    CHECK(umi_web_request_set_target(request, "/view") == UMI_STATUS_OK);
    UmiUiValue value = {.kind = UMI_UI_VALUE_NONE, .string_value = "inactive private bytes"};
    CHECK(umi_ui_reactive_observable_store_put(store, "empty", &value) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_integer(&value, INT64_MIN) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "amount", &value) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_real(&value, -0.0) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "rate", &value) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_boolean(&value, 1) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "ready", &value) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_string(&value, "<b>\r\n\"Text\"</b>") == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "message", &value) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_string(&value, "other private value") == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "private.key", &value) == UMI_STATUS_OK);

    const char *keys[] = {"message", "amount", "empty", "ready", "rate"};
    UmiWebReactiveView view = {"/view", store, keys, sizeof keys / sizeof keys[0]};
    bool allowed = false;
    CHECK(umi_web_service_create(&service) == UMI_STATUS_OK);
    CHECK(UmiWebServiceSetRequestGate(service, Gate, &allowed) == UMI_STATUS_OK);
    CHECK(UmiWebServiceSetBodyFallback(service, UmiWebReactiveViewHandle, &view) == UMI_STATUS_OK);
    CHECK(UmiWebServiceHandleBody(service, request, response, &initial) == UMI_STATUS_OK);
    CHECK(response->status == UMI_HTTP_STATUS_FORBIDDEN && initial == NULL);
    allowed = true;
    CHECK(UmiWebServiceHandleBody(service, request, response, &initial) == UMI_STATUS_OK);
    CHECK(response->status == UMI_HTTP_STATUS_OK && response->body_length == 0U && initial != NULL);
    CHECK(HasHeader(response, "Content-Type", "application/json"));
    CHECK(HasHeader(response, "Cache-Control", "no-store"));
    CHECK(HasHeader(response, "X-Content-Type-Options", "nosniff"));
    const char *first = UmiWebResponseBodyBytes(initial);
    CHECK(UmiWebResponseBodyLength(initial) == strlen(first));
    CHECK(strstr(first, "\"since\":\"0\",\"revision\":\"6\"") != NULL);
    CHECK(strstr(first, "\"key\":\"message\",\"revision\":\"5\",\"kind\":\"string\",\"value\":\"<b>\\r\\n\\\"Text\\\"</b>\"") != NULL);
    CHECK(strstr(first, "\"kind\":\"integer\",\"value\":\"-9223372036854775808\"") != NULL);
    CHECK(strstr(first, "\"kind\":\"none\",\"value\":null") != NULL);
    CHECK(strstr(first, "\"kind\":\"boolean\",\"value\":true") != NULL);
    CHECK(strstr(first, "\"kind\":\"real\",\"value\":-0") != NULL);
    CHECK(strstr(first, "private") == NULL);

    CHECK(umi_ui_value_set_integer(&value, INT64_MAX) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "amount", &value) == UMI_STATUS_OK);
    CHECK(umi_web_request_set_target(request, "/view?since=6") == UMI_STATUS_OK);
    CHECK(UmiWebServiceHandleBody(service, request, response, &delta) == UMI_STATUS_OK);
    CHECK(response->status == UMI_HTTP_STATUS_OK && delta != NULL);
    CHECK(strcmp(UmiWebResponseBodyBytes(delta),
        "{\"schema\":\"umicom.reactive.view/1\",\"since\":\"6\",\"revision\":\"7\","
        "\"rows\":[{\"key\":\"amount\",\"revision\":\"7\",\"kind\":\"integer\",\"value\":\"9223372036854775807\"}]}\n") == 0);
    CHECK(strstr(first, "-9223372036854775808") != NULL);
    UmiWebResponseBodyDestroy(delta);
    delta = NULL;

    CHECK(umi_ui_reactive_observable_store_put(store, "private.key", &value) == UMI_STATUS_OK);
    CHECK(umi_web_request_set_target(request, "/view?since=7") == UMI_STATUS_OK);
    CHECK(UmiWebServiceHandleBody(service, request, response, &delta) == UMI_STATUS_OK);
    CHECK(strcmp(UmiWebResponseBodyBytes(delta),
        "{\"schema\":\"umicom.reactive.view/1\",\"since\":\"7\",\"revision\":\"8\",\"rows\":[]}\n") == 0);
    UmiWebResponseBodyDestroy(delta);
    delta = NULL;
    CHECK(umi_web_request_set_target(request, "/view?since=9") == UMI_STATUS_OK);
    CHECK(UmiWebServiceHandleBody(service, request, response, &delta) == UMI_STATUS_OK);
    CHECK(response->status == UMI_HTTP_STATUS_CONFLICT && delta == NULL);

    const char *refusedQueries[] = {"/view?key=private.key", "/view?since=0&since=1",
        "/view?since=01", "/view?since=18446744073709551616", "/view?since=%30"};
    for (size_t i = 0U; i < sizeof refusedQueries / sizeof refusedQueries[0]; ++i) {
        CHECK(umi_web_request_set_target(request, refusedQueries[i]) == UMI_STATUS_OK);
        CHECK(UmiWebServiceHandleBody(service, request, response, &delta) == UMI_STATUS_OK);
        CHECK(response->status == UMI_HTTP_STATUS_BAD_REQUEST && delta == NULL);
    }
    CHECK(umi_web_request_set_target(request, "/elsewhere") == UMI_STATUS_OK);
    CHECK(UmiWebServiceHandleBody(service, request, response, &delta) == UMI_STATUS_OK);
    CHECK(response->status == UMI_HTTP_STATUS_NOT_FOUND && delta == NULL);
    CHECK(umi_web_request_set_target(request, "/view") == UMI_STATUS_OK);
    request->method = UMI_HTTP_METHOD_POST;
    CHECK(UmiWebServiceHandleBody(service, request, response, &delta) == UMI_STATUS_OK);
    CHECK(response->status == UMI_HTTP_STATUS_METHOD_NOT_ALLOWED && delta == NULL);
    CHECK(HasHeader(response, "Allow", "GET"));
    request->method = UMI_HTTP_METHOD_GET;
    request->body_length = 1U;
    CHECK(UmiWebServiceHandleBody(service, request, response, &delta) == UMI_STATUS_OK);
    CHECK(response->status == UMI_HTTP_STATUS_BAD_REQUEST && delta == NULL);
    request->body_length = 0U;

    /* Exercise both ends of a real JavaScript precision boundary. */
    store->revision = UINT64_MAX - 1U;
    CHECK(umi_ui_reactive_observable_store_put(store, "amount", &value) == UMI_STATUS_OK);
    CHECK(umi_web_request_set_target(request, "/view?since=18446744073709551614") == UMI_STATUS_OK);
    CHECK(UmiWebServiceHandleBody(service, request, response, &delta) == UMI_STATUS_OK);
    CHECK(strstr(UmiWebResponseBodyBytes(delta), "\"revision\":\"18446744073709551615\"") != NULL);
    UmiWebResponseBodyDestroy(delta);
    delta = NULL;

    /* The native value domain includes real values which JSON cannot encode.
     * A refused second row must not publish the first row or metadata. */
    umi_ui_reactive_observable_store_init(store);
    CHECK(umi_ui_reactive_observable_store_put(store, "message", &value) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_real(&value, (double)INFINITY) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "rate", &value) == UMI_STATUS_OK);
    const char *unportable[] = {"message", "rate"};
    view.keys = unportable;
    view.keyCount = sizeof unportable / sizeof unportable[0];
    CHECK(umi_web_request_set_target(request, "/view") == UMI_STATUS_OK);
    CHECK(umi_web_response_set_text(response, 404, "text/plain", "unchanged") == UMI_STATUS_OK);
    CHECK(UmiWebReactiveViewHandle(request, response, &delta, &view) != UMI_STATUS_OK);
    CHECK(response->status == 404 && strcmp(response->body, "unchanged") == 0 && delta == NULL);
    CHECK(umi_ui_value_set_string(&value, "\xC0\xAF") == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "rate", &value) == UMI_STATUS_OK);
    CHECK(UmiWebReactiveViewHandle(request, response, &delta, &view) != UMI_STATUS_OK);
    CHECK(response->status == 404 && strcmp(response->body, "unchanged") == 0 && delta == NULL);
    puts("Reactive HTTP initial, incremental, admission, precision and atomic refusal passed");
    result = EXIT_SUCCESS;
finish:
    UmiWebResponseBodyDestroy(delta);
    UmiWebResponseBodyDestroy(initial);
    umi_web_service_destroy(service);
    free(response);
    free(request);
    free(store);
    return result;
}
