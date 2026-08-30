/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_catalogue.c
 *
 * PURPOSE:
 *   Exercise the binding catalogue reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_catalogue.h"
int main(void) { UmiUiReactiveBindingCatalogue c;umi_ui_reactive_binding_catalogue_init(&c);if(umi_ui_reactive_binding_catalogue_add(&c,"name")!=UMI_STATUS_OK)return 1;return umi_ui_reactive_binding_catalogue_contains(&c,"name")?0:2; }
