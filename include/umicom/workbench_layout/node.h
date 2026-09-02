/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/node.h
 *
 * PURPOSE:
 *   Define one semantic layout node and the invariants used by every toolkit adapter, layout editor, persistence service and recovery flow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_NODE_H
#define UMICOM_WORKBENCH_LAYOUT_NODE_H

#include "umicom/workbench_layout/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named workbench layout node flags values accepted by this public contract.
 */
typedef enum UmiWorkbenchLayoutNodeFlags {
    UMI_WORKBENCH_LAYOUT_NODE_CLOSABLE = 1U << 0,
    UMI_WORKBENCH_LAYOUT_NODE_MOVABLE = 1U << 1,
    UMI_WORKBENCH_LAYOUT_NODE_RESIZABLE = 1U << 2,
    UMI_WORKBENCH_LAYOUT_NODE_PINNED = 1U << 3,
    UMI_WORKBENCH_LAYOUT_NODE_TRANSIENT = 1U << 4,
    UMI_WORKBENCH_LAYOUT_NODE_PRIMARY = 1U << 5,
    UMI_WORKBENCH_LAYOUT_NODE_LOCKED = 1U << 6,
    UMI_WORKBENCH_LAYOUT_NODE_RESTORE_FOCUS = 1U << 7
} UmiWorkbenchLayoutNodeFlags;

/**
 * Represent the workbench layout node data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchLayoutNode {
    uint32_t structure_size;
    char node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char title[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char component_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char context_group_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char monitor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutNodeKind kind;
    UmiWorkbenchLayoutOrientation orientation;
    UmiWorkbenchLayoutDockRegion dock_region;
    UmiWorkbenchLayoutVisibility visibility;
    double split_ratio;
    UmiWorkbenchLayoutRect bounds;
    UmiWorkbenchLayoutSize minimum_size;
    UmiWorkbenchLayoutSize preferred_size;
    size_t parent_index;
    size_t child_indices[UMI_WORKBENCH_LAYOUT_MAX_CHILDREN];
    size_t child_count;
    size_t active_child_index;
    uint32_t flags;
    int32_t order;
    int32_t z_order;
    uint64_t revision;
} UmiWorkbenchLayoutNode;

/**
 * Initialise workbench layout node from caller-provided values so later operations receive
 * a known state.
 */
void umi_workbench_layout_node_init(
    UmiWorkbenchLayoutNode *node,
    const char *node_id,
    UmiWorkbenchLayoutNodeKind kind);

/**
 * Copy workbench layout node into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_workbench_layout_node_copy(
    UmiWorkbenchLayoutNode *destination,
    const UmiWorkbenchLayoutNode *source);

/**
 * Check that workbench layout node satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_workbench_layout_node_validate(
    const UmiWorkbenchLayoutNode *node);

/**
 * Provide the workbench layout node is container operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_node_is_container(
    const UmiWorkbenchLayoutNode *node);

/**
 * Provide the workbench layout node is leaf operation used by this module and its client
 * applications.
 */
bool umi_workbench_layout_node_is_leaf(
    const UmiWorkbenchLayoutNode *node);

/**
 * Provide the workbench layout node accepts child kind operation used by this module and
 * its client applications.
 */
bool umi_workbench_layout_node_accepts_child_kind(
    const UmiWorkbenchLayoutNode *parent,
    UmiWorkbenchLayoutNodeKind child_kind);

/**
 * Provide the workbench layout node has flag operation used by this module and its client
 * applications.
 */
bool umi_workbench_layout_node_has_flag(
    const UmiWorkbenchLayoutNode *node,
    UmiWorkbenchLayoutNodeFlags flag);

/**
 * Provide the workbench layout node set flag operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_node_set_flag(
    UmiWorkbenchLayoutNode *node,
    UmiWorkbenchLayoutNodeFlags flag,
    bool enabled);

/**
 * Provide the workbench layout node set title operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_node_set_title(
    UmiWorkbenchLayoutNode *node,
    const char *title);

/**
 * Provide the workbench layout node set component operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_node_set_component(
    UmiWorkbenchLayoutNode *node,
    const char *component_id,
    const char *owner_application_id);

/**
 * Provide the workbench layout node set context group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_node_set_context_group(
    UmiWorkbenchLayoutNode *node,
    const char *context_group_id);

/**
 * Provide the workbench layout node set monitor operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_node_set_monitor(
    UmiWorkbenchLayoutNode *node,
    const char *monitor_id);

/**
 * Provide the workbench layout node set bounds operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_node_set_bounds(
    UmiWorkbenchLayoutNode *node,
    const UmiWorkbenchLayoutRect *bounds);

/**
 * Provide the workbench layout node set split operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_node_set_split(
    UmiWorkbenchLayoutNode *node,
    UmiWorkbenchLayoutOrientation orientation,
    double split_ratio);

/**
 * Provide the workbench layout node add child index operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_node_add_child_index(
    UmiWorkbenchLayoutNode *node,
    size_t child_index,
    size_t position);

/**
 * Provide the workbench layout node remove child index operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_node_remove_child_index(
    UmiWorkbenchLayoutNode *node,
    size_t child_index);

/**
 * Provide the workbench layout node replace child index operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_node_replace_child_index(
    UmiWorkbenchLayoutNode *node,
    size_t existing_child_index,
    size_t replacement_child_index);

/**
 * Provide the workbench layout node move child index operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_node_move_child_index(
    UmiWorkbenchLayoutNode *node,
    size_t child_index,
    size_t new_position);

/**
 * Provide the workbench layout node contains child index operation used by this module and
 * its client applications.
 */
bool umi_workbench_layout_node_contains_child_index(
    const UmiWorkbenchLayoutNode *node,
    size_t child_index);

/**
 * Provide the workbench layout node child position operation used by this module and its
 * client applications.
 */
size_t umi_workbench_layout_node_child_position(
    const UmiWorkbenchLayoutNode *node,
    size_t child_index);

#ifdef __cplusplus
}
#endif

#endif
