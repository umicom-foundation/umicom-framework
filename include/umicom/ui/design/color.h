/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/color.h
 *
 * PURPOSE:
 *   Provide validated RGBA colour operations for renderer-neutral semantic styling.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_COLOR_H
#define INCLUDE_UMICOM_UI_DESIGN_COLOR_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Construct a clamped RGBA colour from normalized channel values. */
UmiStatus umi_design_color_make(double red, double green, double blue, double alpha, UmiDesignRgba *out_color);
/* Linearly mix two normalized colours using a [0,1] weight. */
UmiStatus umi_design_color_mix(UmiDesignRgba left, UmiDesignRgba right, double weight, UmiDesignRgba *out_color);
/* Return one when every RGBA channel lies in the normalized range. */
int umi_design_color_valid(UmiDesignRgba color);

#ifdef __cplusplus
}
#endif

#endif
