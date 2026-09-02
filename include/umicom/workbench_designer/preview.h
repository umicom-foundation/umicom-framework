/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/preview.h
 *
 * PURPOSE:
 *   Produce immutable preview snapshots used by visual, headless and future web
 *   renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PREVIEW_H
#define UMICOM_WORKBENCH_DESIGNER_PREVIEW_H

#include "umicom/workbench_designer/canvas.h"
#include "umicom/workbench_designer/responsive_preview.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer preview snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerPreviewSnapshot {
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerCanvas canvas;
    UmiWorkbenchDesignerResponsiveProfile profile;
    UmiWorkbenchDesignerSize viewport_size;
    size_t visible_item_count;
    size_t hidden_item_count;
    uint64_t source_revision;
    uint64_t content_hash;
} UmiWorkbenchDesignerPreviewSnapshot;

/**
 * Provide the workbench designer preview build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_preview_build(const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerResponsiveProfile *profile, UmiWorkbenchDesignerPreviewSnapshot *out_snapshot);
/**
 * Provide the workbench designer preview item operation used by this module and its client
 * applications.
 */
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_preview_item(const UmiWorkbenchDesignerPreviewSnapshot *snapshot, size_t index);

#ifdef __cplusplus
}
#endif

#endif
