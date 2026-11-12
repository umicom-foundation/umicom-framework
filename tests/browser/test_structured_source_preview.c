/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/browser/test_structured_source_preview.c
 *
 * PURPOSE:
 *   Verify owned reading results, single projection, failure-atomic publication and complete built-in catalogue installation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/browser/source_preview.h"
#include "umicom/browser/reading_markdown.h"

static UmiStatus Custom(void *context, const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *result)
{
    const char *mode = context;
    umi_browser_source_preview_result_init(result);
    strcpy(result->provider_id, "test.custom"); strcpy(result->content, "custom");
    result->content_bytes = 6U; result->source_bytes = request->source_length;
    result->source_revision = request->source_revision;
    result->kind = UMI_BROWSER_SOURCE_PREVIEW_READABLE_DOCUMENT;
    if (strcmp(mode, "callback-failure") == 0) return UMI_STATUS_IO_ERROR;
    if (strcmp(mode, "callback-stale") == 0) --result->source_revision;
    return UMI_STATUS_OK;
}
static void Register(UmiBrowserSourcePreviewService *service, const char *id, const char *mode)
{
    UmiBrowserSourcePreviewProvider provider;
    umi_browser_source_preview_provider_init(&provider);
    snprintf(provider.provider_id, sizeof(provider.provider_id), "%s", id);
    strcpy(provider.title, "Custom");
    provider.priority = 500U; provider.render = Custom; provider.provider_context = (void *)mode;
    assert(umi_browser_source_preview_service_register(service, &provider) == UMI_STATUS_OK);
}
int main(int argc, char **argv)
{
    assert(argc == 2);
    const char *mode = argv[1];
    UmiBrowserSourcePreviewService *service = NULL;
    assert(umi_browser_source_preview_service_create(&service) == UMI_STATUS_OK);
    if (strncmp(mode, "install-", 8U) == 0) {
        size_t count = strcmp(mode, "install-capacity") == 0 ? 27U :
                       strcmp(mode, "install-exact") == 0 ? 26U : 1U;
        UmiBrowserSourcePreviewProvider before[UMI_BROWSER_SOURCE_PREVIEW_PROVIDER_CAPACITY];
        for (size_t i = 0U; i < count; ++i) {
            char identity[128];
            if (strcmp(mode, "install-duplicate") == 0) strcpy(identity, "umicom.preview.readable-md");
            else snprintf(identity, sizeof(identity), "test.custom.%zu", i);
            Register(service, identity, "custom");
            assert(umi_browser_source_preview_service_at(service, i, &before[i]) == UMI_STATUS_OK);
        }
        UmiStatus wanted = strcmp(mode, "install-exact") == 0 ? UMI_STATUS_OK :
            strcmp(mode, "install-duplicate") == 0 ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_CAPACITY_EXCEEDED;
        assert(umi_browser_source_preview_service_install_builtins(service) == wanted);
        assert(umi_browser_source_preview_service_count(service) == count + (wanted == UMI_STATUS_OK ? 6U : 0U));
        for (size_t i = 0U; i < count; ++i) {
            UmiBrowserSourcePreviewProvider after;
            assert(umi_browser_source_preview_service_at(service, i, &after) == UMI_STATUS_OK);
            assert(memcmp(&after, &before[i], sizeof(after)) == 0);
        }
        umi_browser_source_preview_service_destroy(service); return 0;
    }
    assert(umi_browser_source_preview_service_install_builtins(service) == UMI_STATUS_OK);
    UmiBrowserSourcePreviewRequest request;
    umi_browser_source_preview_request_init(&request);
    request.source = "# Title\n\nRead **owned** text.\n\n~~~\n  code\n~~~";
    request.source_length = strlen(request.source); request.source_revision = 42U;
    request.path = "practice.md";
    UmiBrowserSourcePreviewResult *result = malloc(sizeof(*result)), *before = malloc(sizeof(*before));
    assert(result != NULL && before != NULL);
    umi_browser_source_preview_result_init(result);
    strcpy(result->content, "# Stored source");
    strcpy(result->title, "retained.md");
    UmiBrowserReadingDocument *prior = NULL, *reading;
    assert(UmiBrowserReadingMarkdownCreate("prior", 5U, &prior) == UMI_STATUS_OK);
    reading = prior;
    UmiStatus wanted = UMI_STATUS_OK;
    int expect_model = 1;
    const char *identity = "umicom.preview.readable-md";
    if (strcmp(mode, "markdown") == 0) { request.path = "practice.MARKDOWN"; identity = "umicom.preview.readable-markdown"; }
    else if (strcmp(mode, "language") == 0) { request.path = "Untitled"; request.language_id = "MARKDOWN"; identity = "umicom.preview.markdown-language"; }
    else if (strcmp(mode, "html-priority") == 0) {
        request.path = "practice.html"; request.language_id = "markdown";
        request.source = "<p>HTML</p>"; expect_model = 0; identity = "umicom.preview.readable-html";
    } else if (strcmp(mode, "plain") == 0) {
        request.path = "practice.c"; expect_model = 0; identity = "umicom.preview.plain-text";
    } else if (strcmp(mode, "shortened") == 0) {
        request.source = "# \xE2\x82\xAC"; request.maximum_output_bytes = 2U; expect_model = 0;
    } else if (strcmp(mode, "empty") == 0) request.source = "";
    else if (strcmp(mode, "alias") == 0) { request.source = result->content; request.path = result->title; }
    else if (strcmp(mode, "invalid-source") == 0) { request.source = "\xFF"; wanted = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "null-output") == 0) wanted = UMI_STATUS_INVALID_ARGUMENT;
    else if (strcmp(mode, "callback-failure") == 0 || strcmp(mode, "callback-stale") == 0 ||
             strcmp(mode, "custom") == 0) {
        Register(service, "test.custom", mode); identity = "test.custom"; expect_model = 0;
        if (strcmp(mode, "callback-failure") == 0) wanted = UMI_STATUS_IO_ERROR;
        if (strcmp(mode, "callback-stale") == 0) wanted = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "copied-callback-id") == 0) {
        UmiBrowserSourcePreviewProvider provider;
        assert(umi_browser_source_preview_service_at(service, 2U, &provider) == UMI_STATUS_OK);
        assert(strcmp(provider.provider_id, "umicom.preview.readable-md") == 0);
        strcpy(provider.provider_id, "test.copied"); provider.priority = 900U;
        assert(umi_browser_source_preview_service_register(service, &provider) == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_STATE;
    } else assert(strcmp(mode, "md") == 0 || strcmp(mode, "legacy") == 0);
    request.source_length = strlen(request.source);
    memcpy(before, result, sizeof(*before));
    UmiStatus status = strcmp(mode, "legacy") == 0
        ? umi_browser_source_preview_service_render(service, &request, result)
        : umi_browser_source_preview_service_render_reading(service, &request, result,
            strcmp(mode, "null-output") == 0 ? NULL : &reading);
    assert(status == wanted);
    if (status != UMI_STATUS_OK) {
        assert(reading == prior);
        assert(memcmp(result, before, sizeof(*result)) == 0);
    } else if (strcmp(mode, "legacy") == 0) {
        assert(reading == prior);
        assert(strcmp(result->provider_id, "umicom.preview.readable-md") == 0);
        assert(strcmp(result->content, "Title\nRead owned text.\n  code\n") == 0);
    } else {
        assert((reading != NULL) == expect_model);
        assert(strcmp(result->provider_id, identity) == 0);
        assert(result->source_revision == 42U);
        if (reading != NULL) {
            UmiBrowserReadingSummary summary;
            assert(UmiBrowserReadingDocumentSummary(reading, &summary) == UMI_STATUS_OK);
            assert(summary.source_bytes == request.source_length);
            assert(strcmp(UmiBrowserReadingDocumentText(reading), result->content) == 0);
            if (strcmp(mode, "md") == 0 || strcmp(mode, "markdown") == 0 || strcmp(mode, "language") == 0)
                assert(summary.spans == 3U);
            UmiBrowserReadingDocumentDestroy(reading);
        }
        if (strcmp(mode, "shortened") == 0) assert(result->truncated == 1 && result->content_bytes == 0U);
        if (strcmp(mode, "empty") == 0) assert(result->content_bytes == 0U);
    }
    UmiBrowserReadingDocumentDestroy(prior);
    free(result); free(before);
    umi_browser_source_preview_service_destroy(service);
    return 0;
}
