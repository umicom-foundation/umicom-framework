/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_scheduler.h
 *
 * PURPOSE:
 *   Track pending propagation work and deterministic dispatch generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_SCHEDULER_H
#define UMICOM_UI_REACTIVE_BINDING_SCHEDULER_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive binding scheduler data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveBindingScheduler {
    size_t pending;
    uint64_t generation;
    bool suspended;
} UmiUiReactiveBindingScheduler;
/**
 * Initialise ui reactive binding scheduler from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_binding_scheduler_init(UmiUiReactiveBindingScheduler *item);
/**
 * Check that ui reactive binding scheduler satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_binding_scheduler_valid(const UmiUiReactiveBindingScheduler *item);
#ifdef __cplusplus
}
#endif
#endif
