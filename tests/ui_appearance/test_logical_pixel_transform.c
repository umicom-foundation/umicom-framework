/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_logical_pixel_transform.c
 *
 * PURPOSE:
 *   Verify convert physical pixel measurements to renderer-neutral logical coordinates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/logical_pixel_transform.h"
int main(void){UmiAppearanceLogicalPixelTransform t;double out=0.0;if(umi_appearance_logical_pixel_transform_init(&t,2.0)!=UMI_STATUS_OK)return 1;if(umi_appearance_logical_pixel_transform_convert(&t,100.0,&out)!=UMI_STATUS_OK||out!=50.0)return 2;return 0;}
