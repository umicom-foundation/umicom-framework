/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_form_binding.c
 *
 * PURPOSE:
 *   Exercise the form binding reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/form_binding.h"
int main(void) { UmiUiReactiveFormBinding item; umi_ui_reactive_form_binding_init(&item); return umi_ui_reactive_form_binding_valid(&item) ? 0 : 1; }
