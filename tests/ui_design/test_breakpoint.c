/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_breakpoint.c
 *
 * PURPOSE:
 *   Verify inclusive responsive breakpoint matching.
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

#include "umicom/ui/design/breakpoint.h"

int main(void){UmiDesignBreakpoint b;if(umi_design_breakpoint_init(&b,"desktop",900,1439,UMI_DESIGN_SIZE_EXPANDED)!=UMI_STATUS_OK)return 1;return umi_design_breakpoint_matches(&b,1200)?0:2;}
