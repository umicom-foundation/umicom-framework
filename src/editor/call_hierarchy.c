/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/call_hierarchy.c
 *
 * PURPOSE:
 *   Translate incoming and outgoing call semantics into shared Framework
 *   hierarchy rules without introducing application-specific source logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/call_hierarchy.h"

UmiStatus umi_editor_call_hierarchy_build(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    UmiEditorCallHierarchyDirection direction,
    const UmiEditorSymbolHierarchyBuildOptions *options)
{
    UmiEditorSymbolHierarchyRule rules[2] = {0};
    UmiEditorSymbolHierarchyNodeKind node_kind;

    if (direction != UMI_EDITOR_CALL_HIERARCHY_INCOMING &&
        direction != UMI_EDITOR_CALL_HIERARCHY_OUTGOING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node_kind = direction == UMI_EDITOR_CALL_HIERARCHY_INCOMING
        ? UMI_EDITOR_SYMBOL_HIERARCHY_INCOMING_CALL
        : UMI_EDITOR_SYMBOL_HIERARCHY_OUTGOING_CALL;

    rules[0].struct_size = (uint32_t)sizeof(rules[0]);
    rules[0].api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
    rules[0].relationship_kind =
        direction == UMI_EDITOR_CALL_HIERARCHY_INCOMING
            ? UMI_EDITOR_SYMBOL_RELATIONSHIP_CALLER
            : UMI_EDITOR_SYMBOL_RELATIONSHIP_CALLEE;
    rules[0].node_kind = node_kind;
    rules[0].incoming = 0;

    rules[1].struct_size = (uint32_t)sizeof(rules[1]);
    rules[1].api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
    rules[1].relationship_kind =
        direction == UMI_EDITOR_CALL_HIERARCHY_INCOMING
            ? UMI_EDITOR_SYMBOL_RELATIONSHIP_CALLEE
            : UMI_EDITOR_SYMBOL_RELATIONSHIP_CALLER;
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
