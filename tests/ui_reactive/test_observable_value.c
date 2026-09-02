/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_observable_value.c
 *
 * PURPOSE:
 *   Exercise the observable value reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/observable_value.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveObservableValue item; umi_ui_reactive_observable_value_init(&item); return umi_ui_reactive_observable_value_valid(&item) ? 0 : 1; }
