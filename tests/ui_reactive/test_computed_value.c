/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_computed_value.c
 *
 * PURPOSE:
 *   Exercise the computed value reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/computed_value.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveComputedValue item; umi_ui_reactive_computed_value_init(&item); return umi_ui_reactive_computed_value_valid(&item) ? 0 : 1; }
