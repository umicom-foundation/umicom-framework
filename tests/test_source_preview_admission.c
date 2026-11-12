/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_source_preview_admission.c
 *
 * PURPOSE:
 *   Exercise bounded source, Unicode presentation, provider identity and failure-atomic preview publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/base/utf8.h"
#include "umicom/browser/source_preview.h"

typedef struct Fixture {
    const char *mode;
    int calls;
    UmiBrowserSourcePreviewService *service;
} Fixture;

/* A real service invokes this deterministic provider; it never executes source. */
static UmiStatus fixture_render(void *context,
    const UmiBrowserSourcePreviewRequest *request, UmiBrowserSourcePreviewResult *result)
{
    Fixture *fixture = (Fixture *)context;
    ++fixture->calls;
    assert(request->source[request->source_length] == '\0');
    assert(strlen(request->source) == request->source_length);
    umi_browser_source_preview_result_init(result);
    strcpy(result->provider_id, "test.preview.fixture");
    strcpy(result->title, "Fixture");
    strcpy(result->summary, "Deterministic preview");
    strcpy(result->content, "ok");
    result->kind = UMI_BROWSER_SOURCE_PREVIEW_TEXT;
    result->source_bytes = request->source_length;
    result->source_revision = request->source_revision;
    result->content_bytes = 2U;
    if (strcmp(fixture->mode, "stale") == 0) --result->source_revision;
    else if (strcmp(fixture->mode, "identity") == 0) strcpy(result->provider_id, "wrong");
    else if (strcmp(fixture->mode, "content-utf8") == 0) result->content[0] = (char)0xFF;
    else if (strcmp(fixture->mode, "title-utf8") == 0) result->title[0] = (char)0xFF;
    else if (strcmp(fixture->mode, "summary-utf8") == 0) result->summary[0] = (char)0xFF;
    else if (strcmp(fixture->mode, "unterminated") == 0) memset(result->content, 'x', sizeof(result->content));
    else if (strcmp(fixture->mode, "length") == 0) ++result->content_bytes;
    else if (strcmp(fixture->mode, "source-length") == 0) ++result->source_bytes;
    else if (strcmp(fixture->mode, "truncated-flag") == 0) result->truncated = 2;
    else if (strcmp(fixture->mode, "executed-flag") == 0) result->executed_source = 2;
    else if (strcmp(fixture->mode, "kind") == 0) result->kind = UMI_BROWSER_SOURCE_PREVIEW_NONE;
    else if (strcmp(fixture->mode, "version") == 0) ++result->api_version;
    else if (strcmp(fixture->mode, "size") == 0) --result->structure_size;
    else if (strcmp(fixture->mode, "failure") == 0) return UMI_STATUS_IO_ERROR;
    else if (strcmp(fixture->mode, "unregister") == 0)
        assert(umi_browser_source_preview_service_unregister(fixture->service,
            "test.preview.fixture") == UMI_STATUS_OK);
    else if (strncmp(fixture->mode, "trust-", 6U) == 0) result->executed_source = 1;
    return UMI_STATUS_OK;
}

static UmiBrowserSourcePreviewRequest request_for(const char *text, size_t count)
{
    UmiBrowserSourcePreviewRequest request;
    umi_browser_source_preview_request_init(&request);
    request.source = text;
    request.source_length = count;
    request.path = "sample.c";
    request.language_id = "c";
    request.source_revision = 17U;
    return request;
}

