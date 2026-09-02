/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_observable_store.c
 *
 * PURPOSE:
 *   Exercise the observable store reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/observable_store.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveObservableStore s;UmiUiValue v={0},out={0};v.kind=UMI_UI_VALUE_INTEGER;v.integer_value=42;umi_ui_reactive_observable_store_init(&s);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_reactive_observable_store_put(&s,"order.quantity",&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_reactive_observable_store_get(&s,"order.quantity",&out)!=UMI_STATUS_OK)return 2;return out.integer_value==42?0:3; }
