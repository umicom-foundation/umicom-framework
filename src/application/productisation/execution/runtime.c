/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/runtime.c
 *
 * PURPOSE:
 *   Coordinate queue materialisation, readiness, execution, checkpoints and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/runtime.h"
#include <string.h>
/*
 * Initialise product execution runtime from caller-provided values so later operations
 * receive a known state.
 */
void umi_product_execution_runtime_init(UmiProductExecutionRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return;
    (void)memset(runtime, 0, sizeof(*runtime));
    runtime->policy = umi_product_execution_policy_default();
    umi_product_execution_work_queue_init(&runtime->queue);
    umi_product_execution_dependency_graph_init(&runtime->dependencies);
    umi_product_execution_history_init(&runtime->history);
    umi_product_execution_checkpoint_ledger_init(&runtime->checkpoints);
    umi_product_execution_rollback_plan_init(&runtime->rollback);
    umi_product_execution_cancellation_init(&runtime->cancellation);
    runtime->revision = 1U;
}
/*
 * Provide the product execution runtime set policy operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_runtime_set_policy(
    UmiProductExecutionRuntime *runtime,
    const UmiProductExecutionPolicy *policy)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_execution_policy_validate(policy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        runtime->policy = *policy;
        runtime->revision++;
    }
    return status;
}
/*
 * Provide the product execution runtime set adapter operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_runtime_set_adapter(
    UmiProductExecutionRuntime *runtime,
    const UmiProductExecutionAdapter *adapter)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_execution_adapter_validate(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        runtime->adapter = *adapter;
        runtime->adapter_configured = 1;
        runtime->revision++;
    }
    return status;
}
/*
 * Provide the product execution runtime load plan operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_runtime_load_plan(
    UmiProductExecutionRuntime *runtime,
    const UmiProductisationCompletionPlan *plan)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_execution_work_queue_from_plan(
        &runtime->queue, plan, runtime->policy.maximum_attempts);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_product_execution_dependency_graph_init(&runtime->dependencies);
    status = umi_product_execution_dependency_graph_add_stage_barriers(
        &runtime->dependencies, &runtime->queue);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    runtime->loaded = 1;
    runtime->has_current_item = 0;
    runtime->revision++;
    return umi_product_execution_runtime_refresh(runtime);
}
/*
 * Provide the product execution runtime refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_runtime_refresh(
    UmiProductExecutionRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || !runtime->loaded) return UMI_STATUS_INVALID_STATE;
    runtime->revision++;
    return umi_product_execution_scheduler_refresh_ready(
        &runtime->queue, &runtime->dependencies, &runtime->policy);
}
/*
 * Provide the product execution runtime step operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_execution_runtime_step(
    UmiProductExecutionRuntime *runtime)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || !runtime->loaded || !runtime->adapter_configured) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_product_execution_runtime_refresh(runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_product_execution_scheduler_next(&runtime->queue, &index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    runtime->current_item = index;
    runtime->has_current_item = 1;
    status = umi_product_execution_execute_item(
        &runtime->queue, index, &runtime->policy, &runtime->adapter,
        &runtime->cancellation, &runtime->history);
    runtime->revision++;
    return status;
}
/*
 * Perform product execution runtime through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_product_execution_runtime_run(
    UmiProductExecutionRuntime *runtime,
    size_t maximum_steps)
{
    size_t i;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || maximum_steps == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < maximum_steps; ++i) {
        status = umi_product_execution_runtime_step(runtime);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_INTERNAL_ERROR &&
            status != UMI_STATUS_IO_ERROR &&
            status != UMI_STATUS_UNAVAILABLE) return status;
    }
    return status;
}
/*
 * Provide the product execution runtime checkpoint operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_runtime_checkpoint(
    UmiProductExecutionRuntime *runtime,
    const char *label)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_execution_checkpoint_capture(
        &runtime->checkpoints, &runtime->queue, label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        runtime->revision++;
        (void)umi_product_execution_history_append(
            &runtime->history, runtime->has_current_item ? runtime->current_item : 0U,
            UMI_PRODUCT_EXECUTION_EVENT_CHECKPOINT, UMI_STATUS_OK, label);
    }
    return status;
}
/*
 * Provide the product execution runtime cancel operation used by this module and its
 * client applications.
 */
void umi_product_execution_runtime_cancel(UmiProductExecutionRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime != NULL) {
        umi_product_execution_cancellation_request(&runtime->cancellation);
        runtime->revision++;
    }
}
/*
 * Provide the product execution runtime snapshot operation used by this module and its
 * client applications.
 */
UmiProductExecutionSnapshot umi_product_execution_runtime_snapshot(
    const UmiProductExecutionRuntime *runtime)
{
    UmiProductExecutionSnapshot snapshot;
    (void)memset(&snapshot, 0, sizeof(snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return snapshot;
    snapshot.revision = runtime->revision;
    snapshot.metrics = umi_product_execution_metrics_collect(&runtime->queue);
    snapshot.acceptance = umi_product_execution_acceptance_evaluate(&runtime->queue);
    snapshot.current_item = runtime->current_item;
    snapshot.has_current_item = runtime->has_current_item;
    snapshot.cancellation_requested =
        umi_product_execution_cancellation_requested(&runtime->cancellation);
    return snapshot;
}
