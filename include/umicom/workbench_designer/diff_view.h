/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/diff_view.h
 *
 * PURPOSE:
 *   Project semantic layout differences into rows suitable for side-by-side and
 *   Beyond Compare-style review surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_DIFF_VIEW_H
#define UMICOM_WORKBENCH_DESIGNER_DIFF_VIEW_H

#include "umicom/workbench_layout/diff.h"
#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench designer diff side values accepted by this public contract.
 */
typedef enum UmiWorkbenchDesignerDiffSide {
    UMI_WORKBENCH_DESIGNER_DIFF_BOTH = 1,
    UMI_WORKBENCH_DESIGNER_DIFF_BEFORE_ONLY = 2,
    UMI_WORKBENCH_DESIGNER_DIFF_AFTER_ONLY = 3
} UmiWorkbenchDesignerDiffSide;

/**
 * Represent the workbench designer diff row data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerDiffRow {
    char row_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char field[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char before_value[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char after_value[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char summary[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    UmiWorkbenchLayoutDiffKind kind;
    UmiWorkbenchDesignerDiffSide side;
    bool selected;
    bool expanded;
} UmiWorkbenchDesignerDiffRow;

/**
 * Represent the workbench designer diff view data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerDiffView {
    UmiWorkbenchDesignerDiffRow rows[UMI_WORKBENCH_LAYOUT_MAX_DIFF_ENTRIES];
    size_t count;
    size_t selected_index;
    bool geometry_visible;
    bool unchanged_visible;
    uint64_t revision;
} UmiWorkbenchDesignerDiffView;

/**
 * Initialise workbench designer diff view from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_diff_view_init(UmiWorkbenchDesignerDiffView *view);
/**
 * Provide the workbench designer diff view build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_diff_view_build(UmiWorkbenchDesignerDiffView *view, const UmiWorkbenchLayoutDiff *diff);
/**
 * Provide the workbench designer diff view select operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_diff_view_select(UmiWorkbenchDesignerDiffView *view, size_t index);
/**
 * Find workbench designer diff view while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWorkbenchDesignerDiffRow *umi_workbench_designer_diff_view_at(const UmiWorkbenchDesignerDiffView *view, size_t index);

#ifdef __cplusplus
}
#endif

#endif
