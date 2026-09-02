/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_interaction_state.c
 *
 * PURPOSE:
 *   Verify stable names for define mutually exclusive interaction states used for component style resolution..
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

#include "umicom/ui/design/interaction_state.h"

#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { return strcmp(umi_design_interaction_state_name(UMI_DESIGN_INTERACTION_INVALID),"invalid")==0 ? 0 : 1; }
