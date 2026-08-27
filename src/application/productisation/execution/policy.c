/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/policy.c
 *
 * PURPOSE:
 *   Enforce Framework-first completion and explicit opt-in boundaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/policy.h"
UmiProductExecutionPolicy umi_product_execution_policy_default(void)
{
    UmiProductExecutionPolicy p;
    p.maximum_attempts = 3U;
    p.dry_run = 0;
    p.allow_application_adoption = 1;
    p.allow_external_adapters = 0;
    p.require_framework_completion_first = 1;
    p.require_acceptance_after_delivery = 1;
    return p;
}
UmiStatus umi_product_execution_policy_validate(
    const UmiProductExecutionPolicy *policy)
{
    if (policy == NULL || policy->maximum_attempts == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
UmiStatus umi_product_execution_policy_check_item(
    const UmiProductExecutionPolicy *policy,
    const UmiProductExecutionWorkQueue *queue,
    size_t item_index)
{
    size_t i;
    const UmiProductExecutionWorkItem *item;
    if (policy == NULL || queue == NULL || item_index >= queue->count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = &queue->items[item_index];
    if (item->step.owner == UMI_PRODUCTISATION_OWNER_EXTERNAL_ADAPTER &&
        !policy->allow_external_adapters) return UMI_STATUS_PERMISSION_DENIED;
    if (item->step.owner == UMI_PRODUCTISATION_OWNER_APPLICATION &&
        !policy->allow_application_adoption) return UMI_STATUS_PERMISSION_DENIED;
    if (policy->require_framework_completion_first &&
        item->step.stage != UMI_PRODUCTISATION_STAGE_FRAMEWORK) {
        for (i = 0U; i < queue->count; ++i) {
            if (queue->items[i].step.stage == UMI_PRODUCTISATION_STAGE_FRAMEWORK &&
                queue->items[i].state != UMI_PRODUCT_EXECUTION_SUCCEEDED) {
                return UMI_STATUS_BUSY;
            }
        }
    }
    return UMI_STATUS_OK;
}
