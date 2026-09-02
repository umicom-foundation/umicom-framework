/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_source_navigation_service.c
 *
 * PURPOSE:
 *   Implement the test editor source navigation service behavior for
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
 * Umicom Framework source navigation coordinator tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/source_navigation.h"

/*
 * Exercise navigation symbol and return a clear result when the behaviour no longer
 * matches its contract.
 */
static UmiEditorIndexedSymbol navigation_symbol(const char *id,
                                                 const char *name,
                                                 uint64_t line,
                                                 uint32_t flags)
{
    UmiEditorIndexedSymbol value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    (void)strcpy(value.id, id);
    (void)strcpy(value.provider_id, "native-c-index");
    (void)strcpy(value.workspace_id, "workspace");
    (void)strcpy(value.document_id, "main.c");
    (void)strcpy(value.language_id, "c");
    (void)strcpy(value.name, name);
    (void)strcpy(value.qualified_name, name);
    (void)strcpy(value.detail, "navigation symbol");
    value.kind = UMI_EDITOR_INDEXED_SYMBOL_FUNCTION;
    value.flags = flags;
    assert(umi_editor_source_location_initialize(&value.location,
                                                  "file:///project/main.c",
                                                  line,
                                                  0U) == UMI_STATUS_OK);
    value.location.end_line = line + 2U;
    value.location.end_column = 40U;
    value.selection_location = value.location;
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorSourceNavigation *navigation = NULL;
    UmiEditorNavigationResultSet *results = NULL;
    UmiEditorIndexedSymbol use = navigation_symbol("use", "use_value", 2U,
                                                    0U);
    UmiEditorIndexedSymbol definition = navigation_symbol(
        "definition", "value", 12U,
        UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION);
    UmiEditorSymbolRelationship edge = {0};
    UmiEditorNavigationResult result;
    UmiEditorSourceLocation opened;
    UmiEditorSourceLocation current;
    UmiEditorSourceNavigationSnapshot snapshot;

    assert(umi_editor_source_navigation_create(&navigation) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_create(
               UMI_EDITOR_NAVIGATION_QUERY_DEFINITION, &results) ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(
               umi_editor_source_navigation_symbols(navigation), &use) ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(
               umi_editor_source_navigation_symbols(navigation), &definition) ==
           UMI_STATUS_OK);
    edge.struct_size = (uint32_t)sizeof(edge);
    edge.api_version = UMI_EDITOR_SYMBOL_RELATIONSHIP_API_VERSION;
    (void)strcpy(edge.id, "use-definition");
    (void)strcpy(edge.source_symbol_id, "use");
    (void)strcpy(edge.target_symbol_id, "definition");
    (void)strcpy(edge.provider_id, "native-c-index");
    edge.kind = UMI_EDITOR_SYMBOL_RELATIONSHIP_DEFINITION;
    edge.source_location = use.selection_location;
    edge.target_location = definition.selection_location;
    edge.rank = 100;
    assert(umi_editor_symbol_relationship_graph_upsert(
               umi_editor_source_navigation_relationships(navigation), &edge) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_navigation_resolve(
               navigation, "use", UMI_EDITOR_NAVIGATION_QUERY_DEFINITION,
               results) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_count(results) == 1U);
    assert(umi_editor_navigation_result_set_selected(results, &result) ==
           UMI_STATUS_OK);
    assert(strcmp(result.location.symbol_id, "definition") == 0);
    assert(umi_editor_source_navigation_open(navigation,
                                              &use.selection_location,
                                              &result.location,
                                              &opened) == UMI_STATUS_OK);
    assert(umi_editor_source_location_same_position(&opened, &result.location));
    assert(umi_editor_source_navigation_go_back(navigation, &current) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_location_same_position(&current,
                                                     &use.selection_location));
    assert(umi_editor_source_navigation_go_forward(navigation, &current) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_navigation_snapshot(navigation, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.symbol_count == 2U && snapshot.relationship_count == 1U);
    assert(snapshot.history_count == 2U && snapshot.can_go_back);
    umi_editor_navigation_result_set_destroy(results);
    umi_editor_source_navigation_destroy(navigation);
    return 0;
}
