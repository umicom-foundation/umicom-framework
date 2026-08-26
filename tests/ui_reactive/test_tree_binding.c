/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_tree_binding.c
 *
 * PURPOSE:
 *   Exercise the tree binding reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/tree_binding.h"
int main(void) { UmiUiReactiveTreeBinding item; umi_ui_reactive_tree_binding_init(&item); return umi_ui_reactive_tree_binding_valid(&item) ? 0 : 1; }
