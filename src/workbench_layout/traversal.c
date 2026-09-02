/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/traversal.c
 *
 * PURPOSE:
 *   Implement deterministic pre-order, post-order and breadth-first traversal plus ancestry and descendant queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/traversal.h"

#include <string.h>

#include "internal.h"

typedef struct TraversalFrame {
    size_t node_index;
    size_t depth;
    size_t next_child;
    bool entered;
} TraversalFrame;

/*
 * Provide the traverse depth first operation used by this module and its client
 * applications.
 */
static UmiStatus traverse_depth_first(
    const UmiWorkbenchLayoutDocument *document,
    bool post_order,
    UmiWorkbenchLayoutTraversalVisitor visitor,
    void *visitor_context,
    UmiWorkbenchLayoutTraversalResult *result)
{
    TraversalFrame stack[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    bool visited[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t stack_count = 0U;

    (void)memset(visited, 0, sizeof(visited));
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_layout_index_valid(
            document->root_index, document->node_count)) {
        return UMI_STATUS_INVALID_STATE;
    }

    stack[stack_count++] = (TraversalFrame){
        document->root_index, 0U, 0U, false};

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (stack_count > 0U) {
        TraversalFrame *frame = &stack[stack_count - 1U];
        const UmiWorkbenchLayoutNode *node;

        /* Apply this operation only while the related capability or state is available. */
        if (!umi_workbench_layout_index_valid(
                frame->node_index, document->node_count)) {
            return UMI_STATUS_INVALID_STATE;
        }
        node = &document->nodes[frame->node_index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (!frame->entered) {
            frame->entered = true;
            /* Apply this branch only when its contract condition is satisfied. */
            if (visited[frame->node_index]) {
                return UMI_STATUS_INVALID_STATE;
            }
            visited[frame->node_index] = true;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (frame->depth > result->maximum_depth) {
                result->maximum_depth = frame->depth;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (!post_order) {
                result->visited_count += 1U;
                /* Apply this branch only when its contract condition is satisfied. */
                if (!visitor(
                        visitor_context,
                        document,
                        node,
                        frame->node_index,
                        frame->depth)) {
                    result->stopped_by_visitor = true;
                    return UMI_STATUS_OK;
                }
            }
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (frame->next_child < node->child_count) {
            const size_t child_index =
                node->child_indices[frame->next_child++];
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (stack_count >= UMI_WORKBENCH_LAYOUT_MAX_NODES) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            stack[stack_count++] = (TraversalFrame){
                child_index, frame->depth + 1U, 0U, false};
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (post_order) {
            result->visited_count += 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (!visitor(
                    visitor_context,
                    document,
                    node,
                    frame->node_index,
                    frame->depth)) {
                result->stopped_by_visitor = true;
                return UMI_STATUS_OK;
            }
        }
        stack_count -= 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the traverse breadth first operation used by this module and its client
 * applications.
 */
static UmiStatus traverse_breadth_first(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutTraversalVisitor visitor,
    void *visitor_context,
    UmiWorkbenchLayoutTraversalResult *result)
{
    size_t queue[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t depths[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    bool visited[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t head = 0U;
    size_t tail = 0U;

    (void)memset(visited, 0, sizeof(visited));
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_layout_index_valid(
            document->root_index, document->node_count)) {
        return UMI_STATUS_INVALID_STATE;
    }

    queue[tail] = document->root_index;
    depths[tail] = 0U;
    tail += 1U;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (head < tail) {
        const size_t node_index = queue[head];
        const size_t depth = depths[head];
        const UmiWorkbenchLayoutNode *node;
        size_t child;
        head += 1U;

        /* Apply this operation only while the related capability or state is available. */
        if (!umi_workbench_layout_index_valid(
                node_index, document->node_count) ||
            visited[node_index]) {
            return UMI_STATUS_INVALID_STATE;
        }
        visited[node_index] = true;
        node = &document->nodes[node_index];

        result->visited_count += 1U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (depth > result->maximum_depth) {
            result->maximum_depth = depth;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (!visitor(
                visitor_context,
                document,
                node,
                node_index,
                depth)) {
            result->stopped_by_visitor = true;
            return UMI_STATUS_OK;
        }

        /* Visit each bounded item once so every record receives the same rule. */
        for (child = 0U; child < node->child_count; ++child) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (tail >= UMI_WORKBENCH_LAYOUT_MAX_NODES) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            queue[tail] = node->child_indices[child];
            depths[tail] = depth + 1U;
            tail += 1U;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout traverse operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_traverse(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutTraversalOrder order,
    UmiWorkbenchLayoutTraversalVisitor visitor,
    void *visitor_context,
    UmiWorkbenchLayoutTraversalResult *out_result)
{
    UmiWorkbenchLayoutTraversalResult local_result;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || visitor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (document->node_count == 0U) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_result != NULL) {
            (void)memset(out_result, 0, sizeof(*out_result));
        }
        return UMI_STATUS_OK;
    }

    (void)memset(&local_result, 0, sizeof(local_result));
    /* Select the behaviour associated with the requested command or state value. */
    switch (order) {
    case UMI_WORKBENCH_LAYOUT_TRAVERSAL_PRE_ORDER:
        status = traverse_depth_first(
            document, false, visitor, visitor_context, &local_result);
        break;
    case UMI_WORKBENCH_LAYOUT_TRAVERSAL_POST_ORDER:
        status = traverse_depth_first(
            document, true, visitor, visitor_context, &local_result);
        break;
    case UMI_WORKBENCH_LAYOUT_TRAVERSAL_BREADTH_FIRST:
        status = traverse_breadth_first(
            document, visitor, visitor_context, &local_result);
        break;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_result != NULL) {
        *out_result = local_result;
    }
    return status;
}

/*
 * Provide the collect descendants recursive operation used by this module and its client
 * applications.
 */
static void collect_descendants_recursive(
    const UmiWorkbenchLayoutDocument *document,
    size_t node_index,
    size_t *out_indices,
    size_t capacity,
    size_t *count,
    bool *visited)
{
    const UmiWorkbenchLayoutNode *node;
    size_t child;

    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_layout_index_valid(
            node_index, document->node_count) ||
        visited[node_index]) {
        return;
    }
    visited[node_index] = true;
    node = &document->nodes[node_index];

    /* Visit each bounded item once so every record receives the same rule. */
    for (child = 0U; child < node->child_count; ++child) {
        const size_t child_index = node->child_indices[child];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (*count < capacity) {
            out_indices[*count] = child_index;
        }
        *count += 1U;
        collect_descendants_recursive(
            document,
            child_index,
            out_indices,
            capacity,
            count,
            visited);
    }
}

/*
 * Provide the workbench layout collect descendants operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_collect_descendants(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    size_t *out_indices,
    size_t capacity,
    size_t *out_count)
{
    bool visited[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t node_index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node_id == NULL || out_count == NULL ||
        (capacity > 0U && out_indices == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node_index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (node_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }

    (void)memset(visited, 0, sizeof(visited));
    collect_descendants_recursive(
        document,
        node_index,
        out_indices,
        capacity,
        &count,
        visited);
    *out_count = count;
    return count > capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/*
 * Provide the workbench layout collect ancestors operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_collect_ancestors(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    size_t *out_indices,
    size_t capacity,
    size_t *out_count)
{
    size_t node_index;
    size_t count = 0U;
    bool visited[UMI_WORKBENCH_LAYOUT_MAX_NODES];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node_id == NULL || out_count == NULL ||
        (capacity > 0U && out_indices == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    node_index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (node_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }

    (void)memset(visited, 0, sizeof(visited));
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (document->nodes[node_index].parent_index !=
           UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        node_index = document->nodes[node_index].parent_index;
        /* Apply this operation only while the related capability or state is available. */
        if (!umi_workbench_layout_index_valid(
                node_index, document->node_count) ||
            visited[node_index]) {
            return UMI_STATUS_INVALID_STATE;
        }
        visited[node_index] = true;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count < capacity) {
            out_indices[count] = node_index;
        }
        count += 1U;
    }

    *out_count = count;
    return count > capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/*
 * Provide the workbench layout is ancestor operation used by this module and its client
 * applications.
 */
bool umi_workbench_layout_is_ancestor(
    const UmiWorkbenchLayoutDocument *document,
    const char *possible_ancestor_id,
    const char *node_id)
{
    size_t ancestor_index;
    size_t node_index;
    bool visited[UMI_WORKBENCH_LAYOUT_MAX_NODES];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || possible_ancestor_id == NULL ||
        node_id == NULL) {
        return false;
    }
    ancestor_index = umi_workbench_layout_document_find_node_index(
        document, possible_ancestor_id);
    node_index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (ancestor_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE ||
        node_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return false;
    }

    (void)memset(visited, 0, sizeof(visited));
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (node_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (node_index == ancestor_index) {
            return true;
        }
        /* Apply this operation only while the related capability or state is available. */
        if (!umi_workbench_layout_index_valid(
                node_index, document->node_count) ||
            visited[node_index]) {
            return false;
        }
        visited[node_index] = true;
        node_index = document->nodes[node_index].parent_index;
    }
    return false;
}

/*
 * Provide the workbench layout node depth operation used by this module and its client
 * applications.
 */
size_t umi_workbench_layout_node_depth(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    size_t node_index;
    size_t depth = 0U;
    bool visited[UMI_WORKBENCH_LAYOUT_MAX_NODES];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node_id == NULL) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    node_index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (node_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }

    (void)memset(visited, 0, sizeof(visited));
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (document->nodes[node_index].parent_index !=
           UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (visited[node_index]) {
            return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
        }
        visited[node_index] = true;
        node_index = document->nodes[node_index].parent_index;
        /* Apply this operation only while the related capability or state is available. */
        if (!umi_workbench_layout_index_valid(
                node_index, document->node_count)) {
            return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
        }
        depth += 1U;
    }
    return depth;
}
