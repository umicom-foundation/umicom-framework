/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/adapter.c
 *
 * PURPOSE:
 *   Dispatch completion work through stage-specific host callbacks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/adapter.h"
UmiStatus umi_product_execution_adapter_validate(
    const UmiProductExecutionAdapter *adapter)
{
    if (adapter == NULL || adapter->framework == NULL ||
        adapter->application == NULL || adapter->acceptance == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
UmiStatus umi_product_execution_adapter_invoke(
    const UmiProductExecutionAdapter *adapter,
    const UmiProductExecutionWorkItem *item,
    char *evidence,
    size_t evidence_capacity,
    UmiProductExecutionOutcome *out_outcome)
{
    UmiProductExecutionInvoke invoke;
    if (umi_product_execution_adapter_validate(adapter) != UMI_STATUS_OK ||
        item == NULL || evidence == NULL || evidence_capacity == 0U ||
        out_outcome == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (item->step.stage == UMI_PRODUCTISATION_STAGE_FRAMEWORK) invoke = adapter->framework;
    else if (item->step.stage == UMI_PRODUCTISATION_STAGE_ACCEPTANCE) invoke = adapter->acceptance;
    else if (item->step.owner == UMI_PRODUCTISATION_OWNER_EXTERNAL_ADAPTER) {
        invoke = adapter->external_adapter;
    } else invoke = adapter->application;
    if (invoke == NULL) return UMI_STATUS_UNAVAILABLE;
    return invoke(adapter->context, item, evidence, evidence_capacity, out_outcome);
}
