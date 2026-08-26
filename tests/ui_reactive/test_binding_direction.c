/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_direction.c
 *
 * PURPOSE:
 *   Exercise the binding direction reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_direction.h"
int main(void) { UmiUiReactiveBindingDirectionPolicy item; umi_ui_reactive_binding_direction_init(&item); return umi_ui_reactive_binding_direction_valid(&item) ? 0 : 1; }
