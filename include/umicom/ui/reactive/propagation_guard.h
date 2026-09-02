/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/propagation_guard.h
 *
 * PURPOSE:
 *   Bound propagation depth to prevent runaway reactive graphs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_PROPAGATION_GUARD_H
#define UMICOM_UI_REACTIVE_PROPAGATION_GUARD_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive propagation guard data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactivePropagationGuard {
    size_t depth;
    size_t max_depth;
    bool blocked;
} UmiUiReactivePropagationGuard;
/**
 * Provide the ui reactive propagation guard enter operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_reactive_propagation_guard_enter(UmiUiReactivePropagationGuard *item);
/**
 * Provide the ui reactive propagation guard leave operation used by this module and its
 * client applications.
 */
void umi_ui_reactive_propagation_guard_leave(UmiUiReactivePropagationGuard *item);
#ifdef __cplusplus
}
#endif
#endif
