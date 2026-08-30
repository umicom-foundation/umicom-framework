/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_type_hierarchy.c
 *
 * PURPOSE:
 *   Implement the test editor type hierarchy behavior for
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
 * Umicom Framework type hierarchy tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/type_hierarchy.h"

static UmiEditorIndexedSymbol type_symbol(const char *id, uint64_t line)
{
    UmiEditorIndexedSymbol value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    (void)strcpy(value.id, id);
    (void)strcpy(value.provider_id, "language-provider");
    (void)strcpy(value.workspace_id, "workspace");
    (void)strcpy(value.document_id, "types.hpp");
    (void)strcpy(value.language_id, "cpp");
    (void)strcpy(value.name, id);
    (void)strcpy(value.qualified_name, id);
    (void)strcpy(value.detail, "indexed type");
    value.kind = UMI_EDITOR_INDEXED_SYMBOL_CLASS;
    value.flags = UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION;
    assert(umi_editor_source_location_initialize(&value.location,
                                                  "file:///types.hpp",
                                                  line,
                                                  0U) == UMI_STATUS_OK);
    value.location.end_line = line + 2U;
    value.location.end_column = 20U;
    value.selection_location = value.location;
    return value;
}

static void add_edge(UmiEditorSymbolRelationshipGraph *graph,
                     const char *id,
                     const UmiEditorIndexedSymbol *source,
                     const UmiEditorIndexedSymbol *target,
                     UmiEditorSymbolRelationshipKind kind)
{
    UmiEditorSymbolRelationship edge = {0};
    edge.struct_size = (uint32_t)sizeof(edge);
    edge.api_version = UMI_EDITOR_SYMBOL_RELATIONSHIP_API_VERSION;
    (void)strcpy(edge.id, id);
    (void)strcpy(edge.source_symbol_id, source->id);
    (void)strcpy(edge.target_symbol_id, target->id);
    (void)strcpy(edge.provider_id, "language-provider");
    edge.kind = kind;
    edge.source_location = source->selection_location;
    edge.target_location = target->selection_location;
    assert(umi_editor_symbol_relationship_graph_upsert(graph, &edge) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiEditorSymbolIndex *index = NULL;
    UmiEditorSymbolRelationshipGraph *graph = NULL;
    UmiEditorSymbolHierarchy *hierarchy = NULL;
    UmiEditorIndexedSymbol base = type_symbol("Base", 1U);
    UmiEditorIndexedSymbol derived = type_symbol("Derived", 8U);
    UmiEditorSymbolHierarchyBuildOptions options;
    UmiEditorSymbolHierarchyNode node;

    assert(umi_editor_symbol_index_create(0U, &index) == UMI_STATUS_OK);
    assert(umi_editor_symbol_relationship_graph_create(&graph) == UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_create(&hierarchy) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &base) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &derived) == UMI_STATUS_OK);
    add_edge(graph, "base", &derived, &base,
             UMI_EDITOR_SYMBOL_RELATIONSHIP_BASE_TYPE);
    add_edge(graph, "derived", &base, &derived,
             UMI_EDITOR_SYMBOL_RELATIONSHIP_DERIVED_TYPE);
    assert(umi_editor_symbol_hierarchy_build_options_initialize(&options) ==
           UMI_STATUS_OK);
    options.include_indirect = 0;
    assert(umi_editor_type_hierarchy_build(
               hierarchy, index, graph, "Derived",
               UMI_EDITOR_TYPE_HIERARCHY_SUPERTYPES, &options) ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_count(hierarchy) == 2U);
    assert(umi_editor_symbol_hierarchy_at(hierarchy, 1U, &node) ==
           UMI_STATUS_OK);
    assert(strcmp(node.symbol_id, "Base") == 0);
    assert(node.relationship_count == 2U);
    assert(umi_editor_type_hierarchy_build(
               hierarchy, index, graph, "Base",
               UMI_EDITOR_TYPE_HIERARCHY_SUBTYPES, &options) ==
           UMI_STATUS_OK);
    assert(umi_editor_symbol_hierarchy_at(hierarchy, 1U, &node) ==
           UMI_STATUS_OK);
    assert(strcmp(node.symbol_id, "Derived") == 0);
    umi_editor_symbol_hierarchy_destroy(hierarchy);
    umi_editor_symbol_relationship_graph_destroy(graph);
    umi_editor_symbol_index_destroy(index);
    return 0;
}
