/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_list_binding.c
 *
 * PURPOSE:
 *   Exercise the list binding reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/list_binding.h"
int main(void) { UmiUiReactiveListBinding item; umi_ui_reactive_list_binding_init(&item); return umi_ui_reactive_list_binding_valid(&item) ? 0 : 1; }
