/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/propagation_guard.h
 *
 * PURPOSE:
 *   Bound propagation depth to prevent runaway reactive graphs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_PROPAGATION_GUARD_H
#define UMICOM_UI_REACTIVE_PROPAGATION_GUARD_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactivePropagationGuard {
    size_t depth;
    size_t max_depth;
    bool blocked;
} UmiUiReactivePropagationGuard;
UmiStatus umi_ui_reactive_propagation_guard_enter(UmiUiReactivePropagationGuard *item);
void umi_ui_reactive_propagation_guard_leave(UmiUiReactivePropagationGuard *item);
#ifdef __cplusplus
}
#endif
#endif
