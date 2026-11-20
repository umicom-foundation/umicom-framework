/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_cvm_int_input.c
 *
 * PURPOSE:
 *   Verify local HTTP admission and native execution for a required-input artifact.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/cvm_preview.h"
#include "umicom/toolchain/cvm_preview.h"
#include "umicom/web/connection.h"
#include "umicom/web/loopback_access.h"
#if defined(__has_include)
#if __has_include("umicom_c/int_input.h")
#include "umicom_c/int_input.h"
#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        exit(1); \
    } \
} while (0)

typedef struct FixtureCase {
    const char *name;
    const char *query;
    int code;
    const char *body;
    bool needs_input_sdk;
} FixtureCase;

static const FixtureCase cases[] = {
    {"sdk", "?input=20", 200, "{\"value\":42,\"instructions\":16}\n", false},
    {"positive", "?input=20", 200, "{\"value\":42,\"instructions\":16}\n", true},
    {"negative", "?input=-5", 200, "{\"value\":-10,\"instructions\":12}\n", true},
    {"missing", "", 400, "{\"error\":\"query\"}\n", false},
    {"empty", "?input=", 400, "{\"error\":\"query\"}\n", false},
    {"duplicate", "?input=20&input=21", 400, "{\"error\":\"query\"}\n", false},
    {"extra", "?input=20&other=1", 400, "{\"error\":\"query\"}\n", false},
    {"trailing", "?input=20&", 400, "{\"error\":\"query\"}\n", false},
    {"decimal", "?input=2.0", 400, "{\"error\":\"query\"}\n", false},
    {"range", "?input=-2147483649", 400, "{\"error\":\"query\"}\n", false},
    {"encoded", "?input=%2D5", 400, "{\"error\":\"query\"}\n", false},
    {"malformed-field", "?input=20", 400, "{\"error\":\"query\"}\n", false},
    {"prefix", "?input=20", 400, "{\"error\":\"invalid-artifact\"}\n", true},
    {"ordinary", "?input=20", 400, "{\"error\":\"invalid-artifact\"}\n", true},
    {"old-route", "", 400, "{\"error\":\"invalid-artifact\"}\n", true},
    {"origin", "?input=20", 403, "local browser address", false},
    {"arithmetic", "?input=1073741823", 400, "{\"error\":\"execution-refused\"}\n", true},
    {"fuel", "?input=20", 400, "{\"error\":\"instruction-budget\"}\n", true},
    {"media", "?input=20", 415, "{\"error\":\"content-type\"}\n", false},
};

static size_t MakeArtifact(uint8_t *bytes, size_t capacity, bool ordinary)
{
    size_t size = 0U;
#ifdef UMC_INT_INPUT_BYTECODE_FLAG
    if (!ordinary) {
        const char example[] = "int umicom_run(int x) { int total = x * 2; if(x > 0) { total += 2; } return total; }";
        CHECK(umc_compile_int_input(example, sizeof example - 1U, bytes, capacity, &size, NULL) == UMC_OK);
        return size;
    }
#else
    (void)ordinary;
#endif
    const char source[] = "int main(void) { return 42; }";
    UmcInstruction instructions[16];
    size_t count = 0U;
    CHECK(umc_compile(source, sizeof source - 1U, instructions, 16U, &count, NULL) == UMC_OK);
    CHECK(umc_encode(instructions, count, bytes, capacity, &size, NULL) == UMC_OK);
    return size;
}

static int AdapterStale(void)
{
    UmiCvmPreviewWorkspace workspace;
    UmiCvmPreviewResult result;
    const char source[] = "int main(void) { return 42; }";
    CHECK(umi_cvm_preview(source, sizeof source - 1U, 64U,
        &workspace, sizeof workspace, &result, sizeof result) == UMI_STATUS_OK);
    CHECK(result.completed && result.execution.value == 42);
    static const uint8_t invalid[] = {'U', 'M', 'C', 'B'};
    UmiStatus status = umi_cvm_preview_int_input_bytecode(invalid, sizeof invalid, 20, 64U,
        &workspace, sizeof workspace, &result, sizeof result);
#ifdef UMC_INT_INPUT_BYTECODE_FLAG
    CHECK(status == UMI_STATUS_PARSE_ERROR && result.diagnostic.status == UMC_BAD_BYTECODE);
#else
    CHECK(status == UMI_STATUS_UNAVAILABLE && result.diagnostic.message == NULL);
#endif
    CHECK(!result.completed && result.phase == UMI_CVM_DECODING && result.instruction_count == 0U);
    CHECK(result.execution.value == 0 && result.execution.instructions_executed == 0U);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    if (strcmp(mode, "adapter-stale") == 0) return AdapterStale();
    const FixtureCase *selected = NULL;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(cases[i].name, mode) == 0) selected = &cases[i];
    CHECK(selected != NULL);
    int expected_code = selected->code;
    const char *expected_body = selected->body;
