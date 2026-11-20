/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_cvm_preview.c
 *
 * PURPOSE:
 *   Verify binary HTTP admission, native VM results and bounded refusal together.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/cvm_preview.h"
#include "umicom/web/connection.h"
#include "umicom/web/loopback_access.h"
#include "umicom_c/umicom_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        exit(1); \
    } \
} while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const bool runtime_error = strcmp(mode, "runtime") == 0;
    const char *source = runtime_error ? "int main(void){return 8 / 0;}" :
        "int main(void){return (20 + 1) * 2;}";
    UmcInstruction instructions[32];
    uint8_t artifact[256];
    size_t count = 0U, length = 0U;
    CHECK(umc_compile(source, strlen(source), instructions, 32U, &count, NULL) == UMC_OK);
    CHECK(umc_encode(instructions, count, artifact, sizeof artifact, &length, NULL) == UMC_OK);
    CHECK(memchr(artifact, '\0', length) != NULL);

    const char *media = UMI_WEB_CVM_ARTIFACT_MEDIA_TYPE;
    const char *origin = "http://127.0.0.1:8090";
    const char *extra = "";
    const char *query = "";
    const char *method = "POST";
    unsigned expected = 200U;
    const char *fragment = "\"value\":42";
    UmiWebCvmPreviewConfig preview = {UMI_WEB_CVM_PREVIEW_MAX_FUEL};
    if (strcmp(mode, "uppercase") == 0) {
        media = "Application/Vnd.Umicom.C-Bytecode";
    } else if (strcmp(mode, "permissions") == 0) {
        origin = "http://localhost:8091";
        expected = 403U; fragment = "local browser address";
    } else if (strcmp(mode, "route") == 0) {
        method = "GET";
        expected = 404U; fragment = "Not Found";
    } else if (strcmp(mode, "query") == 0) {
        query = "?unexpected=true";
        expected = 400U; fragment = "\"error\":\"query\"";
    } else if (strcmp(mode, "bad-media") == 0) {
        media = "text/plain";
        expected = 415U; fragment = "content-type";
    } else if (strcmp(mode, "duplicate") == 0) {
        extra = "content-type: " UMI_WEB_CVM_ARTIFACT_MEDIA_TYPE "\r\n";
        expected = 415U; fragment = "content-type";
    } else if (strcmp(mode, "encoded") == 0) {
        extra = "Content-Encoding: gzip\r\n";
        expected = 415U; fragment = "content-type";
    } else if (strcmp(mode, "empty") == 0) {
        length = 0U;
        expected = 400U; fragment = "invalid-artifact";
    } else if (strcmp(mode, "oversized") == 0) {
        expected = 413U; fragment = "artifact-size";
    } else if (strcmp(mode, "truncated") == 0) {
        --length;
        expected = 400U; fragment = "invalid-artifact";
    } else if (runtime_error) {
        expected = 400U; fragment = "execution-refused";
    } else if (strcmp(mode, "fuel") == 0) {
        preview.fuel = 1U;
        expected = 400U; fragment = "instruction-budget";
    } else {
        CHECK(strcmp(mode, "binary") == 0 || strcmp(mode, "invalid-context") == 0);
    }
    char wire[1024];
    const int header = snprintf(wire, sizeof wire, "%s %s%s HTTP/1.1\r\nHost: 127.0.0.1:8090\r\n"
        "Origin: %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n%s\r\n",
        method, UMI_WEB_CVM_PREVIEW_PATH, query, origin, media, length, extra);
    CHECK(header > 0 && (size_t)header + length <= sizeof wire);
    memcpy(wire + header, artifact, length);
    UmiWebRequest *request = calloc(1U, sizeof *request);
    UmiWebResponse *response = calloc(1U, sizeof *response);
    CHECK(request != NULL && response != NULL);
    CHECK(UmiWebRequestParseStrict(wire, (size_t)header + length, sizeof wire, request) == UMI_STATUS_OK);
    CHECK(request->body_length == length && memcmp(request->body, artifact, length) == 0);
    if (strcmp(mode, "oversized") == 0) {
        /* Direct callers must respect the parser's inline model too. */
        request->body_length = sizeof request->body;
    }
    if (strcmp(mode, "invalid-context") == 0) {
        preview.fuel = (size_t)UMI_WEB_CVM_PREVIEW_MAX_FUEL + 1U;
        CHECK(UmiWebCvmPreviewHandle(request, response, &preview) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
        UmiWebService *service = NULL;
        CHECK(umi_web_service_create(&service) == UMI_STATUS_OK);
        UmiWebLoopbackAccess access = {8090U};
        CHECK(UmiWebServiceSetRequestGate(service, UmiWebLoopbackRequestGate, &access) == UMI_STATUS_OK);
        UmiWebRoute route;
        CHECK(umi_web_route_init(&route, UMI_HTTP_METHOD_POST, UMI_WEB_CVM_PREVIEW_PATH,
            UmiWebCvmPreviewHandle, &preview) == UMI_STATUS_OK);
        CHECK(umi_web_router_add(umi_web_service_router(service), &route) == UMI_STATUS_OK);
        CHECK(umi_web_service_handle(service, request, response) == UMI_STATUS_OK);
        CHECK(response->status == (int)expected && strstr(response->body, fragment) != NULL);
        if (expected != 403U && expected != 404U) {
            const char *cache = NULL;
            for (size_t i = 0U; i < response->header_count; ++i) {
                if (umi_web_header_name_equal(&response->headers[i], "Cache-Control")) {
                    cache = response->headers[i].value;
                }
            }
            CHECK(cache != NULL && strcmp(cache, "no-store") == 0);
        }
        if (expected != 200U) {
            CHECK(strstr(response->body, "\"value\"") == NULL);
        }
        umi_web_service_destroy(service);
    }
    free(request);
    free(response);
    return 0;
}
