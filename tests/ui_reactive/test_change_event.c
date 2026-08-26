/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_change_event.c
 *
 * PURPOSE:
 *   Exercise the change event reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/change_event.h"
int main(void) { UmiUiReactiveChangeEvent item; umi_ui_reactive_change_event_init(&item); return umi_ui_reactive_change_event_valid(&item) ? 0 : 1; }
