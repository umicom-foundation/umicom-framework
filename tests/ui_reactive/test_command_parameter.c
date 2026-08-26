/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_command_parameter.c
 *
 * PURPOSE:
 *   Exercise the command parameter reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_parameter.h"
int main(void) { UmiUiReactiveCommandParameter item; umi_ui_reactive_command_parameter_init(&item); return umi_ui_reactive_command_parameter_valid(&item) ? 0 : 1; }
