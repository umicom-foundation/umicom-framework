/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_command_binding.c
 *
 * PURPOSE:
 *   Exercise the command binding reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_binding.h"
int main(void) { UmiUiReactiveCommandBinding item; umi_ui_reactive_command_binding_init(&item); return umi_ui_reactive_command_binding_valid(&item) ? 0 : 1; }
