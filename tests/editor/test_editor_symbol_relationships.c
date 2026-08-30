/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_symbol_relationships.c
 *
 * PURPOSE:
 *   Implement the test editor symbol relationships behavior for
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
 * Umicom Framework symbol relationship graph tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/symbol_relationship.h"

static UmiEditorIndexedSymbol indexed_symbol(const char *id,
                                              const char *name,
                                              uint64_t line)
{
    UmiEditorIndexedSymbol value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    (void)strcpy(value.id, id);
    (void)strcpy(value.provider_id, "compiler");
    (void)strcpy(value.workspace_id, "workspace");
    (void)strcpy(value.document_id, "main.c");
    (void)strcpy(value.language_id, "c");
    (void)strcpy(value.name, name);
    (void)strcpy(value.qualified_name, name);
    (void)strcpy(value.detail, "relationship target");
    value.kind = UMI_EDITOR_INDEXED_SYMBOL_FUNCTION;
    value.flags = UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION;
    assert(umi_editor_source_location_initialize(&value.location,
                                                  "file:///project/main.c",
                                                  line,
                                                  0U) == UMI_STATUS_OK);
    value.location.end_line = line + 2U;
    value.location.end_column = 20U;
    value.selection_location = value.location;
    return value;
}

int main(void)
{
    UmiEditorSymbolIndex *index = NULL;
    UmiEditorSymbolRelationshipGraph *graph = NULL;
    UmiEditorNavigationResultSet *results = NULL;
    UmiEditorIndexedSymbol caller = indexed_symbol("caller", "caller", 2U);
    UmiEditorIndexedSymbol callee = indexed_symbol("callee", "callee", 10U);
    UmiEditorSymbolRelationship edge = {0};
    UmiEditorNavigationResult result;
    size_t removed = 0U;

    assert(umi_editor_symbol_index_create(0U, &index) == UMI_STATUS_OK);
    assert(umi_editor_symbol_relationship_graph_create(&graph) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_create(
               UMI_EDITOR_NAVIGATION_QUERY_REFERENCE, &results) ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &caller) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &callee) == UMI_STATUS_OK);
    edge.struct_size = (uint32_t)sizeof(edge);
    edge.api_version = UMI_EDITOR_SYMBOL_RELATIONSHIP_API_VERSION;
    (void)strcpy(edge.id, "caller-to-callee");
    (void)strcpy(edge.source_symbol_id, "caller");
    (void)strcpy(edge.target_symbol_id, "callee");
    (void)strcpy(edge.provider_id, "compiler");
    edge.kind = UMI_EDITOR_SYMBOL_RELATIONSHIP_REFERENCE;
    edge.source_location = caller.selection_location;
    edge.target_location = callee.selection_location;
    edge.rank = 80;
    assert(umi_editor_symbol_relationship_graph_upsert(graph, &edge) ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_relationship_graph_collect(
               graph, index, "caller", UMI_EDITOR_SYMBOL_RELATIONSHIP_REFERENCE,
               0, results) == UMI_STATUS_OK);
    assert(umi_editor_navigation_result_set_count(results) == 1U);
    assert(umi_editor_navigation_result_set_at(results, 0U, &result) ==
           UMI_STATUS_OK);
    assert(strcmp(result.location.symbol_id, "callee") == 0);
    assert(result.location.kind == UMI_EDITOR_SOURCE_LOCATION_REFERENCE);
    assert(umi_editor_symbol_relationship_graph_remove_symbol(
               graph, "callee", &removed) == UMI_STATUS_OK);
    assert(removed == 1U &&
           umi_editor_symbol_relationship_graph_count(graph) == 0U);
    umi_editor_navigation_result_set_destroy(results);
    umi_editor_symbol_relationship_graph_destroy(graph);
    umi_editor_symbol_index_destroy(index);
    return 0;
}
