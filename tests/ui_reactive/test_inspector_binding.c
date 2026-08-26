/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_inspector_binding.c
 *
 * PURPOSE:
 *   Exercise the inspector binding reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/inspector_binding.h"
int main(void) { UmiUiReactiveInspectorBinding item; umi_ui_reactive_inspector_binding_init(&item); return umi_ui_reactive_inspector_binding_valid(&item) ? 0 : 1; }
