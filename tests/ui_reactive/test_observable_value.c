/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_observable_value.c
 *
 * PURPOSE:
 *   Exercise the observable value reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/observable_value.h"
int main(void) { UmiUiReactiveObservableValue item; umi_ui_reactive_observable_value_init(&item); return umi_ui_reactive_observable_value_valid(&item) ? 0 : 1; }
