/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_physical_pixel_transform.c
 *
 * PURPOSE:
 *   Verify convert logical design measurements to physical device pixels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/physical_pixel_transform.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAppearancePhysicalPixelTransform t;double out=0.0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_physical_pixel_transform_init(&t,2.0)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_physical_pixel_transform_convert(&t,100.0,&out)!=UMI_STATUS_OK||out!=200.0)return 2;return 0;}
