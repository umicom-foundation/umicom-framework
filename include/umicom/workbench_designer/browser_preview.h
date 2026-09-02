/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/browser_preview.h
 *
 * PURPOSE:
 *   Create compact, immutable previews of stored layouts for the Layout Browser
 *   without loading frontend widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_BROWSER_PREVIEW_H
#define UMICOM_WORKBENCH_DESIGNER_BROWSER_PREVIEW_H

#include "umicom/workbench_designer/browser.h"
#include "umicom/workbench_designer/minimap.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer browser preview data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerBrowserPreview {
    char layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char category[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    size_t panel_count;
    size_t floating_window_count;
    size_t monitor_count;
    UmiWorkbenchDesignerMinimap minimap;
    bool available;
    bool stale;
    uint64_t layout_revision;
    uint64_t revision;
} UmiWorkbenchDesignerBrowserPreview;

/**
 * Initialise workbench designer browser preview from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_browser_preview_init(
    UmiWorkbenchDesignerBrowserPreview *preview);
/**
 * Provide the workbench designer browser preview build operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_browser_preview_build(
    UmiWorkbenchDesignerBrowserPreview *preview,
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchDesignerSize surface_size);

#ifdef __cplusplus
}
#endif

#endif
