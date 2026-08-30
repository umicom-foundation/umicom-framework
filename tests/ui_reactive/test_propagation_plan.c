/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_propagation_plan.c
 *
 * PURPOSE:
 *   Exercise the propagation plan reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/propagation_plan.h"
int main(void) { UmiUiReactivePropagationPlan item; umi_ui_reactive_propagation_plan_init(&item); return umi_ui_reactive_propagation_plan_valid(&item) ? 0 : 1; }
