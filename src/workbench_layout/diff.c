/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/diff.c
 *
 * PURPOSE:
 *   Implement deterministic semantic layout comparison for review, audit, source control and Beyond Compare-style inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/diff.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "internal.h"

/* Provide the value text operation used by this module and its client applications. */
static UmiStatus value_text(
    char *buffer,
    size_t capacity,
    const char *value)
{
    return umi_workbench_layout_copy_text(
        buffer,
        capacity,
        value != NULL ? value : "",
        true);
}

/* Provide the value u64 operation used by this module and its client applications. */
static UmiStatus value_u64(
    char *buffer,
    size_t capacity,
    uint64_t value)
{
    return umi_workbench_layout_format(
        buffer, capacity, "%" PRIu64, value);
}

/* Return the number of records represented by value without changing their state. */
static UmiStatus value_size(
    char *buffer,
    size_t capacity,
    size_t value)
{
    return umi_workbench_layout_format(
        buffer, capacity, "%zu", value);
}

/* Provide the value i32 operation used by this module and its client applications. */
static UmiStatus value_i32(
    char *buffer,
    size_t capacity,
    int32_t value)
{
    return umi_workbench_layout_format(
        buffer, capacity, "%" PRId32, value);
}

/* Provide the value double operation used by this module and its client applications. */
static UmiStatus value_double(
    char *buffer,
    size_t capacity,
    double value)
{
    return umi_workbench_layout_format(
        buffer, capacity, "%.6f", value);
}

/* Provide the add text change operation used by this module and its client applications. */
static UmiStatus add_text_change(
    UmiWorkbenchLayoutDiff *diff,
    UmiWorkbenchLayoutDiffKind kind,
    const char *node_id,
    const char *field,
    const char *before,
    const char *after,
    size_t before_index,
    size_t after_index)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_workbench_layout_text_equal(before, after)) {
        return UMI_STATUS_OK;
    }
    return umi_workbench_layout_diff_add(
        diff,
        kind,
        node_id,
        field,
        before != NULL ? before : "",
        after != NULL ? after : "",
        before_index,
        after_index);
}

/* Provide the add u64 change operation used by this module and its client applications. */
static UmiStatus add_u64_change(
    UmiWorkbenchLayoutDiff *diff,
    UmiWorkbenchLayoutDiffKind kind,
    const char *node_id,
    const char *field,
    uint64_t before,
    uint64_t after,
    size_t before_index,
    size_t after_index)
{
    char before_text[64U];
    char after_text[64U];
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (before == after) {
        return UMI_STATUS_OK;
    }
    status = value_u64(before_text, sizeof(before_text), before);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = value_u64(after_text, sizeof(after_text), after);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_workbench_layout_diff_add(
        diff,
        kind,
        node_id,
        field,
        before_text,
        after_text,
        before_index,
        after_index);
}

/* Provide the add size change operation used by this module and its client applications. */
static UmiStatus add_size_change(
    UmiWorkbenchLayoutDiff *diff,
    UmiWorkbenchLayoutDiffKind kind,
    const char *node_id,
    const char *field,
    size_t before,
    size_t after,
    size_t before_index,
    size_t after_index)
{
    char before_text[64U];
    char after_text[64U];
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (before == after) {
        return UMI_STATUS_OK;
    }
    status = value_size(before_text, sizeof(before_text), before);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = value_size(after_text, sizeof(after_text), after);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_workbench_layout_diff_add(
        diff,
        kind,
        node_id,
        field,
        before_text,
        after_text,
        before_index,
        after_index);
}

/* Provide the add i32 change operation used by this module and its client applications. */
static UmiStatus add_i32_change(
    UmiWorkbenchLayoutDiff *diff,
    const char *node_id,
    const char *field,
    int32_t before,
    int32_t after,
    size_t before_index,
    size_t after_index)
{
    char before_text[64U];
    char after_text[64U];
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (before == after) {
        return UMI_STATUS_OK;
    }
    status = value_i32(before_text, sizeof(before_text), before);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = value_i32(after_text, sizeof(after_text), after);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_workbench_layout_diff_add(
        diff,
        UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
        node_id,
        field,
        before_text,
        after_text,
        before_index,
        after_index);
}

