/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_command_enablement.c
 *
 * PURPOSE:
 *   Exercise the command enablement reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_enablement.h"
int main(void) { UmiUiReactiveCommandEnablement item; umi_ui_reactive_command_enablement_init(&item); return umi_ui_reactive_command_enablement_valid(&item) ? 0 : 1; }
