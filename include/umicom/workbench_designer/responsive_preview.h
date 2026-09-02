/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/responsive_preview.h
 *
 * PURPOSE:
 *   Model desktop, tablet, mobile and custom preview profiles without binding
 *   the canonical layout to one frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_RESPONSIVE_PREVIEW_H
#define UMICOM_WORKBENCH_DESIGNER_RESPONSIVE_PREVIEW_H

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench designer preview device values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchDesignerPreviewDevice {
    UMI_WORKBENCH_DESIGNER_PREVIEW_DESKTOP = 1,
    UMI_WORKBENCH_DESIGNER_PREVIEW_LAPTOP = 2,
    UMI_WORKBENCH_DESIGNER_PREVIEW_TABLET = 3,
    UMI_WORKBENCH_DESIGNER_PREVIEW_MOBILE = 4,
    UMI_WORKBENCH_DESIGNER_PREVIEW_CUSTOM = 5
} UmiWorkbenchDesignerPreviewDevice;

/**
 * Represent the workbench designer responsive profile data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerResponsiveProfile {
    char profile_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    UmiWorkbenchDesignerPreviewDevice device;
    UmiWorkbenchDesignerSize viewport_size;
    double scale;
    bool portrait;
    bool hide_secondary_panels;
    bool collapse_sidebars;
} UmiWorkbenchDesignerResponsiveProfile;

/**
 * Represent the workbench designer responsive preview data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerResponsivePreview {
    UmiWorkbenchDesignerResponsiveProfile profiles[16U];
    size_t count;
    char active_profile_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerResponsivePreview;

/**
 * Initialise workbench designer responsive preview from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_responsive_preview_init(UmiWorkbenchDesignerResponsivePreview *preview);
/**
 * Provide the workbench designer responsive preview seed operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_responsive_preview_seed(UmiWorkbenchDesignerResponsivePreview *preview);
/**
 * Add workbench designer responsive preview only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_workbench_designer_responsive_preview_add(UmiWorkbenchDesignerResponsivePreview *preview, const UmiWorkbenchDesignerResponsiveProfile *profile);
/**
 * Provide the workbench designer responsive preview activate operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_designer_responsive_preview_activate(UmiWorkbenchDesignerResponsivePreview *preview, const char *profile_id);
/**
 * Provide the workbench designer responsive preview active operation used by this module
 * and its client applications.
 */
const UmiWorkbenchDesignerResponsiveProfile *umi_workbench_designer_responsive_preview_active(const UmiWorkbenchDesignerResponsivePreview *preview);
/**
 * Perform workbench designer responsive preview through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_workbench_designer_responsive_preview_apply(const UmiWorkbenchDesignerResponsiveProfile *profile, const UmiWorkbenchLayoutDocument *source, UmiWorkbenchLayoutDocument *out_document);

#ifdef __cplusplus
}
#endif

#endif
