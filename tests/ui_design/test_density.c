/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_density.c
 *
 * PURPOSE:
 *   Verify stable names for define semantic ui density profiles shared across touch, desktop and high-density workstations..
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

#include "umicom/ui/design/density.h"

#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { return strcmp(umi_design_density_name(UMI_DESIGN_DENSITY_TOUCH),"touch")==0 ? 0 : 1; }
