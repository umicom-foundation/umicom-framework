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


typedef enum UmiWorkbenchDesignerDiffSide {
    UMI_WORKBENCH_DESIGNER_DIFF_BOTH = 1,
    UMI_WORKBENCH_DESIGNER_DIFF_BEFORE_ONLY = 2,
    UMI_WORKBENCH_DESIGNER_DIFF_AFTER_ONLY = 3
} UmiWorkbenchDesignerDiffSide;

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

typedef struct UmiWorkbenchDesignerDiffView {
    UmiWorkbenchDesignerDiffRow rows[UMI_WORKBENCH_LAYOUT_MAX_DIFF_ENTRIES];
    size_t count;
    size_t selected_index;
    bool geometry_visible;
    bool unchanged_visible;
    uint64_t revision;
} UmiWorkbenchDesignerDiffView;

void umi_workbench_designer_diff_view_init(UmiWorkbenchDesignerDiffView *view);
UmiStatus umi_workbench_designer_diff_view_build(UmiWorkbenchDesignerDiffView *view, const UmiWorkbenchLayoutDiff *diff);
UmiStatus umi_workbench_designer_diff_view_select(UmiWorkbenchDesignerDiffView *view, size_t index);
const UmiWorkbenchDesignerDiffRow *umi_workbench_designer_diff_view_at(const UmiWorkbenchDesignerDiffView *view, size_t index);

#ifdef __cplusplus
}
#endif

#endif
