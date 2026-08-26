/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/accessibility_scale.h
 *
 * PURPOSE:
 *   Combine user text, target-size and contrast preferences for adaptive design previews.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_ACCESSIBILITY_SCALE_H
#define UMICOM_DESIGNER_ADAPTIVE_ACCESSIBILITY_SCALE_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerAccessibilityScale {
    double text_scale;
    double target_scale;
    int high_contrast;
} UmiDesignerAccessibilityScale;
/* Initialise bounded accessibility scaling for a designer preview. */
UmiStatus umi_designer_accessibility_scale_init(UmiDesignerAccessibilityScale *scale,
                                                double text_scale,
                                                double target_scale,
                                                int high_contrast);
/* Apply target scaling to one positive logical control extent. */
int32_t umi_designer_accessibility_scale_target(const UmiDesignerAccessibilityScale *scale,
                                                int32_t logical_extent);

#ifdef __cplusplus
}
#endif
#endif
