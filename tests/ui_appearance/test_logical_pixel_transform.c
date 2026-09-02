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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAppearanceLogicalPixelTransform t;double out=0.0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_logical_pixel_transform_init(&t,2.0)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_logical_pixel_transform_convert(&t,100.0,&out)!=UMI_STATUS_OK||out!=50.0)return 2;return 0;}
