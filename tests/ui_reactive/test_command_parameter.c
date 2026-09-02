/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_command_parameter.c
 *
 * PURPOSE:
 *   Exercise the command parameter reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_parameter.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveCommandParameter item; umi_ui_reactive_command_parameter_init(&item); return umi_ui_reactive_command_parameter_valid(&item) ? 0 : 1; }
