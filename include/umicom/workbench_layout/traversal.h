/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/traversal.h
 *
 * PURPOSE:
 *   Traverse semantic layout trees deterministically without exposing toolkit widget objects or another component's private state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_TRAVERSAL_H
#define UMICOM_WORKBENCH_LAYOUT_TRAVERSAL_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWorkbenchLayoutTraversalOrder {
    UMI_WORKBENCH_LAYOUT_TRAVERSAL_PRE_ORDER = 1,
    UMI_WORKBENCH_LAYOUT_TRAVERSAL_POST_ORDER = 2,
    UMI_WORKBENCH_LAYOUT_TRAVERSAL_BREADTH_FIRST = 3
} UmiWorkbenchLayoutTraversalOrder;

typedef bool (*UmiWorkbenchLayoutTraversalVisitor)(
    void *context,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNode *node,
    size_t node_index,
    size_t depth);

typedef struct UmiWorkbenchLayoutTraversalResult {
    size_t visited_count;
    size_t maximum_depth;
    bool stopped_by_visitor;
} UmiWorkbenchLayoutTraversalResult;

UmiStatus umi_workbench_layout_traverse(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutTraversalOrder order,
    UmiWorkbenchLayoutTraversalVisitor visitor,
    void *visitor_context,
    UmiWorkbenchLayoutTraversalResult *out_result);

UmiStatus umi_workbench_layout_collect_descendants(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    size_t *out_indices,
    size_t capacity,
    size_t *out_count);

UmiStatus umi_workbench_layout_collect_ancestors(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    size_t *out_indices,
    size_t capacity,
    size_t *out_count);

bool umi_workbench_layout_is_ancestor(
    const UmiWorkbenchLayoutDocument *document,
    const char *possible_ancestor_id,
    const char *node_id);

size_t umi_workbench_layout_node_depth(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id);

#ifdef __cplusplus
}
#endif

#endif
