/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_browser_source_preview.c
 *
 * PURPOSE:
 *   Verify provider selection, readable document output, stale-result checks
 *   and the trust gate that protects previews capable of executing source.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/browser/source_preview.h"

/* Simulate an executable provider without actually running external code. */
static UmiStatus render_executable_fixture(
    void *provider_context,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result)
{
    int *call_count = (int *)provider_context;
    static const char CONTENT[] = "sandbox result";

    ++(*call_count);
    umi_browser_source_preview_result_init(out_result);
    out_result->kind = UMI_BROWSER_SOURCE_PREVIEW_NATIVE_DOCUMENT;
    (void)strcpy(out_result->provider_id, "test.preview.executable");
    (void)strcpy(out_result->title, "Executable fixture");
    (void)strcpy(out_result->summary, "Executed by the test fixture.");
    (void)strcpy(out_result->content, CONTENT);
    out_result->source_bytes = request->source_length;
    out_result->content_bytes = sizeof(CONTENT) - 1U;
    out_result->source_revision = request->source_revision;
    out_result->executed_source = 1;
    return UMI_STATUS_OK;
}

/* Return the wrong revision to prove that slow, stale provider output is rejected. */
static UmiStatus render_stale_fixture(
    void *provider_context,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result)
{
    (void)provider_context;
    umi_browser_source_preview_result_init(out_result);
    out_result->kind = UMI_BROWSER_SOURCE_PREVIEW_TEXT;
    (void)strcpy(out_result->provider_id, "test.preview.stale");
    (void)strcpy(out_result->title, "Stale fixture");
    (void)strcpy(out_result->summary, "Deliberately stale output.");
    (void)strcpy(out_result->content, "stale");
    out_result->source_bytes = request->source_length;
    out_result->content_bytes = 5U;
    out_result->source_revision = request->source_revision - 1U;
    return UMI_STATUS_OK;
}

/* Fill the common request fields while each scenario controls trust and consent. */
static UmiBrowserSourcePreviewRequest request_for(
    const char *path,
    const char *language_id,
    const char *source,
    uint64_t revision)
{
    UmiBrowserSourcePreviewRequest request;

    umi_browser_source_preview_request_init(&request);
    request.path = path;
    request.language_id = language_id;
    request.source = source;
    request.source_length = strlen(source);
    request.source_revision = revision;
    return request;
}

/* Exercise the complete service contract as one user-visible preview workflow. */
int main(void)
{
    static const char HTML[] =
        "<html><head><title>Live page</title><style>hidden</style></head>"
        "<body><h1>Preview body</h1><script>not executed</script></body></html>";
    static const char LONG_PATH[] =
        "workspace/a-very-long-folder-name/a-very-long-folder-name/"
        "a-very-long-folder-name/a-very-long-folder-name/"
        "a-very-long-folder-name/a-very-long-folder-name/"
        "source-file-with-a-readable-name.c";
    UmiBrowserSourcePreviewService *service = NULL;
    UmiBrowserSourcePreviewProvider provider;
    UmiBrowserSourcePreviewRequest request;
    UmiBrowserSourcePreviewResult result;
    int executable_calls = 0;

    assert(umi_browser_source_preview_service_create(&service) ==
           UMI_STATUS_OK);
    assert(service != NULL);
    assert(umi_browser_source_preview_service_install_builtins(service) ==
           UMI_STATUS_OK);
    assert(umi_browser_source_preview_service_count(service) == 3U);

    /* HTML is converted to readable content and hidden executable text is removed. */
    request = request_for("lesson.html", "html", HTML, 7U);
    assert(umi_browser_source_preview_service_render(
        service, &request, &result) == UMI_STATUS_OK);
    assert(result.kind == UMI_BROWSER_SOURCE_PREVIEW_READABLE_DOCUMENT);
    assert(strcmp(result.provider_id, "umicom.preview.readable-html") == 0);
    assert(strcmp(result.title, "Live page") == 0);
    assert(strstr(result.content, "Preview body") != NULL);
    assert(strstr(result.content, "hidden") == NULL);
    assert(strstr(result.content, "not executed") == NULL);
    assert(result.source_revision == 7U);
    assert(result.executed_source == 0);

    /* The low-priority plain provider gives every unsupported source a safe fallback. */
    request = request_for("main.c", "c", "int main(void) { return 0; }", 8U);
    assert(umi_browser_source_preview_service_render(
        service, &request, &result) == UMI_STATUS_OK);
    assert(strcmp(result.provider_id, "umicom.preview.plain-text") == 0);
    assert(strcmp(result.content, request.source) == 0);
    assert(result.executed_source == 0);

    /* Long display paths are safely shortened instead of producing an empty
     * panel heading or changing the stable provider identity. */
    request = request_for(LONG_PATH, "c", "int value = 1;", 81U);
    assert(umi_browser_source_preview_service_render(
        service, &request, &result) == UMI_STATUS_OK);
    assert(result.title[0] != '\0');
    assert(result.title[sizeof(result.title) - 1U] == '\0');
    assert(strlen(result.title) == sizeof(result.title) - 1U);

    /* An executable provider cannot run until both trust and consent are present. */
    umi_browser_source_preview_provider_init(&provider);
    (void)strcpy(provider.provider_id, "test.preview.executable");
    (void)strcpy(provider.title, "Executable fixture");
    (void)strcpy(provider.language_id, "c");
    provider.priority = 500U;
    provider.executes_source = 1;
    provider.requires_trusted_workspace = 1;
    provider.render = render_executable_fixture;
    provider.provider_context = &executable_calls;
    assert(umi_browser_source_preview_service_register(service, &provider) ==
           UMI_STATUS_OK);
    assert(umi_browser_source_preview_service_register(service, &provider) ==
           UMI_STATUS_ALREADY_EXISTS);

    request = request_for("main.c", "c", "return 1;", 9U);
    assert(umi_browser_source_preview_service_render(
        service, &request, &result) == UMI_STATUS_OK);
    assert(strcmp(result.provider_id, "umicom.preview.plain-text") == 0);
    assert(executable_calls == 0);

    request.workspace_trusted = 1;
    request.allow_source_execution = 1;
    assert(umi_browser_source_preview_service_render(
        service, &request, &result) == UMI_STATUS_OK);
    assert(strcmp(result.provider_id, "test.preview.executable") == 0);
    assert(result.executed_source == 1);
    assert(executable_calls == 1);

    /* Output from an earlier source revision is never accepted as current. */
    umi_browser_source_preview_provider_init(&provider);
    (void)strcpy(provider.provider_id, "test.preview.stale");
    (void)strcpy(provider.title, "Stale fixture");
    (void)strcpy(provider.extension, ".stale");
    provider.priority = 900U;
    provider.render = render_stale_fixture;
    assert(umi_browser_source_preview_service_register(service, &provider) ==
           UMI_STATUS_OK);
    request = request_for("sample.stale", "text", "current", 10U);
    assert(umi_browser_source_preview_service_render(
        service, &request, &result) == UMI_STATUS_INVALID_STATE);

    assert(umi_browser_source_preview_service_unregister(
        service, "test.preview.stale") == UMI_STATUS_OK);
    assert(umi_browser_source_preview_service_unregister(
        service, "test.preview.stale") == UMI_STATUS_NOT_FOUND);
    umi_browser_source_preview_service_destroy(service);
    return 0;
}
