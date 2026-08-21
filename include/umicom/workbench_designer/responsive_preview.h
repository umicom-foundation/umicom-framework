/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/responsive_preview.h
 *
 * PURPOSE:
 *   Model desktop, tablet, mobile and custom preview profiles without binding
 *   the canonical layout to one frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_RESPONSIVE_PREVIEW_H
#define UMICOM_WORKBENCH_DESIGNER_RESPONSIVE_PREVIEW_H

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerPreviewDevice {
    UMI_WORKBENCH_DESIGNER_PREVIEW_DESKTOP = 1,
    UMI_WORKBENCH_DESIGNER_PREVIEW_LAPTOP = 2,
    UMI_WORKBENCH_DESIGNER_PREVIEW_TABLET = 3,
    UMI_WORKBENCH_DESIGNER_PREVIEW_MOBILE = 4,
    UMI_WORKBENCH_DESIGNER_PREVIEW_CUSTOM = 5
} UmiWorkbenchDesignerPreviewDevice;

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

typedef struct UmiWorkbenchDesignerResponsivePreview {
    UmiWorkbenchDesignerResponsiveProfile profiles[16U];
    size_t count;
    char active_profile_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerResponsivePreview;

void umi_workbench_designer_responsive_preview_init(UmiWorkbenchDesignerResponsivePreview *preview);
UmiStatus umi_workbench_designer_responsive_preview_seed(UmiWorkbenchDesignerResponsivePreview *preview);
UmiStatus umi_workbench_designer_responsive_preview_add(UmiWorkbenchDesignerResponsivePreview *preview, const UmiWorkbenchDesignerResponsiveProfile *profile);
UmiStatus umi_workbench_designer_responsive_preview_activate(UmiWorkbenchDesignerResponsivePreview *preview, const char *profile_id);
const UmiWorkbenchDesignerResponsiveProfile *umi_workbench_designer_responsive_preview_active(const UmiWorkbenchDesignerResponsivePreview *preview);
UmiStatus umi_workbench_designer_responsive_preview_apply(const UmiWorkbenchDesignerResponsiveProfile *profile, const UmiWorkbenchLayoutDocument *source, UmiWorkbenchLayoutDocument *out_document);

#ifdef __cplusplus
}
#endif

#endif
