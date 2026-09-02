/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/merge.c
 *
 * PURPOSE:
 *   Implement deterministic three-way layout merge, conflict reporting and explicit resolution without silently discarding local or remote work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/merge.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "internal.h"

/* Provide the node equal operation used by this module and its client applications. */
static bool node_equal(
    const UmiWorkbenchLayoutNode *left,
    const UmiWorkbenchLayoutNode *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) {
        return left == right;
    }
    return left->kind == right->kind &&
           left->orientation == right->orientation &&
           left->dock_region == right->dock_region &&
           left->visibility == right->visibility &&
           left->split_ratio == right->split_ratio &&
           left->bounds.x == right->bounds.x &&
           left->bounds.y == right->bounds.y &&
           left->bounds.width == right->bounds.width &&
           left->bounds.height == right->bounds.height &&
           left->minimum_size.width == right->minimum_size.width &&
           left->minimum_size.height == right->minimum_size.height &&
           left->preferred_size.width == right->preferred_size.width &&
           left->preferred_size.height == right->preferred_size.height &&
           left->parent_index == right->parent_index &&
           left->child_count == right->child_count &&
           left->active_child_index == right->active_child_index &&
           left->flags == right->flags &&
           left->order == right->order &&
           left->z_order == right->z_order &&
           strcmp(left->node_id, right->node_id) == 0 &&
           strcmp(left->title, right->title) == 0 &&
           strcmp(left->component_id, right->component_id) == 0 &&
           strcmp(left->owner_application_id,
                  right->owner_application_id) == 0 &&
           strcmp(left->context_group_id,
                  right->context_group_id) == 0 &&
           strcmp(left->monitor_id, right->monitor_id) == 0 &&
           memcmp(left->child_indices,
                  right->child_indices,
                  sizeof(left->child_indices)) == 0;
}

/* Provide the metadata equal operation used by this module and its client applications. */
static bool metadata_equal(
    const UmiWorkbenchLayoutDocument *left,
    const UmiWorkbenchLayoutDocument *right)
{
    return left != NULL && right != NULL &&
           strcmp(left->name, right->name) == 0 &&
           strcmp(left->category, right->category) == 0 &&
           strcmp(left->description, right->description) == 0 &&
           left->flags == right->flags &&
           left->tag_count == right->tag_count &&
           memcmp(left->tags,
                  right->tags,
                  sizeof(left->tags)) == 0;
}

/* Provide the node hash operation used by this module and its client applications. */
static uint64_t node_hash(
    const UmiWorkbenchLayoutNode *node)
{
    uint64_t hash = UMI_WORKBENCH_LAYOUT_FNV_OFFSET;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) {
        return 0U;
    }
    hash = umi_workbench_layout_hash_text(hash, node->node_id);
    hash = umi_workbench_layout_hash_text(hash, node->title);
    hash = umi_workbench_layout_hash_text(hash, node->component_id);
    hash = umi_workbench_layout_hash_text(
        hash, node->owner_application_id);
    hash = umi_workbench_layout_hash_text(
        hash, node->context_group_id);
    hash = umi_workbench_layout_hash_text(hash, node->monitor_id);
    hash = umi_workbench_layout_hash_u64(hash, (uint64_t)node->kind);
    hash = umi_workbench_layout_hash_u64(
        hash, (uint64_t)node->orientation);
    hash = umi_workbench_layout_hash_u64(
        hash, (uint64_t)node->dock_region);
    hash = umi_workbench_layout_hash_u64(
        hash, (uint64_t)node->visibility);
    hash = umi_workbench_layout_hash_double(hash, node->split_ratio);
    hash = umi_workbench_layout_hash_i32(hash, node->bounds.x);
    hash = umi_workbench_layout_hash_i32(hash, node->bounds.y);
    hash = umi_workbench_layout_hash_i32(hash, node->bounds.width);
    hash = umi_workbench_layout_hash_i32(hash, node->bounds.height);
    hash = umi_workbench_layout_hash_size(hash, node->parent_index);
    hash = umi_workbench_layout_hash_size(hash, node->child_count);
    hash = umi_workbench_layout_hash_bytes(
        hash, node->child_indices, sizeof(node->child_indices));
    hash = umi_workbench_layout_hash_u64(hash, node->flags);
    hash = umi_workbench_layout_hash_i32(hash, node->order);
    hash = umi_workbench_layout_hash_i32(hash, node->z_order);
    return hash;
}

