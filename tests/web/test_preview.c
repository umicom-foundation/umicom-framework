/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_preview.c
 *
 * PURPOSE:
 *   Exercise preview admission, owned roots and the actual static-file and browser-gate composition without listening sockets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/web/preview.h"
static int Is(const char *a, const char *b) { return strcmp(a, b) == 0; }
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    const char *cases[] = {"null","relative-root","low-port","high-port","long-root","port-min","port-max","port-leading-zero","port-empty","port-sign","port-space","port-overflow","port-text","port-null","create-invalid","create-null-output","index","missing","traversal","post","head","foreign-host","foreign-origin","same-origin","duplicate-host","copied-root","unicode-root","binary","stopped-handle","serve-before-start"};

    size_t found = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) found += (size_t)Is(mode, cases[i]);
    if (found != 1U) return 2;
#ifdef _WIN32
    const char *syntactic_root = "C:/preview-fixture-not-accessed";
#else
    const char *syntactic_root = "/preview-fixture-not-accessed";
#endif
    if (strncmp(mode, "port-", 5U) == 0) {
        const char *text = ""; uint32_t expected = 0U, actual = 99U;
        if (Is(mode, "port-min")) { text = "1024"; expected = 1024U; }
        if (Is(mode, "port-max")) { text = "65535"; expected = 65535U; }
        if (Is(mode, "port-leading-zero")) { text = "0008080"; expected = 8080U; }
        if (Is(mode, "port-sign")) text = "+8080";
        if (Is(mode, "port-space")) text = "8080 ";
        if (Is(mode, "port-overflow")) text = "18446744073709551616";
        if (Is(mode, "port-text")) text = "80eight";
        if (Is(mode, "port-null")) text = NULL;
        CHECK(UmiWebPreviewPortParse(text, &actual) ==
            (expected != 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
        CHECK(actual == (expected != 0U ? expected : 99U));
        CHECK(UmiWebPreviewPortParse("8080", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (Is(mode, "null") || Is(mode, "relative-root") || Is(mode, "low-port") ||
        Is(mode, "high-port") || Is(mode, "long-root")) {
        char large[UMI_WEB_PATH_CAPACITY + 1U]; memset(large, 'a', sizeof(large));
        large[sizeof(large) - 1U] = '\0';
        const char *root_value = Is(mode, "null") ? NULL :
            Is(mode, "relative-root") ? "." : Is(mode, "long-root") ? large : syntactic_root;
        uint32_t port = Is(mode, "low-port") ? 1023U : Is(mode, "high-port") ? 65536U : 8080U;
        CHECK(UmiWebPreviewValidate(root_value, port) != UMI_STATUS_OK);
        return 0;
    }
    if (Is(mode, "create-invalid")) {
        UmiWebPreview *invalid = (UmiWebPreview *)(void *)&found;
        CHECK(UmiWebPreviewCreate(".", 8080U, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(invalid == NULL); return 0;
    }
    if (Is(mode, "create-null-output")) {
        CHECK(UmiWebPreviewCreate(syntactic_root, 8080U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    char fixture[UMI_PATH_CAPACITY], root_path[UMI_PATH_CAPACITY];
    FixtureDirectory(fixture);
    FixturePath(root_path, fixture, Is(mode, "unicode-root") ? "caf\xc3\xa9 files" : "site");
    CHECK(umi_fs_make_directories(root_path) == UMI_STATUS_OK);
    const unsigned char body[] = {'r','e','a','d','y'};
    const unsigned char binary[] = {0U, 0xffU, 10U, 0U};
    CHECK(UmiRootedFileWrite(root_path, "index.html", body, sizeof(body)) == UMI_STATUS_OK);
    CHECK(UmiRootedFileWrite(root_path, "data.bin", binary, sizeof(binary)) == UMI_STATUS_OK);
    UmiWebPreview *preview = NULL;
    CHECK(UmiWebPreviewCreate(root_path, 8080U, &preview) == UMI_STATUS_OK);
    if (Is(mode, "copied-root")) memset(root_path, 0, sizeof(root_path));
    if (Is(mode, "stopped-handle")) {
        CHECK(UmiWebPreviewStop(preview) == UMI_STATUS_OK);
        CHECK(UmiWebPreviewStop(preview) == UMI_STATUS_OK);
    }
    UmiWebExchangeResult exchange; memset(&exchange, 0xff, sizeof(exchange));
    CHECK(UmiWebPreviewServeNext(NULL, 100U, 100U, NULL, &exchange) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!exchange.dispatched && exchange.bytes_received == 0U);
    CHECK(UmiWebPreviewServeNext(preview, 100U, 100U, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    if (Is(mode, "serve-before-start")) {
        CHECK(UmiWebPreviewServeNext(preview, 100U, 100U, NULL, &exchange) == UMI_STATUS_INVALID_STATE);
        CHECK(exchange.bytes_received == 0U); UmiWebPreviewDestroy(preview); return 0;
    }
    const char *method = Is(mode, "post") ? "POST" : Is(mode, "head") ? "HEAD" : "GET";
    const char *target = Is(mode, "missing") ? "/absent.txt" :
        Is(mode, "traversal") ? "/%2e%2e/index.html" : Is(mode, "binary") ? "/data.bin" : "/";
    const char *host = Is(mode, "foreign-host") ? "example.invalid:8080" : "127.0.0.1:8080";
    const char *extra = Is(mode, "foreign-origin") ? "Origin: https://example.invalid\r\n" :
        Is(mode, "same-origin") ? "Origin: http://127.0.0.1:8080\r\nSec-Fetch-Site: same-origin\r\n" : "";
    char message[1024]; int count = snprintf(message, sizeof(message),
        "%s %s HTTP/1.1\r\nHost: %s\r\n%s\r\n", method, target, host, extra);
    CHECK(count > 0 && (size_t)count < sizeof(message));
    UmiWebRequest *request = calloc(1U, sizeof(*request));
    UmiWebResponse *response = calloc(1U, sizeof(*response));
    CHECK(request != NULL && response != NULL);
    CHECK(UmiWebRequestParseStrict(message, (size_t)count, sizeof(message), request) == UMI_STATUS_OK);
    /* A headless caller can supply a parsed structure. The composed gate must
     * still reject duplicates even when there was no wire parser in its path. */
    if (Is(mode, "duplicate-host")) {
        request->headers[request->header_count] = request->headers[0]; ++request->header_count;
    }
    CHECK(UmiWebPreviewHandle(preview, request, response) == UMI_STATUS_OK);
    int expected = Is(mode, "missing") ? 404 : Is(mode, "post") ? 405 :
        (Is(mode, "traversal") || Is(mode, "foreign-host") || Is(mode, "foreign-origin") ||
         Is(mode, "duplicate-host")) ? 403 : 200;
    CHECK(response->status == expected);
    if (expected == 200) {
        const unsigned char *expected_body = Is(mode, "binary") ? binary : body;
        size_t expected_bytes = Is(mode, "binary") ? sizeof(binary) : sizeof(body);
        CHECK(response->body_length == expected_bytes &&
            memcmp(response->body, expected_body, expected_bytes) == 0);
    }
    if (Is(mode, "head")) {
        unsigned char wire[2048]; size_t written = 0U;
        CHECK(UmiWebResponseFormatClosed(response, true, wire, sizeof(wire), &written) == UMI_STATUS_OK);
        CHECK(written >= 4U && memcmp(wire + written - 4U, "\r\n\r\n", 4U) == 0);
    }
    CHECK(UmiWebPreviewHandle(NULL, request, response) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebPreviewHandle(preview, NULL, response) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebPreviewHandle(preview, request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebPreviewStart(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiWebPreviewStop(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiWebPreviewDestroy(NULL); UmiWebPreviewDestroy(preview);
    free(request); free(response); return 0;
}
