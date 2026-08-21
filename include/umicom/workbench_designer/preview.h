/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/preview.h
 *
 * PURPOSE:
 *   Produce immutable preview snapshots used by visual, headless and future web
 *   renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PREVIEW_H
#define UMICOM_WORKBENCH_DESIGNER_PREVIEW_H

#include "umicom/workbench_designer/canvas.h"
#include "umicom/workbench_designer/responsive_preview.h"

#ifdef __cplusplus
extern "C" {
#endif


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

UmiStatus umi_workbench_designer_preview_build(const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerResponsiveProfile *profile, UmiWorkbenchDesignerPreviewSnapshot *out_snapshot);
const UmiWorkbenchDesignerCanvasItem *umi_workbench_designer_preview_item(const UmiWorkbenchDesignerPreviewSnapshot *snapshot, size_t index);

#ifdef __cplusplus
}
#endif

#endif
