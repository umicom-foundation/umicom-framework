/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/work_queue.h
 *
 * PURPOSE:
 *   Materialise a bounded execution queue directly from the canonical completion plan.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_WORK_QUEUE_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_WORK_QUEUE_H
#include "umicom/application/productisation/execution/work_item.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionWorkQueue {
    UmiProductExecutionWorkItem items[UMI_PRODUCT_EXECUTION_MAX_ITEMS];
    size_t count;
} UmiProductExecutionWorkQueue;
void umi_product_execution_work_queue_init(UmiProductExecutionWorkQueue *queue);
UmiStatus umi_product_execution_work_queue_from_plan(
    UmiProductExecutionWorkQueue *queue,
    const UmiProductisationCompletionPlan *plan,
    unsigned maximum_attempts);
UmiProductExecutionWorkItem *umi_product_execution_work_queue_at(
    UmiProductExecutionWorkQueue *queue, size_t index);
const UmiProductExecutionWorkItem *umi_product_execution_work_queue_at_const(
    const UmiProductExecutionWorkQueue *queue, size_t index);
#ifdef __cplusplus
}
#endif
#endif
