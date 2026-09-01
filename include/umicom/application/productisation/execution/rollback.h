/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/rollback.h
 *
 * PURPOSE:
 *   Plan compensation in reverse completion order for successfully delivered work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/** Clear a rollback plan before failed work is analysed. */
void umi_product_execution_rollback_plan_init(UmiProductExecutionRollbackPlan *plan);
/** Build reverse-order rollback entries for successfully mutated work. */
UmiStatus umi_product_execution_rollback_plan_build(
    UmiProductExecutionRollbackPlan *plan,
    const UmiProductExecutionWorkQueue *queue);
/** Borrow the next pending rollback entry in safe execution order. */
UmiStatus umi_product_execution_rollback_next(
    UmiProductExecutionRollbackPlan *plan,
    size_t *out_item_index);
/** Record the outcome and evidence produced by one rollback attempt. */
UmiStatus umi_product_execution_rollback_record(
    UmiProductExecutionRollbackPlan *plan,
    int succeeded);
#ifdef __cplusplus
}
#endif
#endif
