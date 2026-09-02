/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/work_queue.h
 *
 * PURPOSE:
 *   Materialise a bounded execution queue directly from the canonical completion plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_WORK_QUEUE_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_WORK_QUEUE_H
#include "umicom/application/productisation/execution/work_item.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the product execution work queue data shared with callers of this public
 * contract.
 */
typedef struct UmiProductExecutionWorkQueue {
    UmiProductExecutionWorkItem items[UMI_PRODUCT_EXECUTION_MAX_ITEMS];
    size_t count;
} UmiProductExecutionWorkQueue;
/** Clear a bounded work queue before a completion plan is loaded. */
void umi_product_execution_work_queue_init(UmiProductExecutionWorkQueue *queue);
/** Convert completion-plan steps into owned execution work items. */
UmiStatus umi_product_execution_work_queue_from_plan(
    UmiProductExecutionWorkQueue *queue,
    const UmiProductisationCompletionPlan *plan,
    unsigned maximum_attempts);
/** Borrow a mutable work item by index for execution services. */
UmiProductExecutionWorkItem *umi_product_execution_work_queue_at(
    UmiProductExecutionWorkQueue *queue, size_t index);
/** Borrow a read-only work item by index for reports and user interfaces. */
const UmiProductExecutionWorkItem *umi_product_execution_work_queue_at_const(
    const UmiProductExecutionWorkQueue *queue, size_t index);
#ifdef __cplusplus
}
#endif
#endif
