/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/policy.c
 *
 * PURPOSE:
 *   Enforce Framework-first completion and explicit opt-in boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/policy.h"
/*
 * Provide the product execution policy default operation used by this module and its
 * client applications.
 */
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
/*
 * Check that product execution policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_product_execution_policy_validate(
    const UmiProductExecutionPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || policy->maximum_attempts == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the product execution policy check item operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_policy_check_item(
    const UmiProductExecutionPolicy *policy,
    const UmiProductExecutionWorkQueue *queue,
    size_t item_index)
{
    size_t i;
    const UmiProductExecutionWorkItem *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || queue == NULL || item_index >= queue->count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = &queue->items[item_index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->step.owner == UMI_PRODUCTISATION_OWNER_EXTERNAL_ADAPTER &&
        !policy->allow_external_adapters) return UMI_STATUS_PERMISSION_DENIED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->step.owner == UMI_PRODUCTISATION_OWNER_APPLICATION &&
        !policy->allow_application_adoption) return UMI_STATUS_PERMISSION_DENIED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_framework_completion_first &&
        item->step.stage != UMI_PRODUCTISATION_STAGE_FRAMEWORK) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (i = 0U; i < queue->count; ++i) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (queue->items[i].step.stage == UMI_PRODUCTISATION_STAGE_FRAMEWORK &&
                queue->items[i].state != UMI_PRODUCT_EXECUTION_SUCCEEDED) {
                return UMI_STATUS_BUSY;
            }
        }
    }
    return UMI_STATUS_OK;
}
