/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_selection_binding.c
 *
 * PURPOSE:
 *   Exercise the selection binding reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/selection_binding.h"
int main(void) { UmiUiReactiveSelectionBinding item; umi_ui_reactive_selection_binding_init(&item); return umi_ui_reactive_selection_binding_valid(&item) ? 0 : 1; }
