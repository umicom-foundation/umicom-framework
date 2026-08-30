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
typedef struct UmiUiReactivePropagationPlan {
    char binding_ids[128][UMI_UI_REACTIVE_ID_CAPACITY];
    size_t count;
    uint64_t generation;
} UmiUiReactivePropagationPlan;
void umi_ui_reactive_propagation_plan_init(UmiUiReactivePropagationPlan *item);
int umi_ui_reactive_propagation_plan_valid(const UmiUiReactivePropagationPlan *item);
#ifdef __cplusplus
}
#endif
#endif
