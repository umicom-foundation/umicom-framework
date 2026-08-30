/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/physical_pixel_transform.c
 *
 * PURPOSE:
 *   Convert logical design measurements to physical device pixels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/physical_pixel_transform.h"
#include <math.h>
UmiStatus umi_appearance_physical_pixel_transform_init(UmiAppearancePhysicalPixelTransform *item,double scale){if(item==NULL||!isfinite(scale)||scale<=0.0)return UMI_STATUS_INVALID_ARGUMENT;item->scale=scale;return UMI_STATUS_OK;}
UmiStatus umi_appearance_physical_pixel_transform_convert(const UmiAppearancePhysicalPixelTransform *item,double input,double *output){if(item==NULL||output==NULL||!isfinite(input)||item->scale<=0.0)return UMI_STATUS_INVALID_ARGUMENT;*output=input*item->scale;return UMI_STATUS_OK;}
