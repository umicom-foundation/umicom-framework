/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/propagation_plan.h
 *
 * PURPOSE:
 *   Describe ordered binding identifiers for one propagation pass.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_PROPAGATION_PLAN_H
#define UMICOM_UI_REACTIVE_PROPAGATION_PLAN_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive propagation plan data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactivePropagationPlan {
    char binding_ids[128][UMI_UI_REACTIVE_ID_CAPACITY];
    size_t count;
    uint64_t generation;
} UmiUiReactivePropagationPlan;
/**
 * Initialise ui reactive propagation plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_propagation_plan_init(UmiUiReactivePropagationPlan *item);
/**
 * Check that ui reactive propagation plan satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_propagation_plan_valid(const UmiUiReactivePropagationPlan *item);
#ifdef __cplusplus
}
#endif
#endif
