/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/observable_value.h
 *
 * PURPOSE:
 *   Hold a revisioned value and dirty state for reactive propagation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_OBSERVABLE_VALUE_H
#define UMICOM_UI_REACTIVE_OBSERVABLE_VALUE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveObservableValue {
    UmiUiValue value;
    uint64_t revision;
    bool dirty;
} UmiUiReactiveObservableValue;
void umi_ui_reactive_observable_value_init(UmiUiReactiveObservableValue *item);
int umi_ui_reactive_observable_value_valid(const UmiUiReactiveObservableValue *item);
#ifdef __cplusplus
}
#endif
#endif
