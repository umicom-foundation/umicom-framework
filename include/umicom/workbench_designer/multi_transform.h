/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/multi_transform.h
 *
 * PURPOSE:
 *   Apply bounded translation and proportional resize to multiple selected
 *   semantic nodes from one stable transform snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_MULTI_TRANSFORM_H
#define UMICOM_WORKBENCH_DESIGNER_MULTI_TRANSFORM_H

#include "umicom/workbench_designer/canvas.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerTransformItem {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchLayoutRect original_bounds;
    UmiWorkbenchLayoutRect resulting_bounds;
    bool changed;
} UmiWorkbenchDesignerTransformItem;

typedef struct UmiWorkbenchDesignerMultiTransform {
    UmiWorkbenchDesignerTransformItem items[UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS];
    size_t count;
    UmiWorkbenchDesignerRect original_selection_bounds;
    UmiWorkbenchDesignerRect resulting_selection_bounds;
    uint64_t source_revision;
    uint64_t revision;
} UmiWorkbenchDesignerMultiTransform;

void umi_workbench_designer_multi_transform_init(UmiWorkbenchDesignerMultiTransform *transform);
UmiStatus umi_workbench_designer_multi_transform_begin(UmiWorkbenchDesignerMultiTransform *transform, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerSelection *selection);
UmiStatus umi_workbench_designer_multi_transform_translate(UmiWorkbenchDesignerMultiTransform *transform, double delta_x, double delta_y);
UmiStatus umi_workbench_designer_multi_transform_resize(UmiWorkbenchDesignerMultiTransform *transform, UmiWorkbenchDesignerRect new_bounds, bool preserve_aspect);
UmiStatus umi_workbench_designer_multi_transform_apply(const UmiWorkbenchDesignerMultiTransform *transform, UmiWorkbenchLayoutDocument *document);

#ifdef __cplusplus
}
#endif

#endif
