/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_ui_state_service.c
 *
 * PURPOSE:
 *   Exercise the ui state service reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/ui_state_service.h"
int main(void) { UmiUiReactiveUiStateService item; umi_ui_reactive_ui_state_service_init(&item); return umi_ui_reactive_ui_state_service_valid(&item) ? 0 : 1; }
