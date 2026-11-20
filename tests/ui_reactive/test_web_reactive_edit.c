/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_web_reactive_edit.c
 *
 * PURPOSE:
 *   Verify admitted browser integer edits and unchanged native state on refusal.
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

typedef struct EditAdmission {
    unsigned calls;
    bool allowed;
    bool reauthenticate;
    UmiStatus status;
} EditAdmission;

static UmiStatus Authorize(const UmiWebRequest *request, UmiWebResponse *response,
                           bool *accepted, void *context)
{
    EditAdmission *gate = context;
    (void)request;
    gate->calls += 1U;
    *accepted = gate->allowed;
    if (gate->status != UMI_STATUS_OK) return gate->status;
    if (gate->reauthenticate)
        return umi_web_response_set_text(response, 401, "application/json", "{\"error\":\"sign-in\"}\n");
    return UMI_STATUS_OK;
}

static unsigned CountHeader(const UmiWebResponse *response, const char *name, const char *value)
{
    unsigned found = 0U;
    for (size_t index = 0U; index < response->header_count; ++index) {
        const UmiWebHeader *header = &response->headers[index];
        if (umi_web_header_name_equal(header, name) && strcmp(header->value, value) == 0) ++found;
    }
    return found;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    UmiUiReactiveObservableStore *store = malloc(sizeof *store);
    UmiUiReactiveObservableStore *before = malloc(sizeof *before);
    UmiWebOriginPolicy *origins = NULL;
    int failed = 0;
    CHECK(store != NULL && before != NULL);
    CHECK(umi_web_origin_policy_create(&origins) == UMI_STATUS_OK);
    CHECK(umi_web_origin_allow(origins, "http://127.0.0.1:4111") == UMI_STATUS_OK);
    umi_ui_reactive_observable_store_init(store);
    UmiUiValue initial = {.kind = UMI_UI_VALUE_INTEGER, .integer_value = 4};
    if (!strcmp(test, "wrong-kind")) initial.kind = UMI_UI_VALUE_BOOLEAN;
    CHECK(umi_ui_reactive_observable_store_put(store, "practice.input", &initial) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "practice.other", &initial) == UMI_STATUS_OK);

    EditAdmission gate = {.allowed = true, .status = UMI_STATUS_OK};
    UmiWebReactiveIntegerEdit edit = {"/practice-edit", "practice.input", store, origins, Authorize,
                                     &gate, INT64_MIN, INT64_MAX};
    UmiWebRequest request;
    umi_web_request_init(&request);
    request.method = UMI_HTTP_METHOD_POST;
    CHECK(umi_web_request_set_target(&request, edit.path) == UMI_STATUS_OK);
    CHECK(umi_web_header_set(&request.headers[0], "Origin", "http://127.0.0.1:4111") == UMI_STATUS_OK);
    CHECK(umi_web_header_set(&request.headers[1], "Content-Type", UMI_WEB_REACTIVE_EDIT_CONTENT_TYPE) == UMI_STATUS_OK);
    request.header_count = 2U;

    const char *wire = "revision=2&value=42";
    const char *body = "{\"schema\":\"umicom.reactive.edit/1\",\"revision\":\"3\"}\n";
    int expectedHttp = 200;
    int64_t expectedValue = 42;
    unsigned expectedGateCalls = 1U;
    UmiStatus expectedStatus = UMI_STATUS_OK;
    if (!strcmp(test, "minimum")) {
        wire = "revision=2&value=-9223372036854775808";
        expectedValue = INT64_MIN;
    } else if (!strcmp(test, "maximum")) {
        wire = "revision=2&value=9223372036854775807";
        expectedValue = INT64_MAX;
    } else if (!strcmp(test, "last-revision")) {
        store->revision = UINT64_MAX - 1U;
        wire = "revision=18446744073709551614&value=42";
        body = "{\"schema\":\"umicom.reactive.edit/1\",\"revision\":\"18446744073709551615\"}\n";
    } else if (!strcmp(test, "stale") || !strcmp(test, "future")) {
        wire = !strcmp(test, "stale") ? "revision=1&value=42" : "revision=3&value=42";
        expectedHttp = 409;
        body = "{\"error\":\"revision\"}\n";
    } else if (!strcmp(test, "exhausted")) {
        store->revision = UINT64_MAX;
        wire = "revision=18446744073709551615&value=42";
        expectedHttp = 409;
        body = "{\"error\":\"revision-exhausted\"}\n";
    } else if (!strcmp(test, "range")) {
        edit.maximum = 41;
        expectedHttp = 400;
        body = "{\"error\":\"range\"}\n";
    } else if (!strcmp(test, "origin") || !strcmp(test, "duplicate-origin") || !strcmp(test, "required-origin")) {
        if (!strcmp(test, "origin")) {
            CHECK(umi_web_header_set(&request.headers[0], "Origin", "http://127.0.0.1:4112") == UMI_STATUS_OK);
        } else if (!strcmp(test, "duplicate-origin")) {
            request.headers[request.header_count++] = request.headers[0];
        } else {
            request.headers[0] = request.headers[1];
            request.header_count = 1U;
        }
        expectedGateCalls = 0U;
        expectedHttp = 403;
        body = "{\"error\":\"origin\"}\n";
    } else if (!strcmp(test, "method")) {
        request.method = UMI_HTTP_METHOD_GET;
        expectedGateCalls = 0U;
        expectedHttp = 405;
        body = "{\"error\":\"method\"}\n";
    } else if (!strcmp(test, "path")) {
        CHECK(umi_web_request_set_target(&request, "/different-edit") == UMI_STATUS_OK);
        expectedGateCalls = 0U;
        expectedHttp = 404;
        body = "{\"error\":\"edit\"}\n";
    } else if (!strcmp(test, "permission") || !strcmp(test, "reauthenticate")) {
        gate.allowed = false;
        gate.reauthenticate = !strcmp(test, "reauthenticate");
        expectedHttp = gate.reauthenticate ? 401 : 403;
        body = gate.reauthenticate ? "{\"error\":\"sign-in\"}\n" : "{\"error\":\"permission\"}\n";
    } else if (!strcmp(test, "gate-failure")) {
        gate.status = expectedStatus = UMI_STATUS_IO_ERROR;
    } else if (!strcmp(test, "content-type") || !strcmp(test, "duplicate-type")) {
        if (!strcmp(test, "duplicate-type")) request.headers[request.header_count++] = request.headers[1];
        else CHECK(umi_web_header_set(&request.headers[1], "Content-Type", "text/plain") == UMI_STATUS_OK);
        expectedHttp = 415;
        body = "{\"error\":\"type\"}\n";
    } else if (!strcmp(test, "query") || !strcmp(test, "large-body") || !strcmp(test, "embedded-zero")) {
        if (!strcmp(test, "query")) CHECK(umi_web_request_set_target(&request, "/practice-edit?value=42") == UMI_STATUS_OK);
        expectedHttp = 400;
        body = "{\"error\":\"body\"}\n";
    } else if (!strcmp(test, "unconfigured") || !strcmp(test, "excess-header-count")) {
        if (!strcmp(test, "unconfigured")) edit.authorize = NULL;
        else request.header_count = UMI_WEB_MAX_HEADERS + 1U;
        expectedStatus = UMI_STATUS_INVALID_ARGUMENT;
        expectedGateCalls = 0U;
    } else if (!strcmp(test, "unknown-key")) {
        edit.key = "practice.undeclared";
        expectedStatus = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(test, "wrong-kind")) {
        expectedStatus = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(test, "success") != 0 && strcmp(test, "malformed") != 0) {
        failed = 2;
        goto cleanup;
    }

    memcpy(before, store, sizeof *before);
    UmiWebResponse output;
    UmiWebResponse prior;
    memset(&output, 0x4a, sizeof output);
    memcpy(&prior, &output, sizeof prior);

    if (!strcmp(test, "malformed")) {
        static const char *const bad[] = {
            "", "revision=02&value=4", "revision=+2&value=4", "revision=-2&value=4",
            "revision=2&value=04", "revision=2&value=-0", "revision=2&value=+4",
            "revision=2&value=", "revision=2&value=42&key=practice.other", "value=4&revision=2",
            "revision=2&value=%34", "revision=2&value=9223372036854775808",
            "revision=2&value=-9223372036854775809", "revision=18446744073709551616&value=4",
        };
        for (size_t index = 0U; index < sizeof bad / sizeof bad[0]; ++index) {
            request.body_length = strlen(bad[index]);
            memcpy(request.body, bad[index], request.body_length);
            CHECK(UmiWebReactiveIntegerEditHandle(&request, &output, &edit) == UMI_STATUS_OK);
            CHECK(output.status == 400 && memcmp(before, store, sizeof *store) == 0);
        }
        CHECK(gate.calls == sizeof bad / sizeof bad[0]);
        goto cleanup;
    }
    request.body_length = strlen(wire);
    memcpy(request.body, wire, request.body_length);
    if (!strcmp(test, "large-body")) request.body_length = sizeof request.body + 1U;
    if (!strcmp(test, "embedded-zero")) request.body[request.body_length - 1U] = '\0';
    CHECK(UmiWebReactiveIntegerEditHandle(&request, &output, &edit) == expectedStatus);
    CHECK(gate.calls == expectedGateCalls);
    if (expectedStatus != UMI_STATUS_OK) {
        CHECK(memcmp(before, store, sizeof *store) == 0 && memcmp(&prior, &output, sizeof output) == 0);
        goto cleanup;
    }
    CHECK(output.status == expectedHttp && output.body_length == strlen(body) && strcmp(output.body, body) == 0);
    CHECK(CountHeader(&output, "Cache-Control", "no-store") == 1U);
    CHECK(CountHeader(&output, "X-Content-Type-Options", "nosniff") == 1U);
    if (expectedHttp == 405) CHECK(CountHeader(&output, "Allow", "POST") == 1U);
    if (expectedHttp != 200) {
        CHECK(memcmp(before, store, sizeof *store) == 0);
        goto cleanup;
    }
    UmiUiValue updated;
    CHECK(umi_ui_reactive_observable_store_get(store, "practice.input", &updated) == UMI_STATUS_OK);
    CHECK(updated.kind == UMI_UI_VALUE_INTEGER && updated.integer_value == expectedValue);
    CHECK(store->revision == before->revision + 1U && store->items[0].revision == store->revision);
    CHECK(memcmp(&before->items[1], &store->items[1], sizeof store->items[1]) == 0);
    memcpy(before, store, sizeof *before);
    CHECK(UmiWebReactiveIntegerEditHandle(&request, &output, &edit) == UMI_STATUS_OK && output.status == 409);
    CHECK(memcmp(before, store, sizeof *store) == 0);

cleanup:
    umi_web_origin_policy_destroy(origins);
    free(before);
    free(store);
    return failed;
}
