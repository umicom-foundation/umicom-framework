/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_direction.c
 *
 * PURPOSE:
 *   Exercise the binding direction reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_direction.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveBindingDirectionPolicy item; umi_ui_reactive_binding_direction_init(&item); return umi_ui_reactive_binding_direction_valid(&item) ? 0 : 1; }
