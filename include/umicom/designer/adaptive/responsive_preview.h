/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/responsive_preview.h
 *
 * PURPOSE:
 *   Describe one toolkit-neutral responsive preview target for designer rendering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_RESPONSIVE_PREVIEW_H
#define UMICOM_DESIGNER_ADAPTIVE_RESPONSIVE_PREVIEW_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerResponsivePreview {
    char preview_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    UmiAdaptiveViewport viewport;
    UmiAdaptiveInsets safe_area;
    UmiAdaptiveInputMode input_mode;
    UmiRadTargetKind renderer;
    UmiAdaptiveDisplayOrientation orientation;
} UmiDesignerResponsivePreview;
/* Initialise one responsive preview target from logical viewport and renderer state. */
UmiStatus umi_designer_responsive_preview_init(UmiDesignerResponsivePreview *preview,
                                               const char *preview_id,
                                               UmiAdaptiveViewport viewport,
                                               UmiAdaptiveInsets safe_area,
                                               UmiAdaptiveInputMode input_mode,
                                               UmiRadTargetKind renderer);

#ifdef __cplusplus
}
#endif
#endif
