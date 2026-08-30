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
typedef struct UmiUiReactivePropagationResult {
    size_t propagated;
    size_t skipped;
    size_t failed;
    uint64_t generation;
} UmiUiReactivePropagationResult;
void umi_ui_reactive_propagation_result_init(UmiUiReactivePropagationResult *item);
int umi_ui_reactive_propagation_result_valid(const UmiUiReactivePropagationResult *item);
#ifdef __cplusplus
}
#endif
#endif
