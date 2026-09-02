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


/**
 * Represent the workbench designer merge row data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the workbench designer merge view data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerMergeView {
    UmiWorkbenchDesignerMergeRow rows[UMI_WORKBENCH_LAYOUT_MAX_CONFLICTS];
    size_t count;
    size_t unresolved_count;
    size_t selected_index;
    uint64_t revision;
} UmiWorkbenchDesignerMergeView;

/**
 * Initialise workbench designer merge view from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_merge_view_init(UmiWorkbenchDesignerMergeView *view);
/**
 * Provide the workbench designer merge view build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_merge_view_build(UmiWorkbenchDesignerMergeView *view, const UmiWorkbenchLayoutMergePlan *plan);
/**
 * Provide the workbench designer merge view resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_merge_view_resolve(UmiWorkbenchDesignerMergeView *view, const char *conflict_id, UmiWorkbenchLayoutMergeResolution resolution, const char *custom_value);
/**
 * Perform workbench designer merge view through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_workbench_designer_merge_view_apply(const UmiWorkbenchDesignerMergeView *view, UmiWorkbenchLayoutMergePlan *plan);
/**
 * Find workbench designer merge view while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWorkbenchDesignerMergeRow *umi_workbench_designer_merge_view_at(const UmiWorkbenchDesignerMergeView *view, size_t index);

#ifdef __cplusplus
}
#endif

#endif
