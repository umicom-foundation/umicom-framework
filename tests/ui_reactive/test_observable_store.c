/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_observable_store.c
 *
 * PURPOSE:
 *   Exercise the observable store reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/observable_store.h"
int main(void) { UmiUiReactiveObservableStore s;UmiUiValue v={0},out={0};v.kind=UMI_UI_VALUE_INTEGER;v.integer_value=42;umi_ui_reactive_observable_store_init(&s);if(umi_ui_reactive_observable_store_put(&s,"order.quantity",&v)!=UMI_STATUS_OK)return 1;if(umi_ui_reactive_observable_store_get(&s,"order.quantity",&out)!=UMI_STATUS_OK)return 2;return out.integer_value==42?0:3; }
