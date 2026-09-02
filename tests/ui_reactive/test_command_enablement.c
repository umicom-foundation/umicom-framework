/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_command_enablement.c
 *
 * PURPOSE:
 *   Exercise the command enablement reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_enablement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveCommandEnablement item; umi_ui_reactive_command_enablement_init(&item); return umi_ui_reactive_command_enablement_valid(&item) ? 0 : 1; }
