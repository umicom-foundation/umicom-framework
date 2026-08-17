/*-----------------------------------------------------------------------------
 * Umicom Framework document/workspace symbol navigation tests.
 * Created by: Sammy Hegab | Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#define COPY_TEXT(destination, source)                                      \
    do {                                                                     \
        const char *umi_copy_source_local = (source);                        \
        size_t umi_copy_length_local = strlen(umi_copy_source_local);        \
        assert(umi_copy_length_local < sizeof(destination));                 \
        (void)memcpy((destination), umi_copy_source_local,                   \
                     umi_copy_length_local + 1U);                            \
    } while (0)

#include "umicom/editor/symbol_navigation_session.h"

static UmiStatus emit_symbol(UmiEditorNavigationSymbolSink sink,
                             void *user_data,
                             const char *id,
                             const char *parent_id,
                             const char *name,
                             uint64_t start_line,
                             uint64_t end_line)
{
    UmiEditorIndexedSymbol symbol;
    (void)memset(&symbol, 0, sizeof(symbol));
    symbol.struct_size = (uint32_t)sizeof(symbol);
    symbol.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    COPY_TEXT(symbol.id, id);
    COPY_TEXT(symbol.parent_id, parent_id);
    COPY_TEXT(symbol.workspace_id, "workspace");
    COPY_TEXT(symbol.document_id, "main.c");
    COPY_TEXT(symbol.language_id, "c");
    COPY_TEXT(symbol.name, name);
    COPY_TEXT(symbol.qualified_name, name);
    COPY_TEXT(symbol.detail, name);
    symbol.kind = parent_id[0] == '\0'
        ? UMI_EDITOR_INDEXED_SYMBOL_STRUCTURE
        : UMI_EDITOR_INDEXED_SYMBOL_FUNCTION;
    symbol.flags = UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION;
    assert(umi_editor_source_location_initialize(
               &symbol.location, "file:///workspace/main.c", start_line, 0U) ==
           UMI_STATUS_OK);
    symbol.location.end_line = end_line;
    symbol.location.byte_offset = start_line * 10U;
    symbol.location.end_byte_offset = end_line * 10U + 9U;
    symbol.selection_location = symbol.location;
    symbol.selection_location.end_line = start_line;
    symbol.selection_location.end_byte_offset =
        symbol.selection_location.byte_offset + 4U;
    return sink(&symbol, user_data);
}

static UmiStatus fake_symbols(
    void *instance,
    const UmiEditorNavigationRequest *request,
    const UmiEditorNavigationCancellation *cancellation,
    UmiEditorNavigationSymbolSink sink,
    void *sink_user_data,
    UmiEditorNavigationProviderReport *out_report)
{
    (void)instance;
    (void)request;
    (void)out_report;
    assert(!cancellation->is_cancelled(cancellation->user_data));
    assert(emit_symbol(sink, sink_user_data, "type.root", "", "RootType",
                       0U, 30U) == UMI_STATUS_OK);
    assert(emit_symbol(sink, sink_user_data, "method.work", "type.root",
                       "work_item", 4U, 8U) == UMI_STATUS_OK);
    assert(emit_symbol(sink, sink_user_data, "function.helper", "",
                       "helper_function", 40U, 44U) == UMI_STATUS_OK);
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiEditorNavigationProviderRegistry *registry = NULL;
    UmiEditorNavigationProviderRegistration registration;
    UmiEditorSymbolNavigationSession *session = NULL;
    UmiEditorSourceLocation origin;
    UmiEditorSourceLocation cursor;
    UmiEditorNavigationRequest request;
    UmiEditorSymbolNavigationSnapshot snapshot;
    UmiEditorSymbolNavigationEntry selected;
    UmiEditorSymbolNavigationEntry path;

    (void)memset(&registration, 0, sizeof(registration));
    registration.struct_size = (uint32_t)sizeof(registration);
    registration.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    assert(umi_editor_navigation_provider_descriptor_initialize(
               &registration.descriptor, "symbol.provider", "Symbols", "c",
               UMI_EDITOR_NAVIGATION_CAPABILITY_DOCUMENT_SYMBOLS |
                   UMI_EDITOR_NAVIGATION_CAPABILITY_WORKSPACE_SYMBOLS,
               10) == UMI_STATUS_OK);
    registration.functions.struct_size =
        (uint32_t)sizeof(registration.functions);
    registration.functions.api_version =
        UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    registration.functions.symbols = fake_symbols;
    assert(umi_editor_navigation_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_registry_upsert(registry,
                                                          &registration) ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_navigation_session_create(registry, &session) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_location_initialize(
               &origin, "file:///workspace/main.c", 0U, 0U) == UMI_STATUS_OK);
    assert(umi_editor_navigation_request_initialize(
               &request, 200U, UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL,
               &origin, "c") == UMI_STATUS_OK);
    COPY_TEXT(request.document_id, "main.c");
    assert(umi_editor_symbol_navigation_session_open(
               session, UMI_EDITOR_SYMBOL_NAVIGATION_DOCUMENT, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_navigation_session_snapshot(session, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.symbol_count == 3U);
    cursor = origin;
    cursor.line = 5U;
    cursor.end_line = 5U;
    cursor.byte_offset = 55U;
    cursor.end_byte_offset = 55U;
    assert(umi_editor_symbol_navigation_session_select_enclosing(
               session, &cursor) == UMI_STATUS_OK);
    assert(umi_editor_symbol_navigation_session_selected(session, &selected) ==
           UMI_STATUS_OK);
    assert(strcmp(selected.symbol.id, "method.work") == 0);
    assert(umi_editor_symbol_navigation_session_selected_path_count(session) ==
           2U);
    assert(umi_editor_symbol_navigation_session_selected_path_at(
               session, 0U, &path) == UMI_STATUS_OK);
    assert(strcmp(path.symbol.id, "type.root") == 0);
    assert(umi_editor_symbol_navigation_session_set_filter(session,
                                                           "helper") ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_navigation_session_visible_count(session) == 1U);
    assert(umi_editor_symbol_navigation_session_refresh(session) ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_navigation_session_snapshot(session, &snapshot) ==
           UMI_STATUS_OK);
    assert(strcmp(snapshot.filter, "helper") == 0);
    assert(snapshot.visible_symbol_count == 1U);
    assert(umi_editor_symbol_navigation_session_set_filter(session, "") ==
           UMI_STATUS_OK);
    cursor.line = 8U;
    cursor.end_line = 8U;
    cursor.byte_offset = 89U;
    cursor.end_byte_offset = 89U;
    assert(umi_editor_symbol_navigation_session_select_enclosing(
               session, &cursor) == UMI_STATUS_OK);
    assert(umi_editor_symbol_navigation_session_selected(session, &selected) ==
           UMI_STATUS_OK);
    assert(strcmp(selected.symbol.id, "type.root") == 0);
    umi_editor_symbol_navigation_session_destroy(session);
    umi_editor_navigation_provider_registry_destroy(registry);
    return 0;
}
