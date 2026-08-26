/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_descriptor.c
 *
 * PURPOSE:
 *   Exercise the binding descriptor reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_descriptor.h"
int main(void) { UmiUiReactiveBindingDescriptor item; umi_ui_reactive_binding_descriptor_init(&item); return umi_ui_reactive_binding_descriptor_valid(&item) ? 0 : 1; }
