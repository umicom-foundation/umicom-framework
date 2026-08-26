/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/contrast.c
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

#include "umicom/ui/design/contrast.h"

#include <math.h>
static double linear_channel(double c){return c<=0.04045?c/12.92:pow((c+0.055)/1.055,2.4);}
UmiStatus umi_design_contrast_luminance(UmiDesignRgba color,double *out_luminance){if(out_luminance==NULL||!umi_design_color_valid(color))return UMI_STATUS_INVALID_ARGUMENT;*out_luminance=0.2126*linear_channel(color.red)+0.7152*linear_channel(color.green)+0.0722*linear_channel(color.blue);return UMI_STATUS_OK;}
UmiStatus umi_design_contrast_ratio(UmiDesignRgba first,UmiDesignRgba second,double *out_ratio){double a,b,hi,lo;if(out_ratio==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_design_contrast_luminance(first,&a)!=UMI_STATUS_OK||umi_design_contrast_luminance(second,&b)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;hi=a>b?a:b;lo=a>b?b:a;*out_ratio=(hi+0.05)/(lo+0.05);return UMI_STATUS_OK;}
int umi_design_contrast_passes(double ratio,double threshold){return umi_design_number_valid(ratio)&&umi_design_number_valid(threshold)&&threshold>0.0&&ratio>=threshold?1:0;}
