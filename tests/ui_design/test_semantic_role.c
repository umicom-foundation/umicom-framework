/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_semantic_role.c
 *
 * PURPOSE:
 *   Verify stable names for define visual intent roles shared by controls, analytics surfaces and application brands..
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

#include "umicom/ui/design/semantic_role.h"

#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { return strcmp(umi_design_semantic_role_name(UMI_DESIGN_ROLE_ACCENT),"accent")==0 ? 0 : 1; }