#ifndef UMC_INT_INPUT_BYTECODE_FLAG
    if (selected->needs_input_sdk) return 77;
    if (strcmp(mode, "sdk") == 0) {
        expected_code = 503;
        expected_body = "{\"error\":\"input-unavailable\"}\n";
    }
#endif
    uint8_t artifact[512];
    const size_t size = MakeArtifact(artifact, sizeof artifact, strcmp(mode, "ordinary") == 0);
    if (strcmp(mode, "prefix") == 0) artifact[UMC_BYTECODE_HEADER_SIZE + 1U] = 1U;
    const bool old_route = strcmp(mode, "old-route") == 0;
    const char *path = old_route ? UMI_WEB_CVM_PREVIEW_PATH : UMI_WEB_CVM_INT_INPUT_PATH;
    const char *origin = strcmp(mode, "origin") == 0 ? "http://127.0.0.1:8091" : "http://127.0.0.1:8090";
    const char *media = strcmp(mode, "media") == 0 ? "text/plain" : UMI_WEB_CVM_ARTIFACT_MEDIA_TYPE;
    char wire[1024];
    const int header = snprintf(wire, sizeof wire, "POST %s%s HTTP/1.1\r\nHost: 127.0.0.1:8090\r\n"
        "Origin: %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n\r\n", path, selected->query, origin, media, size);
    CHECK(header > 0 && (size_t)header + size <= sizeof wire);
    memcpy(wire + (size_t)header, artifact, size);
    UmiWebRequest *request = calloc(1U, sizeof *request);
    UmiWebResponse *response = calloc(1U, sizeof *response);
    CHECK(request != NULL && response != NULL);
    CHECK(UmiWebRequestParseStrict(wire, (size_t)header + size, sizeof wire, request) == UMI_STATUS_OK);
    if (strcmp(mode, "malformed-field") == 0) memset(request->query, '0', sizeof request->query);
    UmiWebCvmPreviewConfig config = {strcmp(mode, "fuel") == 0 ? 1U : 64U};
    UmiWebService *service = NULL;
    UmiWebLoopbackAccess access = {8090U};
    UmiWebRoute route;
    CHECK(umi_web_service_create(&service) == UMI_STATUS_OK);
    CHECK(UmiWebServiceSetRequestGate(service, UmiWebLoopbackRequestGate, &access) == UMI_STATUS_OK);
    CHECK(umi_web_route_init(&route, UMI_HTTP_METHOD_POST, path,
        old_route ? UmiWebCvmPreviewHandle : UmiWebCvmPreviewIntInputHandle, &config) == UMI_STATUS_OK);
    CHECK(umi_web_router_add(umi_web_service_router(service), &route) == UMI_STATUS_OK);
    CHECK(umi_web_service_handle(service, request, response) == UMI_STATUS_OK);
    CHECK(response->status == expected_code);
    if (expected_code == 403) {
        CHECK(strstr(response->body, expected_body) != NULL);
    } else {
        CHECK(strcmp(response->body, expected_body) == 0);
        unsigned evidence = 0U;
        for (size_t i = 0U; i < response->header_count; ++i) {
            const UmiWebHeader *field = &response->headers[i];
            if (umi_web_header_name_equal(field, "Cache-Control") && strcmp(field->value, "no-store") == 0) evidence |= 1U;
            if (umi_web_header_name_equal(field, "X-Content-Type-Options") && strcmp(field->value, "nosniff") == 0) evidence |= 2U;
        }
        CHECK(evidence == 3U);
    }
    CHECK(request->body_length == size && memcmp(request->body, artifact, size) == 0);
    umi_web_service_destroy(service);
    free(request);
    free(response);
    return 0;
}
