/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_web_reactive_boolean_edit.c
 *
 * PURPOSE:
 *   Verify Boolean browser edits, exact state revision and atomic refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/reactive_edit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%d: %s\n", __LINE__, #condition); failed = 1; goto cleanup; } \
} while (0)

typedef struct Authorization {
    unsigned calls;
    bool allowed;
} Authorization;

/* The fixture owner controls admission. The handler must query this gate
 * for an allowed origin and must leave it untouched for another origin. */
static UmiStatus Authorize(const UmiWebRequest *request, UmiWebResponse *response,
                           bool *accepted, void *context)
{
    (void)request;
    (void)response;
    Authorization *authorization = context;
    ++authorization->calls;
    *accepted = authorization->allowed;
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    int failed = 0;
    UmiUiReactiveObservableStore *store = malloc(sizeof *store);
    UmiUiReactiveObservableStore *before = malloc(sizeof *before);
    UmiWebOriginPolicy *origins = NULL;
    CHECK(store != NULL && before != NULL);
    CHECK(umi_web_origin_policy_create(&origins) == UMI_STATUS_OK);
    CHECK(umi_web_origin_allow(origins, "http://127.0.0.1:4111") == UMI_STATUS_OK);
    umi_ui_reactive_observable_store_init(store);
    UmiUiValue initial;
    UmiUiValue sibling;
    CHECK(umi_ui_value_set_boolean(&initial, strcmp(test, "clear") == 0) == UMI_STATUS_OK);
    if (strcmp(test, "wrong-kind") == 0) CHECK(umi_ui_value_set_integer(&initial, 7) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_integer(&sibling, INT64_MAX) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "practice.enabled", &initial) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "practice.quantity", &sibling) == UMI_STATUS_OK);

    Authorization authorization = {.allowed = true};
    UmiWebReactiveBooleanEdit edit = {"/practice-enabled", "practice.enabled", store, origins, Authorize, &authorization};
    UmiWebRequest request;
    umi_web_request_init(&request);
    request.method = UMI_HTTP_METHOD_POST;
    CHECK(umi_web_request_set_target(&request, edit.path) == UMI_STATUS_OK);
    CHECK(umi_web_header_set(request.headers, "Origin", "http://127.0.0.1:4111") == UMI_STATUS_OK);
    CHECK(umi_web_header_set(request.headers + 1, "Content-Type", UMI_WEB_REACTIVE_EDIT_CONTENT_TYPE) == UMI_STATUS_OK);
    request.header_count = 2U;

    /* Every case uses the same exact endpoint and two-state owner. Separate
     * malformed literals from stale revisions and authorized store errors. */
    const char *wire = "revision=2&value=true";
    const char *expected_body = "{\"schema\":\"umicom.reactive.edit/1\",\"revision\":\"3\"}\n";
    int expected_http = 200, expected_value = 1;
    unsigned expected_calls = 1U;
    UmiStatus expected_status = UMI_STATUS_OK;
    if (strcmp(test, "clear") == 0) {
        wire = "revision=2&value=false";
        expected_value = 0;
    } else if (strcmp(test, "numeric") == 0 || strcmp(test, "quoted") == 0 || strcmp(test, "case") == 0 ||
               strcmp(test, "extra-value") == 0 || strcmp(test, "leading-zero") == 0) {
        if (strcmp(test, "numeric") == 0) wire = "revision=2&value=1";
        else if (strcmp(test, "quoted") == 0) wire = "revision=2&value=\"true\"";
        else if (strcmp(test, "case") == 0) wire = "revision=2&value=True";
        else if (strcmp(test, "extra-value") == 0) wire = "revision=2&value=true&value=false";
        else wire = "revision=02&value=true";
        expected_http = 400;
        expected_body = "{\"error\":\"body\"}\n";
    } else if (strcmp(test, "stale") == 0) {
        wire = "revision=1&value=true";
        expected_http = 409;
        expected_body = "{\"error\":\"revision\"}\n";
    } else if (strcmp(test, "origin") == 0) {
        CHECK(umi_web_header_set(request.headers, "Origin", "http://127.0.0.1:4112") == UMI_STATUS_OK);
        expected_calls = 0U;
        expected_http = 403;
        expected_body = "{\"error\":\"origin\"}\n";
    } else if (strcmp(test, "permission") == 0) {
        authorization.allowed = false;
        expected_http = 403;
        expected_body = "{\"error\":\"permission\"}\n";
    } else if (strcmp(test, "wrong-kind") == 0) {
        expected_status = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(test, "missing-key") == 0) {
        edit.key = "practice.missing";
        expected_status = UMI_STATUS_NOT_FOUND;
    } else {
        CHECK(strcmp(test, "set") == 0);
    }

    memcpy(request.body, wire, strlen(wire));
    request.body_length = strlen(wire);
    memcpy(before, store, sizeof *before);
    UmiWebResponse response;
    UmiWebResponse previous;
    memset(&response, 0x4a, sizeof response);
    memcpy(&previous, &response, sizeof previous);
    CHECK(UmiWebReactiveBooleanEditHandle(&request, &response, &edit) == expected_status);
    CHECK(authorization.calls == expected_calls);
    if (expected_status == UMI_STATUS_OK) {
        CHECK(response.status == expected_http && response.body_length == strlen(expected_body) &&
              strcmp(response.body, expected_body) == 0);
    } else CHECK(memcmp(&previous, &response, sizeof response) == 0);

    /* Success advances only the requested value and preserves an integer at
     * its full range. Every refusal must preserve the entire store. */
    if (expected_status == UMI_STATUS_OK && expected_http == 200) {
        UmiUiValue current;
        CHECK(store->revision == 3U);
        CHECK(umi_ui_reactive_observable_store_get(store, edit.key, &current) == UMI_STATUS_OK);
        CHECK(current.kind == UMI_UI_VALUE_BOOLEAN && current.boolean_value == expected_value);
        CHECK(umi_ui_reactive_observable_store_get(store, "practice.quantity", &current) == UMI_STATUS_OK);
        CHECK(current.kind == UMI_UI_VALUE_INTEGER && current.integer_value == INT64_MAX);
    } else CHECK(memcmp(before, store, sizeof *before) == 0);

cleanup:
    umi_web_origin_policy_destroy(origins);
    free(before);
    free(store);
    return failed;
}
