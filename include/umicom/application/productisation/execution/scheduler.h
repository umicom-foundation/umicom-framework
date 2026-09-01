/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/scheduler.h
 *
 * PURPOSE:
 *   Select the next policy-compliant ready completion step deterministically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_SCHEDULER_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_SCHEDULER_H
#include "umicom/application/productisation/execution/dependency_graph.h"
#include "umicom/application/productisation/execution/policy.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Recalculate which queued items have all prerequisites satisfied. */
UmiStatus umi_product_execution_scheduler_refresh_ready(
    UmiProductExecutionWorkQueue *queue,
    const UmiProductExecutionDependencyGraph *graph,
    const UmiProductExecutionPolicy *policy);
/** Borrow the next ready item in deterministic stage and priority order. */
UmiStatus umi_product_execution_scheduler_next(
    UmiProductExecutionWorkQueue *queue,
    size_t *out_index);
#ifdef __cplusplus
}
#endif
#endif
