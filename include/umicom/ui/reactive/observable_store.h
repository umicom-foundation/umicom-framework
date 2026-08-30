/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/observable_store.h
 *
 * PURPOSE:
 *   Hold bounded revisioned UI state keyed by canonical property paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_OBSERVABLE_STORE_H
#define UMICOM_UI_REACTIVE_OBSERVABLE_STORE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveObservableStore {
    UmiUiReactiveKeyValue items[UMI_UI_REACTIVE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiUiReactiveObservableStore;
void umi_ui_reactive_observable_store_init(UmiUiReactiveObservableStore *item);
UmiStatus umi_ui_reactive_observable_store_put(UmiUiReactiveObservableStore *item,const char *key,const UmiUiValue *value);
UmiStatus umi_ui_reactive_observable_store_get(const UmiUiReactiveObservableStore *item,const char *key,UmiUiValue *out_value);
#ifdef __cplusplus
}
#endif
#endif
