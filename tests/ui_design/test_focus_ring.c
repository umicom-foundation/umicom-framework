/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_focus_ring.c
 *
 * PURPOSE:
 *   Verify accessible focus-ring geometry validation.
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

#include "umicom/ui/design/focus_ring.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDesignFocusRing r;return umi_design_focus_ring_init(&r,2.0,2.0,UMI_DESIGN_COLOR_ACCENT,1.0)==UMI_STATUS_OK?0:1;}
