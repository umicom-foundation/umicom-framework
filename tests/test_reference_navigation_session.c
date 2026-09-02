/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_reference_navigation_session.c
 *
 * PURPOSE:
 *   Implement the test reference navigation session behavior for
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
 * Umicom Framework grouped reference-navigation tests.
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

#include "umicom/editor/reference_navigation_session.h"

/*
 * Exercise emit reference and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus emit_reference(UmiEditorNavigationResultSink sink,
                                void *user_data,
                                const char *uri,
                                uint64_t line,
                                UmiEditorSourceLocationKind kind,
                                const char *label)
{
    UmiEditorNavigationResult result;
    (void)memset(&result, 0, sizeof(result));
    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    assert(umi_editor_source_location_initialize(&result.location, uri, line,
                                                 1U) == UMI_STATUS_OK);
    result.location.kind = kind;
    result.location.document_revision = 3U;
    result.location.byte_offset = line * 10U;
    result.location.end_byte_offset = result.location.byte_offset + 5U;
    COPY_TEXT(result.location.symbol_id, "ref.symbol");
    COPY_TEXT(result.location.label, label);
    COPY_TEXT(result.location.preview, label);
    COPY_TEXT(result.detail, label);
    return sink(&result, user_data);
}

/*
 * Exercise fake query and return a clear result when the behaviour no longer matches its
 * contract.
 */
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
    assert(emit_reference(sink, sink_user_data,
                          "file:///workspace/alpha.c", 2U,
                          UMI_EDITOR_SOURCE_LOCATION_DECLARATION,
                          "alpha declaration") == UMI_STATUS_OK);
    assert(emit_reference(sink, sink_user_data,
                          "file:///workspace/alpha.c", 8U,
                          UMI_EDITOR_SOURCE_LOCATION_REFERENCE,
                          "alpha use") == UMI_STATUS_OK);
    assert(emit_reference(sink, sink_user_data,
                          "file:///workspace/beta.c", 4U,
                          UMI_EDITOR_SOURCE_LOCATION_REFERENCE,
                          "beta first") == UMI_STATUS_OK);
    assert(emit_reference(sink, sink_user_data,
                          "file:///workspace/beta.c", 14U,
                          UMI_EDITOR_SOURCE_LOCATION_REFERENCE,
                          "beta second") == UMI_STATUS_OK);
    return UMI_STATUS_OK;
}

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
    int written;
    (void)instance;
    assert(!cancellation->is_cancelled(cancellation->user_data));
    (void)memset(out_preview, 0, sizeof(*out_preview));
    out_preview->struct_size = (uint32_t)sizeof(*out_preview);
    out_preview->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    COPY_TEXT(out_preview->provider_id, "reference.provider");
    COPY_TEXT(out_preview->uri, request->uri);
    COPY_TEXT(out_preview->language_id, request->language_id);
    out_preview->document_revision = request->document_revision;
    out_preview->start_line = request->start_line;
    out_preview->end_line = request->end_line;
    written = snprintf(out_preview->content, sizeof(out_preview->content),
                       "source:%s", request->uri);
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
    UmiEditorReferenceNavigationSession *session = NULL;
    UmiEditorSourceLocation origin;
    UmiEditorNavigationRequest request;
    UmiEditorReferenceNavigationSnapshot snapshot;
    UmiEditorReferenceNavigationGroup group;
    UmiEditorReferenceNavigationEntry entry;
    size_t beta_group = SIZE_MAX;
    size_t index;

    (void)memset(&registration, 0, sizeof(registration));
    registration.struct_size = (uint32_t)sizeof(registration);
    registration.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    assert(umi_editor_navigation_provider_descriptor_initialize(
               &registration.descriptor, "reference.provider", "References",
               "c", UMI_EDITOR_NAVIGATION_CAPABILITY_REFERENCES |
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
    assert(umi_editor_reference_navigation_session_create(registry, &session) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_location_initialize(
               &origin, "file:///workspace/main.c", 1U, 0U) == UMI_STATUS_OK);
    origin.document_revision = 3U;
    assert(umi_editor_navigation_request_initialize(
               &request, 99U, UMI_EDITOR_NAVIGATION_QUERY_REFERENCE,
               &origin, "c") == UMI_STATUS_OK);
    assert(umi_editor_reference_navigation_session_open(session, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_reference_navigation_session_snapshot(session,
                                                            &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.group_count == 2U);
    assert(snapshot.result_count == 4U);
    assert(snapshot.visible_result_count == 4U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot.group_count; ++index) {
        assert(umi_editor_reference_navigation_session_group_at(
                   session, index, &group) == UMI_STATUS_OK);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (strstr(group.uri, "beta.c") != NULL) beta_group = index;
    }
    assert(beta_group != SIZE_MAX);
    assert(umi_editor_reference_navigation_session_set_include_declaration(
               session, 0) == UMI_STATUS_OK);
    assert(umi_editor_reference_navigation_session_set_filter(session,
                                                              "beta") ==
           UMI_STATUS_OK);
    assert(umi_editor_reference_navigation_session_visible_count(session) ==
           2U);
    assert(umi_editor_reference_navigation_session_visible_entry_at(
               session, 0U, &entry) == UMI_STATUS_OK);
    assert(strstr(entry.result.location.uri, "beta.c") != NULL);
    assert(umi_editor_reference_navigation_session_set_group_expanded(
               session, beta_group, 0) == UMI_STATUS_OK);
    assert(umi_editor_reference_navigation_session_visible_count(session) ==
           0U);
    assert(umi_editor_reference_navigation_session_set_group_expanded(
               session, beta_group, 1) == UMI_STATUS_OK);
    assert(umi_editor_reference_navigation_session_select_next(session, 1) ==
           UMI_STATUS_OK);
    umi_editor_reference_navigation_session_destroy(session);
    umi_editor_navigation_provider_registry_destroy(registry);
    return 0;
}