/*
 * Provide the conflict value node operation used by this module and its client
 * applications.
 */
static UmiStatus conflict_value_node(
    char *buffer,
    size_t capacity,
    const UmiWorkbenchLayoutNode *node,
    const char *absent_text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) {
        return umi_workbench_layout_copy_text(
            buffer, capacity, absent_text, false);
    }
    return umi_workbench_layout_format(
        buffer,
        capacity,
        "node:%s hash:%016" PRIx64,
        node->node_id,
        node_hash(node));
}

/* Provide the add conflict operation used by this module and its client applications. */
static UmiStatus add_conflict(
    UmiWorkbenchLayoutMergePlan *plan,
    UmiWorkbenchLayoutConflictKind kind,
    const char *node_id,
    const char *field,
    const char *base_value,
    const char *local_value,
    const char *remote_value,
    UmiWorkbenchLayoutMergeResolution resolution)
{
    UmiWorkbenchLayoutMergeConflict *conflict;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || field == NULL ||
        base_value == NULL || local_value == NULL ||
        remote_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->conflict_count >=
        UMI_WORKBENCH_LAYOUT_MAX_CONFLICTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    conflict = &plan->conflicts[plan->conflict_count];
    (void)memset(conflict, 0, sizeof(*conflict));
    conflict->structure_size = sizeof(*conflict);
    conflict->kind = kind;
    conflict->resolution = resolution;

    status = umi_workbench_layout_format(
        conflict->conflict_id,
        sizeof(conflict->conflict_id),
        "conflict-%zu",
        plan->conflict_count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            conflict->node_id,
            sizeof(conflict->node_id),
            node_id != NULL ? node_id : "",
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            conflict->field,
            sizeof(conflict->field),
            field,
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            conflict->base_value,
            sizeof(conflict->base_value),
            base_value,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            conflict->local_value,
            sizeof(conflict->local_value),
            local_value,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            conflict->remote_value,
            sizeof(conflict->remote_value),
            remote_value,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(conflict, 0, sizeof(*conflict));
        return status;
    }

    plan->conflict_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the replace or add node operation used by this module and its client
 * applications.
 */
static UmiStatus replace_or_add_node(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNode *node)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = umi_workbench_layout_document_find_node_index(
        document, node->node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return umi_workbench_layout_document_add_node(
            document, node, NULL);
    }

    document->nodes[index] = *node;
    document->nodes[index].structure_size =
        sizeof(document->nodes[index]);
    return UMI_STATUS_OK;
}

/*
 * Provide the remove node if present operation used by this module and its client
 * applications.
 */
static UmiStatus remove_node_if_present(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_document_find_node(
            document, node_id) == NULL) {
        return UMI_STATUS_OK;
    }
    return umi_workbench_layout_document_remove_node(
        document, node_id, true);
}

/*
 * Provide the default resolution operation used by this module and its client
 * applications.
 */
static UmiWorkbenchLayoutMergeResolution default_resolution(
    const UmiWorkbenchLayoutMergeOptions *options,
    bool metadata)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (metadata && options->prefer_local_for_metadata) {
        return UMI_WORKBENCH_LAYOUT_MERGE_USE_LOCAL;
    }
    return UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED;
}

