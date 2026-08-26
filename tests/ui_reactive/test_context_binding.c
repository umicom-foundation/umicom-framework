/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_context_binding.c
 *
 * PURPOSE:
 *   Exercise the context binding reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/context_binding.h"
int main(void) { UmiUiReactiveContextBinding item; umi_ui_reactive_context_binding_init(&item); return umi_ui_reactive_context_binding_valid(&item) ? 0 : 1; }
