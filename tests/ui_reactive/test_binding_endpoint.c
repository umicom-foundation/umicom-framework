/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_endpoint.c
 *
 * PURPOSE:
 *   Exercise the binding endpoint reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_endpoint.h"
int main(void) { UmiUiReactiveBindingEndpoint item; umi_ui_reactive_binding_endpoint_init(&item); return umi_ui_reactive_binding_endpoint_valid(&item) ? 0 : 1; }