/* Provide the add double change operation used by this module and its client applications. */
static UmiStatus add_double_change(
    UmiWorkbenchLayoutDiff *diff,
    const char *node_id,
    const char *field,
    double before,
    double after,
    size_t before_index,
    size_t after_index)
{
    char before_text[64U];
    char after_text[64U];
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (before == after) {
        return UMI_STATUS_OK;
    }
    status = value_double(before_text, sizeof(before_text), before);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = value_double(after_text, sizeof(after_text), after);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_workbench_layout_diff_add(
        diff,
        UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
        node_id,
        field,
        before_text,
        after_text,
        before_index,
        after_index);
}

/* Provide the compare metadata operation used by this module and its client applications. */
static UmiStatus compare_metadata(
    const UmiWorkbenchLayoutDocument *before,
    const UmiWorkbenchLayoutDocument *after,
    const UmiWorkbenchLayoutDiffOptions *options,
    UmiWorkbenchLayoutDiff *diff)
{
    UmiStatus status;

    status = add_text_change(
        diff,
        UMI_WORKBENCH_LAYOUT_DIFF_METADATA_CHANGED,
        "",
        "name",
        before->name,
        after->name,
        UMI_WORKBENCH_LAYOUT_INDEX_NONE,
        UMI_WORKBENCH_LAYOUT_INDEX_NONE);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_text_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_METADATA_CHANGED,
            "",
            "category",
            before->category,
            after->category,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_text_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_METADATA_CHANGED,
            "",
            "description",
            before->description,
            after->description,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_u64_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_METADATA_CHANGED,
            "",
            "flags",
            before->flags,
            after->flags,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_size_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_METADATA_CHANGED,
            "",
            "tag-count",
            before->tag_count,
            after->tag_count,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        options->include_revision) {
        status = add_u64_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_METADATA_CHANGED,
            "",
            "revision",
            before->version.revision,
            after->version.revision,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE,
            UMI_WORKBENCH_LAYOUT_INDEX_NONE);
    }
    return status;
}

