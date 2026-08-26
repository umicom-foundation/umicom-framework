/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/color.c
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

#include "umicom/ui/design/color.h"

int umi_design_color_valid(UmiDesignRgba color)
{
    return umi_design_number_valid(color.red) && umi_design_number_valid(color.green) && umi_design_number_valid(color.blue) && umi_design_number_valid(color.alpha) && color.red >= 0.0 && color.red <= 1.0 && color.green >= 0.0 && color.green <= 1.0 && color.blue >= 0.0 && color.blue <= 1.0 && color.alpha >= 0.0 && color.alpha <= 1.0;
}

UmiStatus umi_design_color_make(double red,double green,double blue,double alpha,UmiDesignRgba *out_color)
{
    if (out_color == NULL || !umi_design_number_valid(red) || !umi_design_number_valid(green) || !umi_design_number_valid(blue) || !umi_design_number_valid(alpha)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_design_clamp(red,0.0,1.0,&out_color->red) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_design_clamp(green,0.0,1.0,&out_color->green) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_design_clamp(blue,0.0,1.0,&out_color->blue) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_design_clamp(alpha,0.0,1.0,&out_color->alpha);
}

UmiStatus umi_design_color_mix(UmiDesignRgba left,UmiDesignRgba right,double weight,UmiDesignRgba *out_color)
{
    double w;
    if (out_color == NULL || !umi_design_color_valid(left) || !umi_design_color_valid(right)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_design_clamp(weight,0.0,1.0,&w) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    out_color->red=left.red+(right.red-left.red)*w; out_color->green=left.green+(right.green-left.green)*w;
    out_color->blue=left.blue+(right.blue-left.blue)*w; out_color->alpha=left.alpha+(right.alpha-left.alpha)*w;
    return UMI_STATUS_OK;
}
