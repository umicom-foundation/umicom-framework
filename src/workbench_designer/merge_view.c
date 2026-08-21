/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/merge_view.c
 *
 * PURPOSE:
 *   Keep merge review state explicit and transfer only validated resolutions to
 *   the semantic merge plan.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/merge_view.h"
#include "internal.h"


void umi_workbench_designer_merge_view_init(UmiWorkbenchDesignerMergeView *view)
{
    if (view == NULL) return;
    (void)memset(view, 0, sizeof(*view));
    view->selected_index = UMI_WORKBENCH_DESIGNER_INDEX_NONE;
}

UmiStatus umi_workbench_designer_merge_view_build(
    UmiWorkbenchDesignerMergeView *view,
    const UmiWorkbenchLayoutMergePlan *plan)
{
    size_t index;
    if (view == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_merge_view_init(view);
    if (plan->conflict_count > UMI_WORKBENCH_LAYOUT_MAX_CONFLICTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
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
        if (row->resolution == UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED) view->unresolved_count += 1U;
    }
    view->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_merge_view_resolve(
    UmiWorkbenchDesignerMergeView *view,
    const char *conflict_id,
    UmiWorkbenchLayoutMergeResolution resolution,
    const char *custom_value)
{
    size_t index;
    if (view == NULL || conflict_id == NULL ||
        resolution < UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED ||
        resolution > UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < view->count; ++index) {
        UmiWorkbenchDesignerMergeRow *row = &view->rows[index];
        if (strcmp(row->conflict_id, conflict_id) != 0) continue;
        if (row->resolution == UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED &&
            resolution != UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED) {
            view->unresolved_count -= 1U;
        } else if (row->resolution != UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED &&
                   resolution == UMI_WORKBENCH_LAYOUT_MERGE_UNRESOLVED) {
            view->unresolved_count += 1U;
        }
        row->resolution = resolution;
        row->custom_value[0] = '\0';
        if (resolution == UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM) {
            if (custom_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
            if (umi_workbench_designer_copy_text(row->custom_value, sizeof(row->custom_value), custom_value) != UMI_STATUS_OK) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }
        view->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_workbench_designer_merge_view_apply(
    const UmiWorkbenchDesignerMergeView *view,
    UmiWorkbenchLayoutMergePlan *plan)
{
    size_t index;
    if (view == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < view->count; ++index) {
        const UmiWorkbenchDesignerMergeRow *row = &view->rows[index];
        UmiStatus status = umi_workbench_layout_merge_resolve(
            plan, row->conflict_id, row->resolution,
            row->resolution == UMI_WORKBENCH_LAYOUT_MERGE_USE_CUSTOM
                ? row->custom_value : NULL);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

const UmiWorkbenchDesignerMergeRow *umi_workbench_designer_merge_view_at(
    const UmiWorkbenchDesignerMergeView *view,
    size_t index)
{
    if (view == NULL || index >= view->count) return NULL;
    return &view->rows[index];
}
