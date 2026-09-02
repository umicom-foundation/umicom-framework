/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_event_binding.c
 *
 * PURPOSE:
 *   Exercise the event binding reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/event_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveEventBinding item; umi_ui_reactive_event_binding_init(&item); return umi_ui_reactive_event_binding_valid(&item) ? 0 : 1; }
