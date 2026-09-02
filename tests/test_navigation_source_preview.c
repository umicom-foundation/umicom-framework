/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_navigation_source_preview.c
 *
 * PURPOSE:
 *   Implement the test navigation source preview behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework source-preview provider and cache tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define COPY_TEXT(destination, source)                                      \
    do {                                                                     \
        const char *umi_copy_source_local = (source);                        \
        size_t umi_copy_length_local = strlen(umi_copy_source_local);        \
        assert(umi_copy_length_local < sizeof(destination));                 \
        (void)memcpy((destination), umi_copy_source_local,                   \
                     umi_copy_length_local + 1U);                            \
    } while (0)

#include "umicom/editor/navigation_source_preview.h"

typedef struct FakePreviewProvider {
    size_t calls;
} FakePreviewProvider;

/*
 * Exercise fake preview and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus fake_preview(
    void *instance,
    const UmiEditorNavigationPreviewRequest *request,
    const UmiEditorNavigationCancellation *cancellation,
    UmiEditorNavigationSourcePreview *out_preview)
{
    FakePreviewProvider *provider = (FakePreviewProvider *)instance;
    int written;
    assert(!cancellation->is_cancelled(cancellation->user_data));
    ++provider->calls;
    (void)memset(out_preview, 0, sizeof(*out_preview));
    out_preview->struct_size = (uint32_t)sizeof(*out_preview);
    out_preview->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    /* Framework must replace provider-supplied provenance with registration. */
    COPY_TEXT(out_preview->provider_id, "spoofed.provider");
    COPY_TEXT(out_preview->uri, request->uri);
    COPY_TEXT(out_preview->language_id, request->language_id);
    out_preview->document_revision = 9U;
    out_preview->start_line = request->start_line;
    out_preview->end_line = request->end_line;
    written = snprintf(out_preview->content, sizeof(out_preview->content),
                       "lines %llu-%llu",
                       (unsigned long long)request->start_line,
                       (unsigned long long)request->end_line);
    assert(written > 0);
    out_preview->content_length = (size_t)written;
    out_preview->complete = 1;
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorNavigationProviderRegistry *registry = NULL;
    UmiEditorNavigationProviderRegistration registration;
    UmiEditorNavigationSourcePreviewCache *cache = NULL;
    UmiEditorNavigationPreviewRequest request;
    UmiEditorNavigationSourcePreview preview;
    UmiEditorNavigationSourcePreviewCacheSnapshot snapshot;
    FakePreviewProvider provider = {0U};
    size_t removed = 0U;

    (void)memset(&registration, 0, sizeof(registration));
    registration.struct_size = (uint32_t)sizeof(registration);
    registration.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    assert(umi_editor_navigation_provider_descriptor_initialize(
               &registration.descriptor, "preview.provider", "Preview", "c",
               UMI_EDITOR_NAVIGATION_CAPABILITY_SOURCE_PREVIEW, 10) ==
           UMI_STATUS_OK);
    registration.functions.struct_size =
        (uint32_t)sizeof(registration.functions);
    registration.functions.api_version =
        UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    registration.functions.preview = fake_preview;
    registration.instance = &provider;
    assert(umi_editor_navigation_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_registry_upsert(registry,
                                                          &registration) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_source_preview_cache_create(
               registry, NULL, &cache) == UMI_STATUS_OK);
    assert(umi_editor_navigation_preview_request_initialize(
               &request, 4U, "file:///workspace/main.c", "c", 10U, 10U,
               14U) == UMI_STATUS_OK);
    assert(umi_editor_navigation_source_preview_cache_resolve(
               cache, &request, &preview) == UMI_STATUS_OK);
    assert(provider.calls == 1U);
    assert(strcmp(preview.content, "lines 10-14") == 0);
    assert(strcmp(preview.provider_id, "preview.provider") == 0);
    assert(preview.stale);
    request.document_revision = 0U;
    assert(umi_editor_navigation_source_preview_cache_resolve(
               cache, &request, &preview) == UMI_STATUS_OK);
    assert(provider.calls == 1U);
    assert(!preview.stale);
    assert(umi_editor_navigation_source_preview_cache_snapshot(
               cache, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.hit_count == 1U);
    assert(snapshot.miss_count == 1U);
    assert(snapshot.entry_count == 1U);
    assert(umi_editor_navigation_source_preview_cache_invalidate_uri(
               cache, request.uri, &removed) == UMI_STATUS_OK);
    assert(removed == 1U);
    umi_editor_navigation_source_preview_cache_destroy(cache);
    umi_editor_navigation_provider_registry_destroy(registry);
    return 0;
}
