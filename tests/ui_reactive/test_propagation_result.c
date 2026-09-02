/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_propagation_result.c
 *
 * PURPOSE:
 *   Exercise the propagation result reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/propagation_result.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactivePropagationResult item; umi_ui_reactive_propagation_result_init(&item); return umi_ui_reactive_propagation_result_valid(&item) ? 0 : 1; }
