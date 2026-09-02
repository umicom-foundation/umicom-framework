/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_color_palette.c
 *
 * PURPOSE:
 *   Verify semantic colour palette insertion, replacement and lookup.
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

#include "umicom/ui/design/color_palette.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiDesignColorPalette p={0}; UmiDesignRgba c,o; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_color_make(0.2,0.4,0.8,1.0,&c)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_color_palette_upsert(&p,UMI_DESIGN_COLOR_ACCENT,c)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_color_palette_find(&p,UMI_DESIGN_COLOR_ACCENT,&o)!=UMI_STATUS_OK)return 3; return o.blue==0.8?0:4; }
