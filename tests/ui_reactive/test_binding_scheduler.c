/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_scheduler.c
 *
 * PURPOSE:
 *   Exercise the binding scheduler reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_scheduler.h"
int main(void) { UmiUiReactiveBindingScheduler item; umi_ui_reactive_binding_scheduler_init(&item); return umi_ui_reactive_binding_scheduler_valid(&item) ? 0 : 1; }