int main(int argc, char **argv)
{
    UmiBrowserSourcePreviewService *service = NULL;
    UmiBrowserSourcePreviewResult *result = malloc(sizeof(*result));
    UmiBrowserSourcePreviewResult *before = malloc(sizeof(*before));
    UmiBrowserSourcePreviewRequest request = request_for("text", 4U);
    UmiBrowserSourcePreviewProvider provider;
    Fixture fixture = {0};
    char *buffer = malloc(UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY);
    UmiStatus expected = UMI_STATUS_OK, status;
    const char *mode;
    int callback_case = 0;
    assert(argc == 2 && result != NULL && before != NULL && buffer != NULL);
    mode = argv[1];
    fixture.mode = mode;
    assert(umi_browser_source_preview_service_create(&service) == UMI_STATUS_OK);
    fixture.service = service;
    assert(umi_browser_source_preview_service_install_builtins(service) == UMI_STATUS_OK);
    memset(result, 0x5A, sizeof(*result));

    if (strcmp(mode, "htm") == 0 || strcmp(mode, "html-unicode") == 0) {
        request = request_for("<p>A\xE2\x82\xAC" "B</p>", 13U);
        request.source_length = strlen(request.source);
        request.path = strcmp(mode, "htm") == 0 ? "page.HTM" : "page.html";
        request.maximum_output_bytes = 3U;
    } else if (strncmp(mode, "scalar-", 7U) == 0) {
        request = request_for("A\xF0\x9F\x8C\x8D" "B", 6U);
        request.maximum_output_bytes = (size_t)(mode[7] - '0');
        assert(mode[7] >= '1' && mode[7] <= '6' && mode[8] == '\0');
    } else if (strcmp(mode, "title-unicode") == 0) {
        memset(buffer, 'a', 126U);
        memcpy(buffer + 126U, "\xE2\x82\xAC.c", 6U);
        request.path = buffer;
    } else if (strcmp(mode, "html-title") == 0) {
        strcpy(buffer, "<title>");
        memset(buffer + 7U, 'a', 254U);
        strcpy(buffer + 261U, "\xE2\x82\xAC</title><p>body</p>");
        request = request_for(buffer, strlen(buffer));
        request.path = "page.html";
    } else if (strcmp(mode, "exact-span") == 0 || strcmp(mode, "terminated-snapshot") == 0) {
        /* Deliberately no NUL in the owned source object. Sanitizers can detect
         * any attempt to scan beyond these two readable bytes. */
        free(buffer); buffer = malloc(2U); assert(buffer != NULL);
        buffer[0] = 'o'; buffer[1] = 'k';
        request = request_for(buffer, 2U);
        callback_case = strcmp(mode, "terminated-snapshot") == 0;
    } else if (strcmp(mode, "alias") == 0) {
        umi_browser_source_preview_result_init(result);
        strcpy(result->content, "old source");
        strcpy(result->title, "old.c");
        request = request_for(result->content, 10U);
        request.path = result->title;
    } else if (strcmp(mode, "empty") == 0) request = request_for("", 0U);
    else if (strcmp(mode, "span-limit") == 0) {
        memset(buffer, 'x', UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY);
        request = request_for(buffer, UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY - 1U);
    } else if (strcmp(mode, "span-over") == 0) {
        request.source_length = UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "embedded-nul") == 0) {
        request = request_for("a\0b", 3U); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "source-utf8") == 0) {
        request = request_for("\xED\xA0\x80", 3U); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "path-limit") == 0 || strcmp(mode, "language-limit") == 0) {
        memset(buffer, 'x', UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY);
        if (strcmp(mode, "path-limit") == 0) request.path = buffer;
        else request.language_id = buffer;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "path-utf8") == 0) {
        request.path = "\xFF"; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "language-utf8") == 0) {
        request.language_id = "\xFF"; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "request-trust") == 0) {
        request.workspace_trusted = 2; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "request-consent") == 0) {
        request.allow_source_execution = -1; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "budget-zero") == 0) {
        request.maximum_output_bytes = 0U; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "budget-over") == 0) {
        request.maximum_output_bytes = UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "revision-zero") == 0) {
        request.source_revision = 0U; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "null-source") == 0) {
        request.source = NULL; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else callback_case = 1;

    if (callback_case) {
        umi_browser_source_preview_provider_init(&provider);
        strcpy(provider.provider_id, "test.preview.fixture");
        strcpy(provider.title, "Fixture");
        provider.priority = 900U;
        provider.render = fixture_render;
        provider.provider_context = &fixture;
        if (strncmp(mode, "provider-", 9U) == 0) {
            if (strcmp(mode, "provider-title") == 0) provider.title[0] = (char)0xFF;
            else if (strcmp(mode, "provider-id") == 0) provider.provider_id[0] = (char)0xFF;
            else if (strcmp(mode, "provider-language") == 0) strcpy(provider.language_id, "\xFF");
            else if (strcmp(mode, "provider-extension") == 0) strcpy(provider.extension, ".\xFF");
            else if (strcmp(mode, "provider-execute") == 0) provider.executes_source = 2;
            else if (strcmp(mode, "provider-trust") == 0) provider.requires_trusted_workspace = -1;
            else assert(0);
            assert(umi_browser_source_preview_service_register(service, &provider) == UMI_STATUS_INVALID_ARGUMENT);
            /* The original HTML/text catalogue count is retained for review.
     * Three shared Markdown selectors now extend that catalogue. */
#if 0
    assert(umi_browser_source_preview_service_count(service) == 3U);
#else
    assert(umi_browser_source_preview_service_count(service) == 6U);
#endif
            callback_case = 0;
        } else {
            if (strncmp(mode, "trust-", 6U) == 0) {
                provider.executes_source = 1;
                /* Omitting requires_trusted_workspace must not bypass trust. */
                request.workspace_trusted = strcmp(mode, "trust-consent-only") != 0;
                request.allow_source_execution = strcmp(mode, "trust-workspace-only") != 0;
            } else if (strcmp(mode, "unregister") != 0 && strcmp(mode, "terminated-snapshot") != 0) {
                expected = strcmp(mode, "failure") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_INVALID_STATE;
            }
            if (strcmp(mode, "over-budget") == 0) request.maximum_output_bytes = 1U;
            assert(umi_browser_source_preview_service_register(service, &provider) == UMI_STATUS_OK);
        }
    }
    memcpy(before, result, sizeof(*before));
    status = umi_browser_source_preview_service_render(service, &request, result);
    assert(status == expected);
    if (status != UMI_STATUS_OK) assert(memcmp(result, before, sizeof(*result)) == 0);
    else {
        assert(UmiUtf8Validate((const unsigned char *)result->content, result->content_bytes, NULL));
        assert(UmiUtf8Validate((const unsigned char *)result->title, strlen(result->title), NULL));
        assert(result->source_revision == request.source_revision);
        assert(result->source_bytes == request.source_length);
        if (strcmp(mode, "htm") == 0)
            assert(strcmp(result->provider_id, "umicom.preview.readable-htm") == 0);
        if (strcmp(mode, "htm") == 0 || strcmp(mode, "html-unicode") == 0)
            assert(strcmp(result->content, "A") == 0 && result->truncated == 1);
        if (strncmp(mode, "scalar-", 7U) == 0) {
            size_t wanted = request.maximum_output_bytes < 5U ? 1U : request.maximum_output_bytes;
            assert(result->content_bytes == wanted);
            assert(result->truncated == (wanted < 6U));
        }
        if (strcmp(mode, "title-unicode") == 0) assert(strlen(result->title) == 126U);
        if (strcmp(mode, "html-title") == 0) assert(strlen(result->title) == 127U);
        if (strcmp(mode, "exact-span") == 0) assert(strcmp(result->content, "ok") == 0);
        if (strcmp(mode, "alias") == 0) {
            assert(strcmp(result->content, "old source") == 0);
            assert(strcmp(result->title, "old.c") == 0);
        }
        if (strcmp(mode, "empty") == 0) assert(result->content_bytes == 0U && !result->truncated);
        if (strcmp(mode, "span-limit") == 0)
            assert(result->content_bytes == UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY - 1U);
        if (strncmp(mode, "trust-", 6U) == 0) {
            int allowed = strcmp(mode, "trust-both") == 0;
            assert(fixture.calls == allowed);
            assert(result->executed_source == allowed);
        }
        if (strcmp(mode, "terminated-snapshot") == 0) assert(fixture.calls == 1 && strcmp(result->content, "ok") == 0);
        if (strcmp(mode, "unregister") == 0) {
            assert(fixture.calls == 1);
            assert(strcmp(result->provider_id, "test.preview.fixture") == 0);
            /* The original HTML/text catalogue count is retained for review.
     * Three shared Markdown selectors now extend that catalogue. */
#if 0
    assert(umi_browser_source_preview_service_count(service) == 3U);
#else
    assert(umi_browser_source_preview_service_count(service) == 6U);
#endif
        }
    }
    free(buffer); free(before); free(result);
    umi_browser_source_preview_service_destroy(service);
    return 0;
}
