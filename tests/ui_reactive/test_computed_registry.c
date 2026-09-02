/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_computed_registry.c
 *
 * PURPOSE:
 *   Exercise the computed registry reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/computed_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveComputedRegistry item; umi_ui_reactive_computed_registry_init(&item); return umi_ui_reactive_computed_registry_valid(&item) ? 0 : 1; }
