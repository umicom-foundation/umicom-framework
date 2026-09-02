/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_ui_state_service.c
 *
 * PURPOSE:
 *   Exercise the ui state service reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/ui_state_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveUiStateService item; umi_ui_reactive_ui_state_service_init(&item); return umi_ui_reactive_ui_state_service_valid(&item) ? 0 : 1; }
