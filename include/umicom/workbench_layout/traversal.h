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

/**
 * List the named workbench layout traversal order values accepted by this public contract.
 */
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

/**
 * Represent the workbench layout traversal result data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutTraversalResult {
    size_t visited_count;
    size_t maximum_depth;
    bool stopped_by_visitor;
} UmiWorkbenchLayoutTraversalResult;

/**
 * Provide the workbench layout traverse operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_traverse(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutTraversalOrder order,
    UmiWorkbenchLayoutTraversalVisitor visitor,
    void *visitor_context,
    UmiWorkbenchLayoutTraversalResult *out_result);

/**
 * Provide the workbench layout collect descendants operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_collect_descendants(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    size_t *out_indices,
    size_t capacity,
    size_t *out_count);

/**
 * Provide the workbench layout collect ancestors operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_collect_ancestors(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    size_t *out_indices,
    size_t capacity,
    size_t *out_count);

/**
 * Provide the workbench layout is ancestor operation used by this module and its client
 * applications.
 */
bool umi_workbench_layout_is_ancestor(
    const UmiWorkbenchLayoutDocument *document,
    const char *possible_ancestor_id,
    const char *node_id);

/**
 * Provide the workbench layout node depth operation used by this module and its client
 * applications.
 */
size_t umi_workbench_layout_node_depth(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id);

#ifdef __cplusplus
}
#endif

#endif
