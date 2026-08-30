/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/merge_view.h
 *
 * PURPOSE:
 *   Present three-way merge conflicts and collect explicit user resolutions
 *   without mutating source documents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_MERGE_VIEW_H
#define UMICOM_WORKBENCH_DESIGNER_MERGE_VIEW_H

#include "umicom/workbench_layout/merge.h"
#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerMergeRow {
    char conflict_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char field[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char base_value[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char local_value[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char remote_value[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char custom_value[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    UmiWorkbenchLayoutConflictKind kind;
    UmiWorkbenchLayoutMergeResolution resolution;
    bool selected;
} UmiWorkbenchDesignerMergeRow;

typedef struct UmiWorkbenchDesignerMergeView {
    UmiWorkbenchDesignerMergeRow rows[UMI_WORKBENCH_LAYOUT_MAX_CONFLICTS];
    size_t count;
    size_t unresolved_count;
    size_t selected_index;
    uint64_t revision;
} UmiWorkbenchDesignerMergeView;

void umi_workbench_designer_merge_view_init(UmiWorkbenchDesignerMergeView *view);
UmiStatus umi_workbench_designer_merge_view_build(UmiWorkbenchDesignerMergeView *view, const UmiWorkbenchLayoutMergePlan *plan);
UmiStatus umi_workbench_designer_merge_view_resolve(UmiWorkbenchDesignerMergeView *view, const char *conflict_id, UmiWorkbenchLayoutMergeResolution resolution, const char *custom_value);
UmiStatus umi_workbench_designer_merge_view_apply(const UmiWorkbenchDesignerMergeView *view, UmiWorkbenchLayoutMergePlan *plan);
const UmiWorkbenchDesignerMergeRow *umi_workbench_designer_merge_view_at(const UmiWorkbenchDesignerMergeView *view, size_t index);

#ifdef __cplusplus
}
#endif

#endif
