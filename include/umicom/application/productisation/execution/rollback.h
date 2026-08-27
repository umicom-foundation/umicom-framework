/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/rollback.h
 *
 * PURPOSE:
 *   Plan compensation in reverse completion order for successfully delivered work.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_ROLLBACK_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_ROLLBACK_H
#include "umicom/application/productisation/execution/work_queue.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionRollbackEntry {
    size_t item_index;
    int completed;
} UmiProductExecutionRollbackEntry;
typedef struct UmiProductExecutionRollbackPlan {
    UmiProductExecutionRollbackEntry entries[UMI_PRODUCT_EXECUTION_MAX_ROLLBACKS];
    size_t count;
    size_t cursor;
} UmiProductExecutionRollbackPlan;
void umi_product_execution_rollback_plan_init(UmiProductExecutionRollbackPlan *plan);
UmiStatus umi_product_execution_rollback_plan_build(
    UmiProductExecutionRollbackPlan *plan,
    const UmiProductExecutionWorkQueue *queue);
UmiStatus umi_product_execution_rollback_next(
    UmiProductExecutionRollbackPlan *plan,
    size_t *out_item_index);
UmiStatus umi_product_execution_rollback_record(
    UmiProductExecutionRollbackPlan *plan,
    int succeeded);
#ifdef __cplusplus
}
#endif
#endif
