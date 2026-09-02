/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_color.c
 *
 * PURPOSE:
 *   Verify normalized colour validation and deterministic colour mixing.
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

#include "umicom/ui/design/color.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignRgba a,b,m;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_color_make(0.0,0.0,0.0,1.0,&a) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_color_make(1.0,1.0,1.0,1.0,&b) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_color_mix(a,b,0.5,&m) != UMI_STATUS_OK) return 3;
    return (m.red==0.5 && m.green==0.5 && m.blue==0.5) ? 0 : 4;
}
