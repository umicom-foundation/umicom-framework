/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_event_binding.c
 *
 * PURPOSE:
 *   Exercise the event binding reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/event_binding.h"
int main(void) { UmiUiReactiveEventBinding item; umi_ui_reactive_event_binding_init(&item); return umi_ui_reactive_event_binding_valid(&item) ? 0 : 1; }
