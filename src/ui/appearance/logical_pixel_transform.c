/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/logical_pixel_transform.c
 *
 * PURPOSE:
 *   Convert physical pixel measurements to renderer-neutral logical coordinates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/logical_pixel_transform.h"
#include <math.h>
UmiStatus umi_appearance_logical_pixel_transform_init(UmiAppearanceLogicalPixelTransform *item,double scale){if(item==NULL||!isfinite(scale)||scale<=0.0)return UMI_STATUS_INVALID_ARGUMENT;item->scale=scale;return UMI_STATUS_OK;}
UmiStatus umi_appearance_logical_pixel_transform_convert(const UmiAppearanceLogicalPixelTransform *item,double input,double *output){if(item==NULL||output==NULL||!isfinite(input)||item->scale<=0.0)return UMI_STATUS_INVALID_ARGUMENT;*output=input/item->scale;return UMI_STATUS_OK;}
