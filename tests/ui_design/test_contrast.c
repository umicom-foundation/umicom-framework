/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_contrast.c
 *
 * PURPOSE:
 *   Verify accessible contrast calculation for black and white.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/contrast.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignRgba a,b;double ratio=0.0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_color_make(0.0,0.0,0.0,1.0,&a)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_color_make(1.0,1.0,1.0,1.0,&b)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_contrast_ratio(a,b,&ratio)!=UMI_STATUS_OK)return 3;return umi_design_contrast_passes(ratio,7.0)?0:4;}
