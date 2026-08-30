/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/type_hierarchy.c
 *
 * PURPOSE:
 *   Translate supertype and subtype semantics into shared Framework hierarchy
 *   rules for C, C++, Python, Java and future language providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/type_hierarchy.h"

UmiStatus umi_editor_type_hierarchy_build(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    UmiEditorTypeHierarchyDirection direction,
    const UmiEditorSymbolHierarchyBuildOptions *options)
{
    UmiEditorSymbolHierarchyRule rules[2] = {0};
    UmiEditorSymbolHierarchyNodeKind node_kind;

    if (direction != UMI_EDITOR_TYPE_HIERARCHY_SUPERTYPES &&
        direction != UMI_EDITOR_TYPE_HIERARCHY_SUBTYPES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node_kind = direction == UMI_EDITOR_TYPE_HIERARCHY_SUPERTYPES
        ? UMI_EDITOR_SYMBOL_HIERARCHY_SUPERTYPE
        : UMI_EDITOR_SYMBOL_HIERARCHY_SUBTYPE;

    rules[0].struct_size = (uint32_t)sizeof(rules[0]);
    rules[0].api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
    rules[0].relationship_kind =
        direction == UMI_EDITOR_TYPE_HIERARCHY_SUPERTYPES
            ? UMI_EDITOR_SYMBOL_RELATIONSHIP_BASE_TYPE
            : UMI_EDITOR_SYMBOL_RELATIONSHIP_DERIVED_TYPE;
    rules[0].node_kind = node_kind;
    rules[0].incoming = 0;

    rules[1].struct_size = (uint32_t)sizeof(rules[1]);
    rules[1].api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
    rules[1].relationship_kind =
        direction == UMI_EDITOR_TYPE_HIERARCHY_SUPERTYPES
            ? UMI_EDITOR_SYMBOL_RELATIONSHIP_DERIVED_TYPE
            : UMI_EDITOR_SYMBOL_RELATIONSHIP_BASE_TYPE;
    rules[1].node_kind = node_kind;
    rules[1].incoming = 1;

    return umi_editor_symbol_hierarchy_build_relationships(
        hierarchy,
        symbol_index,
        relationship_graph,
        root_symbol_id,
        rules,
        sizeof(rules) / sizeof(rules[0]),
        options);
}
