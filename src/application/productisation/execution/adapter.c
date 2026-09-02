/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/adapter.c
 *
 * PURPOSE:
 *   Dispatch completion work through stage-specific host callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/adapter.h"
/*
 * Check that product execution adapter satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_product_execution_adapter_validate(
    const UmiProductExecutionAdapter *adapter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->framework == NULL ||
        adapter->application == NULL || adapter->acceptance == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the product execution adapter invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_adapter_invoke(
    const UmiProductExecutionAdapter *adapter,
    const UmiProductExecutionWorkItem *item,
    char *evidence,
    size_t evidence_capacity,
    UmiProductExecutionOutcome *out_outcome)
{
    UmiProductExecutionInvoke invoke;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_product_execution_adapter_validate(adapter) != UMI_STATUS_OK ||
        item == NULL || evidence == NULL || evidence_capacity == 0U ||
        out_outcome == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (item->step.stage == UMI_PRODUCTISATION_STAGE_FRAMEWORK) invoke = adapter->framework;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (item->step.stage == UMI_PRODUCTISATION_STAGE_ACCEPTANCE) invoke = adapter->acceptance;
    else /* Apply this branch only when its contract condition is satisfied. */ if (item->step.owner == UMI_PRODUCTISATION_OWNER_EXTERNAL_ADAPTER) {
        invoke = adapter->external_adapter;
    } /* Use this fallback path when the earlier condition does not apply. */ else invoke = adapter->application;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (invoke == NULL) return UMI_STATUS_UNAVAILABLE;
    return invoke(adapter->context, item, evidence, evidence_capacity, out_outcome);
}
