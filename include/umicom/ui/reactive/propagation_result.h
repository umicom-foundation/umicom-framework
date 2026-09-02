/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/propagation_result.h
 *
 * PURPOSE:
 *   Summarise propagated, skipped and failed binding operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_PROPAGATION_RESULT_H
#define UMICOM_UI_REACTIVE_PROPAGATION_RESULT_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive propagation result data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactivePropagationResult {
    size_t propagated;
    size_t skipped;
    size_t failed;
    uint64_t generation;
} UmiUiReactivePropagationResult;
/**
 * Initialise ui reactive propagation result from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_propagation_result_init(UmiUiReactivePropagationResult *item);
/**
 * Check that ui reactive propagation result satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_propagation_result_valid(const UmiUiReactivePropagationResult *item);
#ifdef __cplusplus
}
#endif
#endif
