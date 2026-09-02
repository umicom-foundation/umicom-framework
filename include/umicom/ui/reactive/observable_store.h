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
/**
 * Represent the ui reactive observable store data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveObservableStore {
    UmiUiReactiveKeyValue items[UMI_UI_REACTIVE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiUiReactiveObservableStore;
/**
 * Initialise ui reactive observable store from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_observable_store_init(UmiUiReactiveObservableStore *item);
/**
 * Provide the ui reactive observable store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_reactive_observable_store_put(UmiUiReactiveObservableStore *item,const char *key,const UmiUiValue *value);
/**
 * Provide the ui reactive observable store get operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_reactive_observable_store_get(const UmiUiReactiveObservableStore *item,const char *key,UmiUiValue *out_value);
#ifdef __cplusplus
}
#endif
#endif
