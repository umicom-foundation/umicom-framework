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


/**
 * Represent the workbench designer transform item data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerTransformItem {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchLayoutRect original_bounds;
    UmiWorkbenchLayoutRect resulting_bounds;
    bool changed;
} UmiWorkbenchDesignerTransformItem;

/**
 * Represent the workbench designer multi transform data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerMultiTransform {
    UmiWorkbenchDesignerTransformItem items[UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS];
    size_t count;
    UmiWorkbenchDesignerRect original_selection_bounds;
    UmiWorkbenchDesignerRect resulting_selection_bounds;
    uint64_t source_revision;
    uint64_t revision;
} UmiWorkbenchDesignerMultiTransform;

/**
 * Initialise workbench designer multi transform from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_multi_transform_init(UmiWorkbenchDesignerMultiTransform *transform);
/**
 * Provide the workbench designer multi transform begin operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_multi_transform_begin(UmiWorkbenchDesignerMultiTransform *transform, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerSelection *selection);
/**
 * Provide the workbench designer multi transform translate operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_designer_multi_transform_translate(UmiWorkbenchDesignerMultiTransform *transform, double delta_x, double delta_y);
/**
 * Provide the workbench designer multi transform resize operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_multi_transform_resize(UmiWorkbenchDesignerMultiTransform *transform, UmiWorkbenchDesignerRect new_bounds, bool preserve_aspect);
/**
 * Perform workbench designer multi transform through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_workbench_designer_multi_transform_apply(const UmiWorkbenchDesignerMultiTransform *transform, UmiWorkbenchLayoutDocument *document);

#ifdef __cplusplus
}
#endif

#endif
