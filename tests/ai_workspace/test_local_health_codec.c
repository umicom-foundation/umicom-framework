/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_health_codec.c
 *
 * PURPOSE:
 *   Check health response admission independently of any HTTP server or model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_internal.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

typedef struct HealthCase {
    const char *name;
    long http;
    const char *json;
    UmiStatus expected;
} HealthCase;
static const HealthCase cases[] = {
    {"ready", 200, "{\"status\":\"ok\"}", UMI_STATUS_OK},
    {"loading", 503, "{\"error\":{\"code\":503,\"message\":\"Loading model\",\"type\":\"unavailable_error\"}}", UMI_STATUS_OK},
    {"unicode", 503, "{\"error\":{\"code\":503,\"message\":\"Chargement — مرحبا\",\"type\":\"unavailable_error\"}}", UMI_STATUS_OK},
    {"space", 200, " \n{\"status\":\"ok\"}\t\r\n", UMI_STATUS_OK},
    {"trailing", 200, "{\"status\":\"ok\"} {}", UMI_STATUS_PARSE_ERROR},
    {"comment", 200, "{\"status\":\"ok\"/* comment */}", UMI_STATUS_PARSE_ERROR},
    {"wrong-status", 200, "{\"status\":\"loading\"}", UMI_STATUS_PARSE_ERROR},
    {"wrong-type", 200, "{\"status\":true}", UMI_STATUS_PARSE_ERROR},
    {"embedded-nul", 200, "{\"status\":\"ok\\u0000\"}", UMI_STATUS_PARSE_ERROR},
    {"error-with-ok", 200, "{\"status\":\"ok\",\"error\":null}", UMI_STATUS_PARSE_ERROR},
    {"array", 200, "[{\"status\":\"ok\"}]", UMI_STATUS_PARSE_ERROR},
    {"truncated", 200, "{\"status\":", UMI_STATUS_PARSE_ERROR},
    {"wrong-http", 201, "{\"status\":\"ok\"}", UMI_STATUS_IO_ERROR},
    {"redirect", 302, "{\"status\":\"ok\"}", UMI_STATUS_IO_ERROR},
    {"generic-busy", 503, "{\"error\":{\"code\":503,\"message\":\"Busy\",\"type\":\"other\"}}", UMI_STATUS_PARSE_ERROR},
    {"mismatched-code", 503, "{\"error\":{\"code\":500,\"message\":\"Loading\",\"type\":\"unavailable_error\"}}", UMI_STATUS_PARSE_ERROR},
    {"string-code", 503, "{\"error\":{\"code\":\"503\",\"message\":\"Loading\",\"type\":\"unavailable_error\"}}", UMI_STATUS_PARSE_ERROR},
    {"missing-message", 503, "{\"error\":{\"code\":503,\"type\":\"unavailable_error\"}}", UMI_STATUS_PARSE_ERROR},
    {"contradiction", 503, "{\"status\":\"ok\",\"error\":{\"code\":503,\"message\":\"Loading\",\"type\":\"unavailable_error\"}}", UMI_STATUS_PARSE_ERROR}
};
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!UmiAiWorkspaceLocalProviderAvailable()) {
        fputs("SKIP: optional local HTTP/JSON support was not built; no health codec exercised.\n", stderr);
        return 77;
    }
    UmiAiLocalHealth observed;
    unsigned char before[sizeof observed];
    memset(&observed, 0xa5, sizeof observed); memcpy(before, &observed, sizeof observed);
    if (!strcmp(argv[1], "bounds")) {
        char body[AW_HEALTH_BODY_CAPACITY]; memset(body, ' ', sizeof body);
        CHECK(AwLocalHealthDecode(200L, body, sizeof body, &observed) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!memcmp(before, &observed, sizeof observed));
        CHECK(AwLocalHealthDecode(200L, NULL, 1U, &observed) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalHealthDecode(200L, "{}", 0U, &observed) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(AwLocalHealthDecode(200L, "{}", 2U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(before, &observed, sizeof observed));
        return 0;
    }
    if (!strcmp(argv[1], "raw-nul") || !strcmp(argv[1], "invalid-utf8")) {
        char body[] = "{\"status\":\"ok\"} ";
        body[sizeof body - 2U] = !strcmp(argv[1], "raw-nul") ? '\0' : (char)0xff;
        CHECK(AwLocalHealthDecode(200L, body, sizeof body - 1U, &observed) == UMI_STATUS_PARSE_ERROR);
        CHECK(!memcmp(before, &observed, sizeof observed));
        return 0;
    }
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) {
        const HealthCase *test = &cases[i];
        if (strcmp(argv[1], test->name)) continue;
        CHECK(AwLocalHealthDecode(test->http, test->json, strlen(test->json), &observed) == test->expected);
        if (test->expected == UMI_STATUS_OK) {
            CHECK(observed.http_status == (uint32_t)test->http);
            CHECK(observed.endpoint_reports_ready == (test->http == 200L));
        } else CHECK(!memcmp(before, &observed, sizeof observed));
        return 0;
    }
    return 2;
}
