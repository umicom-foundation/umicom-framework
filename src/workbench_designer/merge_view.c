/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/merge_view.c
 *
 * PURPOSE:
 *   Keep merge review state explicit and transfer only validated resolutions to
 *   the semantic merge plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/merge_view.h"
#include "internal.h"


/*
 * Initialise workbench designer merge view from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_merge_view_init(UmiWorkbenchDesignerMergeView *view)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return;
    (void)memset(view, 0, sizeof(*view));
    view->selected_index = UMI_WORKBENCH_DESIGNER_INDEX_NONE;
}

/*
 * Provide the workbench designer merge view build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_merge_view_build(
    UmiWorkbenchDesignerMergeView *view,
    const UmiWorkbenchLayoutMergePlan *plan)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_merge_view_init(view);
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->conflict_count > UMI_WORKBENCH_LAYOUT_MAX_CONFLICTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->conflict_count; ++index) {
        const UmiWorkbenchLayoutMergeConflict *source = &plan->conflicts[index];
        UmiWorkbenchDesignerMergeRow *row = &view->rows[view->count++];
        (void)memset(row, 0, sizeof(*row));
        (void)umi_workbench_designer_copy_text(row->conflict_id, sizeof(row->conflict_id), source->conflict_id);
        (void)umi_workbench_designer_copy_text(row->node_id, sizeof(row->node_id), source->node_id);
        (void)umi_workbench_designer_copy_text(row->field, sizeof(row->field), source->field);
        (void)umi_workbench_designer_copy_text(row->base_value, sizeof(row->base_value), source->base_value);
        (void)umi_workbench_designer_copy_text(row->local_value, sizeof(row->local_value), source->local_value);
        (void)umi_workbench_designer_copy_text(row->remote_value, sizeof(row->remote_value), source->remote_value);
        (void)umi_workbench_designer_copy_text(row->custom_value, sizeof(row->custom_value), source->custom_value);
        row->kind = source->kind;
        row->resolution = source->resolution;
        /* Apply this branch only when its contract condition is satisfied. */
        if (row->resolution == UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED) view->unresolved_count += 1U;
    }
    view->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer merge view resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_merge_view_resolve(
    UmiWorkbenchDesignerMergeView *view,
    const char *conflict_id,
    UmiWorkbenchLayoutMergeResolution resolution,
    const char *custom_value)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || conflict_id == NULL ||
        resolution < UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED ||
        resolution > UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < view->count; ++index) {
        UmiWorkbenchDesignerMergeRow *row = &view->rows[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(row->conflict_id, conflict_id) != 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (row->resolution == UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED &&
            resolution != UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED) {
            view->unresolved_count -= 1U;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (row->resolution != UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED &&
                   resolution == UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED) {
            view->unresolved_count += 1U;
        }
        row->resolution = resolution;
        row->custom_value[0] = '\0';
        /* Apply this branch only when its contract condition is satisfied. */
        if (resolution == UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM) {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (custom_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (umi_workbench_designer_copy_text(row->custom_value, sizeof(row->custom_value), custom_value) != UMI_STATUS_OK) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }
        view->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Perform workbench designer merge view through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_workbench_designer_merge_view_apply(
    const UmiWorkbenchDesignerMergeView *view,
    UmiWorkbenchLayoutMergePlan *plan)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < view->count; ++index) {
        const UmiWorkbenchDesignerMergeRow *row = &view->rows[index];
        UmiStatus status = umi_workbench_layout_merge_resolve(
            plan, row->conflict_id, row->resolution,
            row->resolution == UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM
                ? row->custom_value : NULL);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Find workbench designer merge view while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWorkbenchDesignerMergeRow *umi_workbench_designer_merge_view_at(
    const UmiWorkbenchDesignerMergeView *view,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || index >= view->count) return NULL;
    return &view->rows[index];
}
