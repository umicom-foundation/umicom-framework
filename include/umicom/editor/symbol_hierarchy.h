/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/symbol_hierarchy.h
 *
 * PURPOSE:
 *   Provide one reusable hierarchical symbol tree and relationship traversal
 *   engine for call, type and future language-aware hierarchy features.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named editor symbol hierarchy node kind values accepted by this public
 * contract.
 */
typedef enum UmiEditorSymbolHierarchyNodeKind {
    UMI_EDITOR_SYMBOL_HIERARCHY_ROOT = 1,
    UMI_EDITOR_SYMBOL_HIERARCHY_INCOMING_CALL = 2,
    UMI_EDITOR_SYMBOL_HIERARCHY_OUTGOING_CALL = 3,
    UMI_EDITOR_SYMBOL_HIERARCHY_SUPERTYPE = 4,
    UMI_EDITOR_SYMBOL_HIERARCHY_SUBTYPE = 5,
    UMI_EDITOR_SYMBOL_HIERARCHY_OVERRIDE = 6
} UmiEditorSymbolHierarchyNodeKind;

/**
 * Represent the editor symbol hierarchy node data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor symbol hierarchy rule data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSymbolHierarchyRule {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorSymbolRelationshipKind relationship_kind;
    UmiEditorSymbolHierarchyNodeKind node_kind;
    int incoming;
} UmiEditorSymbolHierarchyRule;

/**
 * Represent the editor symbol hierarchy build options data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorSymbolHierarchyBuildOptions {
    uint32_t struct_size;
    uint32_t api_version;
    size_t maximum_depth;
    size_t maximum_nodes;
    int include_indirect;
} UmiEditorSymbolHierarchyBuildOptions;

/**
 * Represent the editor symbol hierarchy snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor symbol hierarchy data shared with callers of this public contract.
 */
typedef struct UmiEditorSymbolHierarchy UmiEditorSymbolHierarchy;

/**
 * Initialise editor symbol hierarchy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_symbol_hierarchy_create(
    UmiEditorSymbolHierarchy **out_hierarchy);
/**
 * Release or reset state held by editor symbol hierarchy so the same storage can be reused
 * safely.
 */
void umi_editor_symbol_hierarchy_destroy(UmiEditorSymbolHierarchy *hierarchy);
/**
 * Release or reset state held by editor symbol hierarchy so the same storage can be reused
 * safely.
 */
UmiStatus umi_editor_symbol_hierarchy_clear(
    UmiEditorSymbolHierarchy *hierarchy);
/**
 * Add editor symbol hierarchy only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_symbol_hierarchy_add(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolHierarchyNode *node);
/**
 * Provide the editor symbol hierarchy finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_hierarchy_finalize(
    UmiEditorSymbolHierarchy *hierarchy,
    int truncated);
/**
 * Provide the editor symbol hierarchy build relationships operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_symbol_hierarchy_build_relationships(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    const UmiEditorSymbolHierarchyRule *rules,
    size_t rule_count,
    const UmiEditorSymbolHierarchyBuildOptions *options);
/**
 * Provide the editor symbol hierarchy select operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_symbol_hierarchy_select(
    UmiEditorSymbolHierarchy *hierarchy,
    const char *node_id);
/**
 * Provide the editor symbol hierarchy set expanded operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_hierarchy_set_expanded(
    UmiEditorSymbolHierarchy *hierarchy,
    const char *node_id,
    int expanded);
/**
 * Find editor symbol hierarchy while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_symbol_hierarchy_at(
    const UmiEditorSymbolHierarchy *hierarchy,
    size_t index,
    UmiEditorSymbolHierarchyNode *out_node);
/**
 * Find editor symbol hierarchy visible while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_symbol_hierarchy_visible_at(
    const UmiEditorSymbolHierarchy *hierarchy,
    size_t visible_index,
    UmiEditorSymbolHierarchyNode *out_node);
/**
 * Find editor symbol hierarchy while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_symbol_hierarchy_selected(
    const UmiEditorSymbolHierarchy *hierarchy,
    UmiEditorSymbolHierarchyNode *out_node);
/**
 * Provide the editor symbol hierarchy snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_hierarchy_snapshot(
    const UmiEditorSymbolHierarchy *hierarchy,
    UmiEditorSymbolHierarchySnapshot *out_snapshot);
/**
 * Initialise editor symbol hierarchy build options from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_symbol_hierarchy_build_options_initialize(
    UmiEditorSymbolHierarchyBuildOptions *options);
/**
 * Return the number of records represented by editor symbol hierarchy without changing
 * their state.
 */
size_t umi_editor_symbol_hierarchy_count(
    const UmiEditorSymbolHierarchy *hierarchy);
/**
 * Return the number of records represented by editor symbol hierarchy visible without
 * changing their state.
 */
size_t umi_editor_symbol_hierarchy_visible_count(
    const UmiEditorSymbolHierarchy *hierarchy);
/**
 * Provide the editor symbol hierarchy revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_symbol_hierarchy_revision(
    const UmiEditorSymbolHierarchy *hierarchy);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SYMBOL_HIERARCHY_H */
