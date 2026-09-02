/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_navigation_experience.c
 *
 * PURPOSE:
 *   Implement the test navigation experience behavior for
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
 * Umicom Framework complete navigation-experience orchestration tests.
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

#include "umicom/editor/navigation_experience.h"

/*
 * Exercise provider query and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus provider_query(
    void *instance,
    const UmiEditorNavigationRequest *request,
    const UmiEditorNavigationCancellation *cancellation,
    UmiEditorNavigationResultSink sink,
    void *sink_user_data,
    UmiEditorNavigationProviderReport *out_report)
{
    UmiEditorNavigationResult result;
    (void)instance;
    (void)out_report;
    assert(!cancellation->is_cancelled(cancellation->user_data));
    (void)memset(&result, 0, sizeof(result));
    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    assert(umi_editor_source_location_initialize(
               &result.location,
               request->query_kind == UMI_EDITOR_NAVIGATION_QUERY_REFERENCE
                   ? "file:///workspace/reference.c"
                   : "file:///workspace/definition.c",
               request->query_kind == UMI_EDITOR_NAVIGATION_QUERY_REFERENCE
                   ? 12U
                   : 4U,
               0U) == UMI_STATUS_OK);
    result.location.kind =
        request->query_kind == UMI_EDITOR_NAVIGATION_QUERY_REFERENCE
            ? UMI_EDITOR_SOURCE_LOCATION_REFERENCE
            : UMI_EDITOR_SOURCE_LOCATION_DEFINITION;
    result.location.document_revision = request->document_revision;
    result.location.byte_offset = result.location.line * 10U;
    result.location.end_byte_offset = result.location.byte_offset + 4U;
    COPY_TEXT(result.location.symbol_id, "experience.symbol");
    COPY_TEXT(result.location.label, "experience_symbol");
    result.primary = 1;
    return sink(&result, sink_user_data);
}

/*
 * Exercise provider symbols and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus provider_symbols(
    void *instance,
    const UmiEditorNavigationRequest *request,
    const UmiEditorNavigationCancellation *cancellation,
    UmiEditorNavigationSymbolSink sink,
    void *sink_user_data,
    UmiEditorNavigationProviderReport *out_report)
{
    UmiEditorIndexedSymbol symbol;
    (void)instance;
    (void)request;
    (void)out_report;
    assert(!cancellation->is_cancelled(cancellation->user_data));
    (void)memset(&symbol, 0, sizeof(symbol));
    symbol.struct_size = (uint32_t)sizeof(symbol);
    symbol.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    COPY_TEXT(symbol.id, "experience.symbol");
    COPY_TEXT(symbol.document_id, "main.c");
    COPY_TEXT(symbol.language_id, "c");
    COPY_TEXT(symbol.name, "experience_symbol");
    COPY_TEXT(symbol.qualified_name, "experience_symbol");
    symbol.kind = UMI_EDITOR_INDEXED_SYMBOL_FUNCTION;
    symbol.flags = UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION;
    assert(umi_editor_source_location_initialize(
               &symbol.location, "file:///workspace/main.c", 3U, 0U) ==
           UMI_STATUS_OK);
    symbol.location.end_line = 7U;
    symbol.location.byte_offset = 30U;
    symbol.location.end_byte_offset = 79U;
    symbol.selection_location = symbol.location;
    return sink(&symbol, sink_user_data);
}

/*
 * Exercise provider hierarchy and return a clear result when the behaviour no longer
 * matches its contract.
 */
static UmiStatus provider_hierarchy(
    void *instance,
    const UmiEditorNavigationHierarchyRequest *request,
    const UmiEditorNavigationCancellation *cancellation,
    UmiEditorNavigationHierarchySink sink,
    void *sink_user_data,
    UmiEditorNavigationProviderReport *out_report)
{
    UmiEditorNavigationHierarchyNode node;
    (void)instance;
    (void)request;
    (void)out_report;
    assert(!cancellation->is_cancelled(cancellation->user_data));
    (void)memset(&node, 0, sizeof(node));
    node.struct_size = (uint32_t)sizeof(node);
    node.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    COPY_TEXT(node.node_id, "experience.root");
    COPY_TEXT(node.symbol_id, "experience.symbol");
    COPY_TEXT(node.label, "experience_symbol");
    assert(umi_editor_source_location_initialize(
               &node.location, "file:///workspace/main.c", 3U, 0U) ==
           UMI_STATUS_OK);
    return sink(&node, sink_user_data);
}

