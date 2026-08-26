/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_physical_pixel_transform.c
 *
 * PURPOSE:
 *   Verify convert logical design measurements to physical device pixels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/physical_pixel_transform.h"
int main(void){UmiAppearancePhysicalPixelTransform t;double out=0.0;if(umi_appearance_physical_pixel_transform_init(&t,2.0)!=UMI_STATUS_OK)return 1;if(umi_appearance_physical_pixel_transform_convert(&t,100.0,&out)!=UMI_STATUS_OK||out!=200.0)return 2;return 0;}
