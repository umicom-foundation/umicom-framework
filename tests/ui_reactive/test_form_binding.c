/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_form_binding.c
 *
 * PURPOSE:
 *   Exercise the form binding reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/form_binding.h"
int main(void) { UmiUiReactiveFormBinding item; umi_ui_reactive_form_binding_init(&item); return umi_ui_reactive_form_binding_valid(&item) ? 0 : 1; }
