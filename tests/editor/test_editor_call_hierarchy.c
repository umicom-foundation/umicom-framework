/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_call_hierarchy.c
 *
 * PURPOSE:
 *   Implement the test editor call hierarchy behavior for
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
 * Umicom Framework call hierarchy tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/call_hierarchy.h"

/*
 * Exercise symbol and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorIndexedSymbol symbol(const char *id, uint64_t line)
{
    UmiEditorIndexedSymbol value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    (void)strcpy(value.id, id);
    (void)strcpy(value.provider_id, "compiler");
    (void)strcpy(value.workspace_id, "workspace");
    (void)strcpy(value.document_id, "calls.c");
    (void)strcpy(value.language_id, "c");
    (void)strcpy(value.name, id);
    (void)strcpy(value.qualified_name, id);
    (void)strcpy(value.detail, "callable symbol");
    value.kind = UMI_EDITOR_INDEXED_SYMBOL_FUNCTION;
    value.flags = UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION;
    assert(umi_editor_source_location_initialize(&value.location,
                                                  "file:///calls.c",
                                                  line,
                                                  0U) == UMI_STATUS_OK);
    value.location.end_line = line + 1U;
    value.location.end_column = 20U;
    value.selection_location = value.location;
    return value;
}

/*
 * Exercise add edge and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void add_edge(UmiEditorSymbolRelationshipGraph *graph,
                     const char *id,
                     const char *source_id,
                     const char *target_id,
                     UmiEditorSymbolRelationshipKind kind,
                     const UmiEditorIndexedSymbol *source,
                     const UmiEditorIndexedSymbol *target)
{
    UmiEditorSymbolRelationship edge = {0};
    edge.struct_size = (uint32_t)sizeof(edge);
    edge.api_version = UMI_EDITOR_SYMBOL_RELATIONSHIP_API_VERSION;
    (void)strcpy(edge.id, id);
    (void)strcpy(edge.source_symbol_id, source_id);
    (void)strcpy(edge.target_symbol_id, target_id);
    (void)strcpy(edge.provider_id, "compiler");
    edge.kind = kind;
    edge.source_location = source->selection_location;
    edge.target_location = target->selection_location;
    assert(umi_editor_symbol_relationship_graph_upsert(graph, &edge) ==
           UMI_STATUS_OK);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorSymbolIndex *index = NULL;
    UmiEditorSymbolRelationshipGraph *graph = NULL;
    UmiEditorSymbolHierarchy *hierarchy = NULL;
    UmiEditorIndexedSymbol root = symbol("root", 1U);
    UmiEditorIndexedSymbol child = symbol("child", 5U);
    UmiEditorIndexedSymbol leaf = symbol("leaf", 9U);
    UmiEditorSymbolHierarchyNode node;
    UmiEditorSymbolHierarchySnapshot snapshot;

    assert(umi_editor_symbol_index_create(0U, &index) == UMI_STATUS_OK);
    assert(umi_editor_symbol_relationship_graph_create(&graph) == UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_create(&hierarchy) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &root) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &child) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &leaf) == UMI_STATUS_OK);
    add_edge(graph, "callee-1", "root", "child",
             UMI_EDITOR_SYMBOL_RELATIONSHIP_CALLEE, &root, &child);
    add_edge(graph, "caller-1", "child", "root",
             UMI_EDITOR_SYMBOL_RELATIONSHIP_CALLER, &child, &root);
    add_edge(graph, "callee-2", "child", "leaf",
             UMI_EDITOR_SYMBOL_RELATIONSHIP_CALLEE, &child, &leaf);
    add_edge(graph, "callee-cycle", "leaf", "root",
             UMI_EDITOR_SYMBOL_RELATIONSHIP_CALLEE, &leaf, &root);
    assert(umi_editor_call_hierarchy_build(
               hierarchy, index, graph, "root",
               UMI_EDITOR_CALL_HIERARCHY_OUTGOING, NULL) == UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_count(hierarchy) == 4U);
    assert(umi_editor_symbol_hierarchy_at(hierarchy, 1U, &node) ==
           UMI_STATUS_OK);
    assert(strcmp(node.symbol_id, "child") == 0);
    assert(node.relationship_count == 2U);
    assert(umi_editor_symbol_hierarchy_snapshot(hierarchy, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.cycle_count == 1U && !snapshot.truncated);
    assert(umi_editor_call_hierarchy_build(
               hierarchy, index, graph, "root",
               UMI_EDITOR_CALL_HIERARCHY_INCOMING, NULL) == UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_count(hierarchy) >= 2U);
    umi_editor_symbol_hierarchy_destroy(hierarchy);
    umi_editor_symbol_relationship_graph_destroy(graph);
    umi_editor_symbol_index_destroy(index);
    return 0;
}
