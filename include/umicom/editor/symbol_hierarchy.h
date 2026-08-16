/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/symbol_hierarchy.h
 *
 * PURPOSE:
 *   Provide one reusable hierarchical symbol tree and relationship traversal
 *   engine for call, type and future language-aware hierarchy features.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SYMBOL_HIERARCHY_H
#define UMICOM_EDITOR_SYMBOL_HIERARCHY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/symbol_relationship.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION 1U
#define UMI_EDITOR_HIERARCHY_NODE_ID_CAPACITY 256U
#define UMI_EDITOR_HIERARCHY_DEFAULT_MAXIMUM_DEPTH 8U
#define UMI_EDITOR_HIERARCHY_DEFAULT_MAXIMUM_NODES 512U
#define UMI_EDITOR_HIERARCHY_MAXIMUM_DEPTH 64U
#define UMI_EDITOR_HIERARCHY_MAXIMUM_NODES 4096U

typedef enum UmiEditorSymbolHierarchyNodeKind {
    UMI_EDITOR_SYMBOL_HIERARCHY_ROOT = 1,
    UMI_EDITOR_SYMBOL_HIERARCHY_INCOMING_CALL = 2,
    UMI_EDITOR_SYMBOL_HIERARCHY_OUTGOING_CALL = 3,
    UMI_EDITOR_SYMBOL_HIERARCHY_SUPERTYPE = 4,
    UMI_EDITOR_SYMBOL_HIERARCHY_SUBTYPE = 5,
    UMI_EDITOR_SYMBOL_HIERARCHY_OVERRIDE = 6
} UmiEditorSymbolHierarchyNodeKind;

typedef struct UmiEditorSymbolHierarchyNode {
    uint32_t struct_size;
    uint32_t api_version;
    char node_id[UMI_EDITOR_HIERARCHY_NODE_ID_CAPACITY];
    char parent_node_id[UMI_EDITOR_HIERARCHY_NODE_ID_CAPACITY];
    char symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char provider_id[UMI_EDITOR_SYMBOL_PROVIDER_CAPACITY];
    char label[UMI_EDITOR_SYMBOL_NAME_CAPACITY];
    char detail[UMI_EDITOR_SYMBOL_DETAIL_CAPACITY];
    UmiEditorSymbolHierarchyNodeKind kind;
    UmiEditorIndexedSymbolKind symbol_kind;
    UmiEditorSourceLocation location;
    size_t depth;
    size_t child_count;
    size_t relationship_count;
    int expanded;
    int selected;
    int visible;
    int cycle;
} UmiEditorSymbolHierarchyNode;

typedef struct UmiEditorSymbolHierarchyRule {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorSymbolRelationshipKind relationship_kind;
    UmiEditorSymbolHierarchyNodeKind node_kind;
    int incoming;
} UmiEditorSymbolHierarchyRule;

typedef struct UmiEditorSymbolHierarchyBuildOptions {
    uint32_t struct_size;
    uint32_t api_version;
    size_t maximum_depth;
    size_t maximum_nodes;
    int include_indirect;
} UmiEditorSymbolHierarchyBuildOptions;

typedef struct UmiEditorSymbolHierarchySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t node_count;
    size_t visible_node_count;
    size_t selected_index;
    size_t cycle_count;
    uint64_t revision;
    int has_selection;
    int truncated;
} UmiEditorSymbolHierarchySnapshot;

typedef struct UmiEditorSymbolHierarchy UmiEditorSymbolHierarchy;

UmiStatus umi_editor_symbol_hierarchy_create(
    UmiEditorSymbolHierarchy **out_hierarchy);
void umi_editor_symbol_hierarchy_destroy(UmiEditorSymbolHierarchy *hierarchy);
UmiStatus umi_editor_symbol_hierarchy_clear(
    UmiEditorSymbolHierarchy *hierarchy);
UmiStatus umi_editor_symbol_hierarchy_add(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolHierarchyNode *node);
UmiStatus umi_editor_symbol_hierarchy_finalize(
    UmiEditorSymbolHierarchy *hierarchy,
    int truncated);
UmiStatus umi_editor_symbol_hierarchy_build_relationships(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    const UmiEditorSymbolHierarchyRule *rules,
    size_t rule_count,
    const UmiEditorSymbolHierarchyBuildOptions *options);
UmiStatus umi_editor_symbol_hierarchy_select(
    UmiEditorSymbolHierarchy *hierarchy,
    const char *node_id);
UmiStatus umi_editor_symbol_hierarchy_set_expanded(
    UmiEditorSymbolHierarchy *hierarchy,
    const char *node_id,
    int expanded);
UmiStatus umi_editor_symbol_hierarchy_at(
    const UmiEditorSymbolHierarchy *hierarchy,
    size_t index,
    UmiEditorSymbolHierarchyNode *out_node);
UmiStatus umi_editor_symbol_hierarchy_visible_at(
    const UmiEditorSymbolHierarchy *hierarchy,
    size_t visible_index,
    UmiEditorSymbolHierarchyNode *out_node);
UmiStatus umi_editor_symbol_hierarchy_selected(
    const UmiEditorSymbolHierarchy *hierarchy,
    UmiEditorSymbolHierarchyNode *out_node);
UmiStatus umi_editor_symbol_hierarchy_snapshot(
    const UmiEditorSymbolHierarchy *hierarchy,
    UmiEditorSymbolHierarchySnapshot *out_snapshot);
UmiStatus umi_editor_symbol_hierarchy_build_options_initialize(
    UmiEditorSymbolHierarchyBuildOptions *options);
size_t umi_editor_symbol_hierarchy_count(
    const UmiEditorSymbolHierarchy *hierarchy);
size_t umi_editor_symbol_hierarchy_visible_count(
    const UmiEditorSymbolHierarchy *hierarchy);
uint64_t umi_editor_symbol_hierarchy_revision(
    const UmiEditorSymbolHierarchy *hierarchy);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SYMBOL_HIERARCHY_H */
