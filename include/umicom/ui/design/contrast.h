/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/contrast.h
 *
 * PURPOSE:
 *   Calculate relative luminance and contrast ratios for accessible semantic colour combinations.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_CONTRAST_H
#define INCLUDE_UMICOM_UI_DESIGN_CONTRAST_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/color.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/color.h"
/* Calculate WCAG-style relative luminance for an opaque normalized colour. */
UmiStatus umi_design_contrast_luminance(UmiDesignRgba color, double *out_luminance);
/* Calculate the contrast ratio between two normalized colours. */
UmiStatus umi_design_contrast_ratio(UmiDesignRgba first, UmiDesignRgba second, double *out_ratio);
/* Return one when a contrast ratio meets a requested positive threshold. */
int umi_design_contrast_passes(double ratio, double threshold);

#ifdef __cplusplus
}
#endif

#endif
