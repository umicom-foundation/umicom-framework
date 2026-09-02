/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_rename_symbol.c
 *
 * PURPOSE:
 *   Implement the test editor rename symbol behavior for
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
 * Umicom Framework
 * File: tests/editor/test_editor_rename_symbol.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/rename_symbol.h"

/*
 * Exercise set location and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void set_location(UmiEditorSourceLocation *location,
                         uint64_t start,
                         uint64_t end,
                         uint64_t revision,
                         UmiEditorSourceLocationKind kind)
{
    assert(umi_editor_source_location_initialize(
               location, "file:///workspace/main.c", 0U, start) ==
           UMI_STATUS_OK);
    location->kind = kind;
    location->byte_offset = start;
    location->end_byte_offset = end;
    location->end_column = end;
    location->document_revision = revision;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    static const char SOURCE[] = "int count = count + 1;";
    UmiEditorTextBuffer *buffer = NULL;
    UmiEditorTextBufferView view;
    UmiEditorSymbolIndex *symbols = NULL;
    UmiEditorIndexedSymbol symbol = {0};
    UmiEditorNavigationResultSet *references = NULL;
    UmiEditorNavigationResult reference = {0};
    UmiEditorRenameSymbolPlan *plan = NULL;
    UmiEditorRenameSymbolRequest request = {0};
    UmiEditorRenameSymbolSnapshot snapshot;
    size_t applied = 0U;

    assert(umi_editor_rename_symbol_name_validate("valid_name", 0, 0) ==
           UMI_STATUS_OK);
    assert(umi_editor_rename_symbol_name_validate("9invalid", 0, 0) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_editor_text_buffer_create(0U, &buffer) == UMI_STATUS_OK);
    assert(umi_editor_text_buffer_set(buffer, SOURCE, sizeof(SOURCE) - 1U) ==
           UMI_STATUS_OK);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_create(0U, &symbols) == UMI_STATUS_OK);

    symbol.struct_size = (uint32_t)sizeof(symbol);
    symbol.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    (void)strcpy(symbol.id, "symbol-count");
    (void)strcpy(symbol.provider_id, "native-c");
    (void)strcpy(symbol.workspace_id, "workspace");
    (void)strcpy(symbol.document_id, "main.c");
    (void)strcpy(symbol.language_id, "c");
    (void)strcpy(symbol.name, "count");
    (void)strcpy(symbol.qualified_name, "count");
    symbol.kind = UMI_EDITOR_INDEXED_SYMBOL_VARIABLE;
    symbol.flags = UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION;
    set_location(&symbol.location, 4U, 9U, view.revision,
                 UMI_EDITOR_SOURCE_LOCATION_DEFINITION);
    symbol.selection_location = symbol.location;
    assert(umi_editor_symbol_index_upsert(symbols, &symbol) == UMI_STATUS_OK);

    assert(umi_editor_navigation_result_set_create(
               UMI_EDITOR_NAVIGATION_QUERY_REFERENCE, &references) ==
           UMI_STATUS_OK);
    reference.struct_size = (uint32_t)sizeof(reference);
    reference.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    (void)strcpy(reference.provider_id, "native-c");
    set_location(&reference.location, 12U, 17U, view.revision,
                 UMI_EDITOR_SOURCE_LOCATION_REFERENCE);
    assert(umi_editor_navigation_result_set_upsert(references, &reference) ==
           UMI_STATUS_OK);

    assert(umi_editor_rename_symbol_plan_create(&plan) == UMI_STATUS_OK);
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_EDITOR_RENAME_SYMBOL_API_VERSION;
    request.symbol_id = "symbol-count";
    request.new_name = "total";
    assert(umi_editor_rename_symbol_plan_prepare(
               plan, symbols, references, &request) == UMI_STATUS_OK);
    assert(umi_editor_rename_symbol_plan_snapshot(plan, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.edit_count == 2U);
    assert(snapshot.document_count == 1U);
    assert(snapshot.can_apply);
    assert(umi_editor_rename_symbol_plan_apply_document(
               plan, "file:///workspace/main.c", buffer, 1, &applied) ==
           UMI_STATUS_OK);
    assert(applied == 2U);
    assert(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    assert(strcmp(view.bytes, "int total = total + 1;") == 0);
    assert(umi_editor_rename_symbol_plan_snapshot(plan, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_EDITOR_RENAME_SYMBOL_APPLIED);

    umi_editor_rename_symbol_plan_destroy(plan);
    umi_editor_navigation_result_set_destroy(references);
    umi_editor_symbol_index_destroy(symbols);
    umi_editor_text_buffer_destroy(buffer);
    return 0;
}