/* Provide the merge metadata operation used by this module and its client applications. */
static UmiStatus merge_metadata(
    const UmiWorkbenchLayoutDocument *base,
    const UmiWorkbenchLayoutDocument *local,
    const UmiWorkbenchLayoutDocument *remote,
    const UmiWorkbenchLayoutMergeOptions *options,
    UmiWorkbenchLayoutMergePlan *plan,
    UmiWorkbenchLayoutDocument *document)
{
    bool local_changed = !metadata_equal(base, local);
    bool remote_changed = !metadata_equal(base, remote);
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!local_changed && remote_changed) {
        status = umi_workbench_layout_document_set_metadata(
            document,
            remote->name,
            remote->category,
            remote->description);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        document->flags = remote->flags;
        document->tag_count = remote->tag_count;
        (void)memcpy(
            document->tags,
            remote->tags,
            sizeof(document->tags));
        plan->automatically_merged_count += 1U;
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (local_changed && !remote_changed) {
        *document = *local;
        plan->automatically_merged_count += 1U;
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!local_changed && !remote_changed) {
        return UMI_STATUS_OK;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (metadata_equal(local, remote)) {
        *document = *local;
        plan->automatically_merged_count += 1U;
        return UMI_STATUS_OK;
    }

    status = add_conflict(
        plan,
        UMI_WORKBENCH_LAYOUT_CONFLICT_METADATA,
        "",
        "metadata",
        base->name,
        local->name,
        remote->name,
        default_resolution(options, true));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (options->prefer_local_for_metadata) {
        *document = *local;
    }
    return UMI_STATUS_OK;
}

/* Provide the merge one node operation used by this module and its client applications. */
static UmiStatus merge_one_node(
    const UmiWorkbenchLayoutDocument *base,
    const UmiWorkbenchLayoutDocument *local,
    const UmiWorkbenchLayoutDocument *remote,
    const char *node_id,
    const UmiWorkbenchLayoutMergeOptions *options,
    UmiWorkbenchLayoutMergePlan *plan,
    UmiWorkbenchLayoutDocument *document)
{
    const UmiWorkbenchLayoutNode *base_node =
        umi_workbench_layout_document_find_node(base, node_id);
    const UmiWorkbenchLayoutNode *local_node =
        umi_workbench_layout_document_find_node(local, node_id);
    const UmiWorkbenchLayoutNode *remote_node =
        umi_workbench_layout_document_find_node(remote, node_id);
    char base_value[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    char local_value[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    char remote_value[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (base_node == NULL) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (local_node != NULL && remote_node == NULL) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (options->preserve_local_additions) {
                status = replace_or_add_node(document, local_node);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    plan->automatically_merged_count += 1U;
                }
                return status;
            }
            return UMI_STATUS_OK;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (local_node == NULL && remote_node != NULL) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (options->preserve_remote_additions) {
                status = replace_or_add_node(document, remote_node);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    plan->automatically_merged_count += 1U;
                }
                return status;
            }
            return UMI_STATUS_OK;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (local_node != NULL && remote_node != NULL) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (node_equal(local_node, remote_node)) {
                status = replace_or_add_node(document, local_node);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    plan->automatically_merged_count += 1U;
                }
                return status;
            }

            (void)conflict_value_node(
                base_value, sizeof(base_value), NULL, "absent");
            (void)conflict_value_node(
                local_value, sizeof(local_value),
                local_node, "absent");
            (void)conflict_value_node(
                remote_value, sizeof(remote_value),
                remote_node, "absent");
            return add_conflict(
                plan,
                UMI_WORKBENCH_LAYOUT_CONFLICT_NODE_ADDED_DIFFERENTLY,
                node_id,
                "node",
                base_value,
                local_value,
                remote_value,
                UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED);
        }
        return UMI_STATUS_OK;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (local_node == NULL && remote_node == NULL) {
        status = remove_node_if_present(document, node_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            plan->automatically_merged_count += 1U;
        }
        return status;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (local_node == NULL && remote_node != NULL) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (node_equal(base_node, remote_node)) {
            status = remove_node_if_present(document, node_id);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                plan->automatically_merged_count += 1U;
            }
            return status;
        }
        (void)conflict_value_node(
            base_value, sizeof(base_value),
            base_node, "absent");
        (void)conflict_value_node(
            local_value, sizeof(local_value), NULL, "removed");
        (void)conflict_value_node(
            remote_value, sizeof(remote_value),
            remote_node, "absent");
        return add_conflict(
            plan,
            UMI_WORKBENCH_LAYOUT_CONFLICT_NODE_REMOVED_AND_CHANGED,
            node_id,
            "node",
            base_value,
            local_value,
            remote_value,
            UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED);
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (local_node != NULL && remote_node == NULL) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (node_equal(base_node, local_node)) {
            status = remove_node_if_present(document, node_id);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                plan->automatically_merged_count += 1U;
            }
            return status;
        }
        (void)conflict_value_node(
            base_value, sizeof(base_value),
            base_node, "absent");
        (void)conflict_value_node(
            local_value, sizeof(local_value),
            local_node, "absent");
        (void)conflict_value_node(
            remote_value, sizeof(remote_value), NULL, "removed");
        return add_conflict(
            plan,
            UMI_WORKBENCH_LAYOUT_CONFLICT_NODE_REMOVED_AND_CHANGED,
            node_id,
            "node",
            base_value,
            local_value,
            remote_value,
            UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED);
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (node_equal(local_node, remote_node)) {
        status = replace_or_add_node(document, local_node);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK &&
            !node_equal(base_node, local_node)) {
            plan->automatically_merged_count += 1U;
        }
        return status;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (node_equal(base_node, local_node)) {
        status = replace_or_add_node(document, remote_node);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            plan->automatically_merged_count += 1U;
        }
        return status;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (node_equal(base_node, remote_node)) {
        status = replace_or_add_node(document, local_node);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            plan->automatically_merged_count += 1U;
        }
        return status;
    }

    (void)conflict_value_node(
        base_value, sizeof(base_value), base_node, "absent");
    (void)conflict_value_node(
        local_value, sizeof(local_value), local_node, "absent");
    (void)conflict_value_node(
        remote_value, sizeof(remote_value), remote_node, "absent");
    return add_conflict(
        plan,
        UMI_WORKBENCH_LAYOUT_CONFLICT_NODE_CHANGED_DIFFERENTLY,
        node_id,
        "node",
        base_value,
        local_value,
        remote_value,
        UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED);
}

