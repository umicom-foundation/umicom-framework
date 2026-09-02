/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/work_item.h
 *
 * PURPOSE:
 *   Represent one immutable completion-plan step plus execution state and evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_WORK_ITEM_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_WORK_ITEM_H

#include "umicom/application/productisation/completion_plan.h"
#include "umicom/application/productisation/execution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the product execution work item data shared with callers of this public
 * contract.
 */
typedef struct UmiProductExecutionWorkItem {
    size_t plan_index;
    UmiProductisationCompletionStep step;
    UmiProductExecutionState state;
    UmiProductExecutionOutcome outcome;
    unsigned attempts;
    unsigned maximum_attempts;
    char evidence_reference[UMI_PRODUCTISATION_REFERENCE_CAPACITY];
    char detail[UMI_PRODUCT_EXECUTION_DETAIL_CAPACITY];
} UmiProductExecutionWorkItem;

/** Initialise one work item from a completion-plan step and stable index. */
UmiStatus umi_product_execution_work_item_init(
    UmiProductExecutionWorkItem *item,
    size_t plan_index,
    const UmiProductisationCompletionStep *step,
    unsigned maximum_attempts);
/** Apply one allowed lifecycle transition and update timing evidence. */
UmiStatus umi_product_execution_work_item_transition(
    UmiProductExecutionWorkItem *item,
    UmiProductExecutionState next);
/** Return non-zero when policy and attempt count allow another execution. */
int umi_product_execution_work_item_retryable(
    const UmiProductExecutionWorkItem *item);

#ifdef __cplusplus
}
#endif

#endif