/* Provide the compare children operation used by this module and its client applications. */
static UmiStatus compare_children(
    const UmiWorkbenchLayoutNode *before,
    const UmiWorkbenchLayoutNode *after,
    const UmiWorkbenchLayoutDiffOptions *options,
    UmiWorkbenchLayoutDiff *diff,
    size_t before_index,
    size_t after_index)
{
    size_t index;
    UmiStatus status;

    status = add_size_change(
        diff,
        UMI_WORKBENCH_LAYOUT_DIFF_ORDER_CHANGED,
        before->node_id,
        "child-count",
        before->child_count,
        after->child_count,
        before_index,
        after_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || !options->include_order) {
        return status;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < before->child_count &&
         index < after->child_count;
         ++index) {
        status = add_size_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_ORDER_CHANGED,
            before->node_id,
            "child-index",
            before->child_indices[index],
            after->child_indices[index],
            before_index,
            after_index);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the compare node fields operation used by this module and its client
 * applications.
 */
static UmiStatus compare_node_fields(
    const UmiWorkbenchLayoutNode *before,
    const UmiWorkbenchLayoutNode *after,
    const UmiWorkbenchLayoutDiffOptions *options,
    UmiWorkbenchLayoutDiff *diff,
    size_t before_index,
    size_t after_index)
{
    UmiStatus status;

    status = add_text_change(
        diff,
        UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
        before->node_id,
        "title",
        before->title,
        after->title,
        before_index,
        after_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_text_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
            before->node_id,
            "component-id",
            before->component_id,
            after->component_id,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_text_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
            before->node_id,
            "owner-application-id",
            before->owner_application_id,
            after->owner_application_id,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_text_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
            before->node_id,
            "context-group-id",
            before->context_group_id,
            after->context_group_id,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_text_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
            before->node_id,
            "monitor-id",
            before->monitor_id,
            after->monitor_id,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_u64_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
            before->node_id,
            "kind",
            (uint64_t)before->kind,
            (uint64_t)after->kind,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_u64_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
            before->node_id,
            "orientation",
            (uint64_t)before->orientation,
            (uint64_t)after->orientation,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_u64_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
            before->node_id,
            "dock-region",
            (uint64_t)before->dock_region,
            (uint64_t)after->dock_region,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_u64_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
            before->node_id,
            "visibility",
            (uint64_t)before->visibility,
            (uint64_t)after->visibility,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_double_change(
            diff,
            before->node_id,
            "split-ratio",
            before->split_ratio,
            after->split_ratio,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_u64_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
            before->node_id,
            "flags",
            before->flags,
            after->flags,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_i32_change(
            diff,
            before->node_id,
            "order",
            before->order,
            after->order,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_i32_change(
            diff,
            before->node_id,
            "z-order",
            before->z_order,
            after->z_order,
            before_index,
            after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_size_change(
            diff,
            UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED,
            before->node_id,
            "active-child-index",
            before->active_child_index,
            after->active_child_index,
            before_index,
            after_index);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && options->include_geometry) {
        status = add_i32_change(
            diff, before->node_id, "bounds.x",
            before->bounds.x, after->bounds.x,
            before_index, after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && options->include_geometry) {
        status = add_i32_change(
            diff, before->node_id, "bounds.y",
            before->bounds.y, after->bounds.y,
            before_index, after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && options->include_geometry) {
        status = add_i32_change(
            diff, before->node_id, "bounds.width",
            before->bounds.width, after->bounds.width,
            before_index, after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && options->include_geometry) {
        status = add_i32_change(
            diff, before->node_id, "bounds.height",
            before->bounds.height, after->bounds.height,
            before_index, after_index);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = compare_children(
            before, after, options, diff,
            before_index, after_index);
    }
    return status;
}

/*
 * Provide the workbench layout diff options default operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutDiffOptions
umi_workbench_layout_diff_options_default(void)
{
    UmiWorkbenchLayoutDiffOptions options;

    (void)memset(&options, 0, sizeof(options));
    options.structure_size = sizeof(options);
    options.include_metadata = true;
    options.include_geometry = true;
    options.include_order = true;
    options.include_revision = false;
    options.include_unchanged_nodes = false;
    return options;
}

/*
 * Initialise workbench layout diff from caller-provided values so later operations receive
 * a known state.
 */
void umi_workbench_layout_diff_init(
    UmiWorkbenchLayoutDiff *diff)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diff == NULL) {
        return;
    }
    (void)memset(diff, 0, sizeof(*diff));
    diff->structure_size = sizeof(*diff);
}

/*
 * Add workbench layout diff only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_layout_diff_add(
    UmiWorkbenchLayoutDiff *diff,
    UmiWorkbenchLayoutDiffKind kind,
    const char *node_id,
    const char *field,
    const char *before_value,
    const char *after_value,
    size_t before_index,
    size_t after_index)
{
    UmiWorkbenchLayoutDiffEntry *entry;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diff == NULL || field == NULL ||
        before_value == NULL || after_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (diff->entry_count >= UMI_WORKBENCH_LAYOUT_MAX_DIFF_ENTRIES) {
        diff->truncated = true;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    entry = &diff->entries[diff->entry_count];
    (void)memset(entry, 0, sizeof(*entry));
    entry->structure_size = sizeof(*entry);
    entry->kind = kind;
    entry->before_index = before_index;
    entry->after_index = after_index;

    status = value_text(
        entry->node_id, sizeof(entry->node_id),
        node_id != NULL ? node_id : "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = value_text(
            entry->field, sizeof(entry->field), field);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = value_text(
            entry->before_value,
            sizeof(entry->before_value),
            before_value);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = value_text(
            entry->after_value,
            sizeof(entry->after_value),
            after_value);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(entry, 0, sizeof(*entry));
        return status;
    }

    diff->entry_count += 1U;
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_WORKBENCH_LAYOUT_DIFF_METADATA_CHANGED:
        diff->metadata_change_count += 1U;
        break;
    case UMI_WORKBENCH_LAYOUT_DIFF_NODE_ADDED:
        diff->added_count += 1U;
        break;
    case UMI_WORKBENCH_LAYOUT_DIFF_NODE_REMOVED:
        diff->removed_count += 1U;
        break;
    case UMI_WORKBENCH_LAYOUT_DIFF_NODE_MOVED:
        diff->moved_count += 1U;
        break;
    case UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED:
    case UMI_WORKBENCH_LAYOUT_DIFF_ORDER_CHANGED:
        diff->changed_count += 1U;
        break;
    default:
        break;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout compare operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_compare(
    const UmiWorkbenchLayoutDocument *before,
    const UmiWorkbenchLayoutDocument *after,
    const UmiWorkbenchLayoutDiffOptions *options,
    UmiWorkbenchLayoutDiff *out_diff)
{
    UmiWorkbenchLayoutDiffOptions effective;
    size_t before_index;
    size_t after_index;
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (before == NULL || after == NULL || out_diff == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    effective = options != NULL
        ? *options
        : umi_workbench_layout_diff_options_default();
    umi_workbench_layout_diff_init(out_diff);

    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.include_metadata) {
        status = compare_metadata(
            before, after, &effective, out_diff);
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (before_index = 0U;
         status == UMI_STATUS_OK &&
         before_index < before->node_count;
         ++before_index) {
        const UmiWorkbenchLayoutNode *before_node =
            &before->nodes[before_index];

        after_index = umi_workbench_layout_document_find_node_index(
            after, before_node->node_id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (after_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            status = umi_workbench_layout_diff_add(
                out_diff,
                UMI_WORKBENCH_LAYOUT_DIFF_NODE_REMOVED,
                before_node->node_id,
                "node",
                "present",
                "removed",
                before_index,
                UMI_WORKBENCH_LAYOUT_INDEX_NONE);
            continue;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (before_node->parent_index !=
            after->nodes[after_index].parent_index) {
            status = umi_workbench_layout_diff_add(
                out_diff,
                UMI_WORKBENCH_LAYOUT_DIFF_NODE_MOVED,
                before_node->node_id,
                "parent-index",
                "changed",
                "changed",
                before_index,
                after_index);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = compare_node_fields(
                before_node,
                &after->nodes[after_index],
                &effective,
                out_diff,
                before_index,
                after_index);
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (after_index = 0U;
         status == UMI_STATUS_OK &&
         after_index < after->node_count;
         ++after_index) {
        const UmiWorkbenchLayoutNode *after_node =
            &after->nodes[after_index];

        before_index = umi_workbench_layout_document_find_node_index(
            before, after_node->node_id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (before_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            status = umi_workbench_layout_diff_add(
                out_diff,
                UMI_WORKBENCH_LAYOUT_DIFF_NODE_ADDED,
                after_node->node_id,
                "node",
                "absent",
                "present",
                UMI_WORKBENCH_LAYOUT_INDEX_NONE,
                after_index);
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
        out_diff->truncated = true;
        return UMI_STATUS_OK;
    }
    return status;
}

/*
 * Find workbench layout diff while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWorkbenchLayoutDiffEntry *
umi_workbench_layout_diff_at(
    const UmiWorkbenchLayoutDiff *diff,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diff == NULL || index >= diff->entry_count) {
        return NULL;
    }
    return &diff->entries[index];
}

/*
 * Provide the workbench layout diff is empty operation used by this module and its client
 * applications.
 */
bool umi_workbench_layout_diff_is_empty(
    const UmiWorkbenchLayoutDiff *diff)
{
    return diff != NULL && diff->entry_count == 0U;
}

/*
 * Provide the workbench layout diff format operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_diff_format(
    const UmiWorkbenchLayoutDiff *diff,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    size_t index;
    size_t used = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diff == NULL || out_required == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < diff->entry_count; ++index) {
        const UmiWorkbenchLayoutDiffEntry *entry =
            &diff->entries[index];
        int written = snprintf(
            buffer != NULL && used < capacity
                ? buffer + used
                : NULL,
            buffer != NULL && used < capacity
                ? capacity - used
                : 0U,
            "%s%s%s: %s -> %s\n",
            umi_workbench_layout_diff_kind_text(entry->kind),
            entry->node_id[0] != '\0' ? " " : "",
            entry->node_id,
            entry->before_value,
            entry->after_value);
        /* Apply this branch only when its contract condition is satisfied. */
        if (written < 0) {
            return UMI_STATUS_INTERNAL_ERROR;
        }
        used += (size_t)written;
    }

    *out_required = used + 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || capacity < *out_required) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (buffer != NULL && capacity > 0U) {
            buffer[capacity - 1U] = '\0';
        }
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    buffer[used] = '\0';
    return UMI_STATUS_OK;
}