/*
 * Provide the identifier already processed operation used by this module and its client
 * applications.
 */
static bool identifier_already_processed(
    char processed[UMI_WORKBENCH_LAYOUT_MAX_NODES * 3U]
                  [UMI_WORKBENCH_LAYOUT_ID_CAPACITY],
    size_t processed_count,
    const char *node_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < processed_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(processed[index], node_id) == 0) {
            return true;
        }
    }
    return false;
}

/* Provide the merge node sets operation used by this module and its client applications. */
static UmiStatus merge_node_sets(
    const UmiWorkbenchLayoutDocument *base,
    const UmiWorkbenchLayoutDocument *local,
    const UmiWorkbenchLayoutDocument *remote,
    const UmiWorkbenchLayoutMergeOptions *options,
    UmiWorkbenchLayoutMergePlan *plan,
    UmiWorkbenchLayoutDocument *document)
{
    char processed[UMI_WORKBENCH_LAYOUT_MAX_NODES * 3U]
                  [UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    const UmiWorkbenchLayoutDocument *documents[3U] = {
        base, local, remote
    };
    size_t processed_count = 0U;
    size_t document_index;
    size_t node_index;
    UmiStatus status = UMI_STATUS_OK;

    (void)memset(processed, 0, sizeof(processed));
    /* Visit each bounded item once so every record receives the same rule. */
    for (document_index = 0U;
         document_index < 3U && status == UMI_STATUS_OK;
         ++document_index) {
        const UmiWorkbenchLayoutDocument *source =
            documents[document_index];

        /* Visit each bounded item once so every record receives the same rule. */
        for (node_index = 0U;
             node_index < source->node_count &&
             status == UMI_STATUS_OK;
             ++node_index) {
            const char *node_id = source->nodes[node_index].node_id;

            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (identifier_already_processed(
                    processed, processed_count, node_id)) {
                continue;
            }
            status = umi_workbench_layout_copy_text(
                processed[processed_count],
                sizeof(processed[processed_count]),
                node_id,
                false);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
            processed_count += 1U;
            status = merge_one_node(
                base,
                local,
                remote,
                node_id,
                options,
                plan,
                document);
        }
    }
    return status;
}

/*
 * Provide the select conflict node operation used by this module and its client
 * applications.
 */
static UmiStatus select_conflict_node(
    const UmiWorkbenchLayoutDocument *base,
    const UmiWorkbenchLayoutDocument *local,
    const UmiWorkbenchLayoutDocument *remote,
    const UmiWorkbenchLayoutMergeConflict *conflict,
    const UmiWorkbenchLayoutNode **out_node,
    bool *out_remove)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (conflict == NULL || out_node == NULL ||
        out_remove == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_node = NULL;
    *out_remove = false;

    /* Select the behaviour associated with the requested command or state value. */
    switch (conflict->resolution) {
    case UMI_WORKBENCH_LAYOUT_MERGE_USE_BASE:
        *out_node = umi_workbench_layout_document_find_node(
            base, conflict->node_id);
        break;
    case UMI_WORKBENCH_LAYOUT_MERGE_USE_LOCAL:
        *out_node = umi_workbench_layout_document_find_node(
            local, conflict->node_id);
        break;
    case UMI_WORKBENCH_LAYOUT_MERGE_USE_REMOTE:
        *out_node = umi_workbench_layout_document_find_node(
            remote, conflict->node_id);
        break;
    case UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM:
        return UMI_STATUS_NOT_IMPLEMENTED;
    default:
        return UMI_STATUS_INVALID_STATE;
    }

    *out_remove = *out_node == NULL;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout merge options default operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutMergeOptions
umi_workbench_layout_merge_options_default(void)
{
    UmiWorkbenchLayoutMergeOptions options;

    (void)memset(&options, 0, sizeof(options));
    options.structure_size = sizeof(options);
    options.prefer_local_for_metadata = false;
    options.preserve_remote_additions = true;
    options.preserve_local_additions = true;
    options.reject_unresolved_conflicts = true;
    return options;
}

/*
 * Initialise workbench layout merge plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_merge_plan_init(
    UmiWorkbenchLayoutMergePlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) {
        return;
    }
    (void)memset(plan, 0, sizeof(*plan));
    plan->structure_size = sizeof(*plan);
    plan->complete = true;
}

/*
 * Provide the workbench layout merge three way operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_merge_three_way(
    const UmiWorkbenchLayoutDocument *base,
    const UmiWorkbenchLayoutDocument *local,
    const UmiWorkbenchLayoutDocument *remote,
    const UmiWorkbenchLayoutMergeOptions *options,
    UmiWorkbenchLayoutMergePlan *out_plan,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchLayoutMergeOptions effective;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (base == NULL || local == NULL || remote == NULL ||
        out_plan == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    effective = options != NULL
        ? *options
        : umi_workbench_layout_merge_options_default();
    umi_workbench_layout_merge_plan_init(out_plan);
    status = umi_workbench_layout_document_copy(
        out_document, base);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = merge_metadata(
            base,
            local,
            remote,
            &effective,
            out_plan,
            out_document);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = merge_node_sets(
            base,
            local,
            remote,
            &effective,
            out_plan,
            out_document);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        out_plan->complete = false;
        return status;
    }

    out_plan->complete =
        umi_workbench_layout_merge_unresolved_count(out_plan) == 0U;
    umi_workbench_layout_document_refresh_hash(out_document);

    /* Apply this branch only when its contract condition is satisfied. */
    if (!out_plan->complete &&
        effective.reject_unresolved_conflicts) {
        return UMI_STATUS_BUSY;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout merge resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_merge_resolve(
    UmiWorkbenchLayoutMergePlan *plan,
    const char *conflict_id,
    UmiWorkbenchLayoutMergeResolution resolution,
    const char *custom_value)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL ||
        !umi_workbench_layout_text_present(conflict_id) ||
        resolution < UMI_WORKBENCH_LAYOUT_MERGE_USE_BASE ||
        resolution > UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->conflict_count; ++index) {
        UmiWorkbenchLayoutMergeConflict *conflict =
            &plan->conflicts[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_workbench_layout_text_equal(
                conflict->conflict_id, conflict_id)) {
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (resolution == UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM) {
            status = umi_workbench_layout_copy_text(
                conflict->custom_value,
                sizeof(conflict->custom_value),
                custom_value != NULL ? custom_value : "",
                false);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            conflict->custom_value[0] = '\0';
        }
        conflict->resolution = resolution;
        plan->complete =
            umi_workbench_layout_merge_unresolved_count(plan) == 0U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the workbench layout merge apply resolutions operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_merge_apply_resolutions(
    const UmiWorkbenchLayoutDocument *base,
    const UmiWorkbenchLayoutDocument *local,
    const UmiWorkbenchLayoutDocument *remote,
    const UmiWorkbenchLayoutMergePlan *plan,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchLayoutMergeOptions options =
        umi_workbench_layout_merge_options_default();
    UmiWorkbenchLayoutMergePlan automatic_plan;
    UmiStatus status;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (base == NULL || local == NULL || remote == NULL ||
        plan == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_merge_unresolved_count(plan) > 0U) {
        return UMI_STATUS_BUSY;
    }

    options.reject_unresolved_conflicts = false;
    status = umi_workbench_layout_merge_three_way(
        base,
        local,
        remote,
        &options,
        &automatic_plan,
        out_document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK && status != UMI_STATUS_BUSY) {
        return status;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->conflict_count; ++index) {
        const UmiWorkbenchLayoutMergeConflict *conflict =
            &plan->conflicts[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (conflict->kind ==
            UMI_WORKBENCH_LAYOUT_CONFLICT_METADATA) {
            const UmiWorkbenchLayoutDocument *source = NULL;
            /* Apply this branch only when its contract condition is satisfied. */
            if (conflict->resolution ==
                UMI_WORKBENCH_LAYOUT_MERGE_USE_BASE) {
                source = base;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (conflict->resolution ==
                       UMI_WORKBENCH_LAYOUT_MERGE_USE_LOCAL) {
                source = local;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (conflict->resolution ==
                       UMI_WORKBENCH_LAYOUT_MERGE_USE_REMOTE) {
                source = remote;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (conflict->resolution ==
                       UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM) {
                status = umi_workbench_layout_copy_text(
                    out_document->name,
                    sizeof(out_document->name),
                    conflict->custom_value,
                    false);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) {
                    return status;
                }
                continue;
            }
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (source != NULL) {
                status = umi_workbench_layout_document_set_metadata(
                    out_document,
                    source->name,
                    source->category,
                    source->description);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) {
                    return status;
                }
                out_document->flags = source->flags;
                out_document->tag_count = source->tag_count;
                (void)memcpy(
                    out_document->tags,
                    source->tags,
                    sizeof(out_document->tags));
            }
            continue;
        }

        {
            const UmiWorkbenchLayoutNode *selected_node;
            bool remove_selected;
            status = select_conflict_node(
                base,
                local,
                remote,
                conflict,
                &selected_node,
                &remove_selected);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (remove_selected) {
                status = remove_node_if_present(
                    out_document, conflict->node_id);
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                status = replace_or_add_node(
                    out_document, selected_node);
            }
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
        }
    }

    umi_workbench_layout_document_increment_revision(out_document);
    return UMI_STATUS_OK;
}

/*
 * Find workbench layout merge conflict while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchLayoutMergeConflict *
umi_workbench_layout_merge_conflict_at(
    const UmiWorkbenchLayoutMergePlan *plan,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || index >= plan->conflict_count) {
        return NULL;
    }
    return &plan->conflicts[index];
}

/*
 * Return the number of records represented by workbench layout merge unresolved without
 * changing their state.
 */
size_t umi_workbench_layout_merge_unresolved_count(
    const UmiWorkbenchLayoutMergePlan *plan)
{
    size_t index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->conflict_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (plan->conflicts[index].resolution ==
            UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED) {
            count += 1U;
        }
    }
    return count;
}