/*
 * Exercise provider preview and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus provider_preview(
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
    COPY_TEXT(out_preview->provider_id, "experience.provider");
    COPY_TEXT(out_preview->uri, request->uri);
    COPY_TEXT(out_preview->language_id, request->language_id);
    out_preview->document_revision = request->document_revision;
    out_preview->start_line = request->start_line;
    out_preview->end_line = request->end_line;
    written = snprintf(out_preview->content, sizeof(out_preview->content),
                       "experience:%s", request->uri);
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
    UmiEditorNavigationExperience *experience = NULL;
    UmiEditorNavigationProviderRegistration registration;
    UmiEditorSourceLocation origin;
    UmiEditorNavigationRequest request;
    UmiEditorNavigationHierarchyRequest hierarchy_request;
    UmiEditorNavigationResolution resolution;
    UmiEditorNavigationExperienceSnapshot snapshot;
    UmiEditorSourceLocation selected;

    assert(umi_editor_navigation_experience_create(&experience) ==
           UMI_STATUS_OK);
    (void)memset(&registration, 0, sizeof(registration));
    registration.struct_size = (uint32_t)sizeof(registration);
    registration.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    assert(umi_editor_navigation_provider_descriptor_initialize(
               &registration.descriptor, "experience.provider", "Experience",
               "c", UMI_EDITOR_NAVIGATION_CAPABILITY_DEFINITION |
                        UMI_EDITOR_NAVIGATION_CAPABILITY_REFERENCES |
                        UMI_EDITOR_NAVIGATION_CAPABILITY_DOCUMENT_SYMBOLS |
                        UMI_EDITOR_NAVIGATION_CAPABILITY_CALL_HIERARCHY |
                        UMI_EDITOR_NAVIGATION_CAPABILITY_SOURCE_PREVIEW,
               20) == UMI_STATUS_OK);
    registration.functions.struct_size =
        (uint32_t)sizeof(registration.functions);
    registration.functions.api_version =
        UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    registration.functions.query = provider_query;
    registration.functions.symbols = provider_symbols;
    registration.functions.hierarchy = provider_hierarchy;
    registration.functions.preview = provider_preview;
    assert(umi_editor_navigation_experience_register_provider(
               experience, &registration) == UMI_STATUS_OK);

    assert(umi_editor_source_location_initialize(
               &origin, "file:///workspace/main.c", 1U, 0U) == UMI_STATUS_OK);
    origin.document_revision = 11U;
    assert(umi_editor_navigation_request_initialize(
               &request, 300U, UMI_EDITOR_NAVIGATION_QUERY_DEFINITION,
               &origin, "c") == UMI_STATUS_OK);
    assert(umi_editor_navigation_experience_resolve(
               experience, &request, &resolution) == UMI_STATUS_OK);
    assert(resolution.state == UMI_EDITOR_NAVIGATION_RESOLUTION_DIRECT);
    assert(resolution.has_target);
    assert(umi_editor_navigation_experience_selected_target(
               experience, &selected) == UMI_STATUS_OK);
    assert(strstr(selected.uri, "definition.c") != NULL);
    assert(umi_editor_navigation_experience_refresh(experience) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_experience_selected_target(
               experience, &selected) == UMI_STATUS_OK);

    assert(umi_editor_navigation_experience_open_peek(experience, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_experience_active_surface(experience) ==
           UMI_EDITOR_NAVIGATION_SURFACE_PEEK);

    request.query_kind = UMI_EDITOR_NAVIGATION_QUERY_REFERENCE;
    request.request_id = 301U;
    assert(umi_editor_navigation_experience_open_references(
               experience, &request) == UMI_STATUS_OK);
    assert(umi_editor_navigation_experience_active_surface(experience) ==
           UMI_EDITOR_NAVIGATION_SURFACE_REFERENCES);

    assert(umi_editor_navigation_hierarchy_request_initialize(
               &hierarchy_request, 302U,
               UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING,
               "experience.symbol", "c") == UMI_STATUS_OK);
    hierarchy_request.kind = (UmiEditorNavigationHierarchyKind)99;
    assert(umi_editor_navigation_experience_open_hierarchy(
               experience, &hierarchy_request) == UMI_STATUS_INVALID_ARGUMENT);
    hierarchy_request.kind = UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING;
    assert(umi_editor_navigation_experience_open_hierarchy(
               experience, &hierarchy_request) == UMI_STATUS_OK);
    assert(umi_editor_navigation_experience_active_surface(experience) ==
           UMI_EDITOR_NAVIGATION_SURFACE_CALL_HIERARCHY);

    assert(umi_editor_navigation_request_initialize(
               &request, 303U, UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL,
               &origin, "c") == UMI_STATUS_OK);
    COPY_TEXT(request.document_id, "main.c");
    assert(umi_editor_navigation_experience_open_symbols(
               experience, UMI_EDITOR_SYMBOL_NAVIGATION_DOCUMENT,
               &request) == UMI_STATUS_OK);
    assert(umi_editor_navigation_experience_snapshot(experience, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.provider_count == 1U);
    assert(snapshot.active_item_count == 1U);
    assert(snapshot.has_selection);
    assert(umi_editor_navigation_experience_close_active(experience, 1) ==
           UMI_STATUS_OK);
    umi_editor_navigation_experience_destroy(experience);
    return 0;
}
