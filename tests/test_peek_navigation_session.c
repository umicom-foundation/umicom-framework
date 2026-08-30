/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_peek_navigation_session.c
 *
 * PURPOSE:
 *   Implement the test peek navigation session behavior for
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
 * Umicom Framework professional peek-session tests.
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

#include "umicom/editor/peek_navigation_session.h"

static UmiStatus emit_definition(UmiEditorNavigationResultSink sink,
                                 void *user_data,
                                 const char *uri,
                                 uint64_t line)
{
    UmiEditorNavigationResult result;
    (void)memset(&result, 0, sizeof(result));
    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    assert(umi_editor_source_location_initialize(&result.location, uri, line,
                                                 0U) == UMI_STATUS_OK);
    result.location.kind = UMI_EDITOR_SOURCE_LOCATION_DEFINITION;
    result.location.document_revision = 5U;
    result.location.byte_offset = line * 8U;
    result.location.end_byte_offset = result.location.byte_offset + 3U;
    COPY_TEXT(result.location.symbol_id, "peek.symbol");
    COPY_TEXT(result.location.label, "peek_symbol");
    return sink(&result, user_data);
}

static UmiStatus fake_query(
    void *instance,
    const UmiEditorNavigationRequest *request,
    const UmiEditorNavigationCancellation *cancellation,
    UmiEditorNavigationResultSink sink,
    void *sink_user_data,
    UmiEditorNavigationProviderReport *out_report)
{
    (void)instance;
    (void)request;
    (void)out_report;
    assert(!cancellation->is_cancelled(cancellation->user_data));
    assert(emit_definition(sink, sink_user_data,
                           "file:///workspace/first.c", 10U) == UMI_STATUS_OK);
    assert(emit_definition(sink, sink_user_data,
                           "file:///workspace/second.c", 20U) == UMI_STATUS_OK);
    return UMI_STATUS_OK;
}

static UmiStatus fake_preview(
    void *instance,
    const UmiEditorNavigationPreviewRequest *request,
    const UmiEditorNavigationCancellation *cancellation,
    UmiEditorNavigationSourcePreview *out_preview)
{
    int written;
    (void)instance;
    assert(!cancellation->is_cancelled(cancellation->user_data));
    (void)memset(out_preview, 0, sizeof(*out_preview));
    out_preview->struct_size = (uint32_t)sizeof(*out_preview);
    out_preview->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    COPY_TEXT(out_preview->provider_id, "peek.provider");
    COPY_TEXT(out_preview->uri, request->uri);
    COPY_TEXT(out_preview->language_id, request->language_id);
    out_preview->document_revision = request->document_revision;
    out_preview->start_line = request->start_line;
    out_preview->end_line = request->end_line;
    written = snprintf(out_preview->content, sizeof(out_preview->content),
                       "preview:%s", request->uri);
    assert(written > 0);
    out_preview->content_length = (size_t)written;
    out_preview->complete = 1;
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiEditorNavigationProviderRegistry *registry = NULL;
    UmiEditorNavigationProviderRegistration registration;
    UmiEditorPeekNavigationSession *session = NULL;
    UmiEditorSourceLocation origin;
    UmiEditorNavigationRequest request;
    UmiEditorPeekNavigationSnapshot snapshot;
    UmiEditorNavigationResult result;
    UmiEditorNavigationSourcePreview preview;

    (void)memset(&registration, 0, sizeof(registration));
    registration.struct_size = (uint32_t)sizeof(registration);
    registration.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    assert(umi_editor_navigation_provider_descriptor_initialize(
               &registration.descriptor, "peek.provider", "Peek", "c",
               UMI_EDITOR_NAVIGATION_CAPABILITY_DEFINITION |
                   UMI_EDITOR_NAVIGATION_CAPABILITY_SOURCE_PREVIEW,
               10) == UMI_STATUS_OK);
    registration.functions.struct_size =
        (uint32_t)sizeof(registration.functions);
    registration.functions.api_version =
        UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    registration.functions.query = fake_query;
    registration.functions.preview = fake_preview;
    assert(umi_editor_navigation_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_registry_upsert(registry,
                                                          &registration) ==
           UMI_STATUS_OK);
    assert(umi_editor_peek_navigation_session_create(registry, &session) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_location_initialize(
               &origin, "file:///workspace/main.c", 1U, 0U) == UMI_STATUS_OK);
    origin.document_revision = 5U;
    assert(umi_editor_navigation_request_initialize(
               &request, 88U, UMI_EDITOR_NAVIGATION_QUERY_DEFINITION,
               &origin, "c") == UMI_STATUS_OK);
    assert(umi_editor_peek_navigation_session_open(session, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_peek_navigation_session_snapshot(session, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.visible && snapshot.result_count == 2U);
    assert(snapshot.has_preview);
    assert(umi_editor_peek_navigation_session_active_result(session, &result) ==
           UMI_STATUS_OK);
    assert(strcmp(result.location.uri, "file:///workspace/first.c") == 0);
    assert(umi_editor_peek_navigation_session_select_next(session, 0) ==
           UMI_STATUS_OK);
    assert(umi_editor_peek_navigation_session_active_preview(session, &preview) ==
           UMI_STATUS_OK);
    assert(strstr(preview.content, "second.c") != NULL);
    assert(umi_editor_peek_navigation_session_set_pinned(session, 1) ==
           UMI_STATUS_OK);
    assert(umi_editor_peek_navigation_session_close(session, 0) ==
           UMI_STATUS_BUSY);
    assert(umi_editor_peek_navigation_session_close(session, 1) ==
           UMI_STATUS_OK);
    umi_editor_peek_navigation_session_destroy(session);
    umi_editor_navigation_provider_registry_destroy(registry);
    return 0;
}
