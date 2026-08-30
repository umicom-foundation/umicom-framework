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
void umi_product_execution_runtime_init(UmiProductExecutionRuntime *runtime)
{
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
UmiStatus umi_product_execution_runtime_set_policy(
    UmiProductExecutionRuntime *runtime,
    const UmiProductExecutionPolicy *policy)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_execution_policy_validate(policy);
    if (status == UMI_STATUS_OK) {
        runtime->policy = *policy;
        runtime->revision++;
    }
    return status;
}
UmiStatus umi_product_execution_runtime_set_adapter(
    UmiProductExecutionRuntime *runtime,
    const UmiProductExecutionAdapter *adapter)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_execution_adapter_validate(adapter);
    if (status == UMI_STATUS_OK) {
        runtime->adapter = *adapter;
        runtime->adapter_configured = 1;
        runtime->revision++;
    }
    return status;
}
UmiStatus umi_product_execution_runtime_load_plan(
    UmiProductExecutionRuntime *runtime,
    const UmiProductisationCompletionPlan *plan)
{
    UmiStatus status;
    if (runtime == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_execution_work_queue_from_plan(
        &runtime->queue, plan, runtime->policy.maximum_attempts);
    if (status != UMI_STATUS_OK) return status;
    umi_product_execution_dependency_graph_init(&runtime->dependencies);
    status = umi_product_execution_dependency_graph_add_stage_barriers(
        &runtime->dependencies, &runtime->queue);
    if (status != UMI_STATUS_OK) return status;
    runtime->loaded = 1;
    runtime->has_current_item = 0;
    runtime->revision++;
    return umi_product_execution_runtime_refresh(runtime);
}
UmiStatus umi_product_execution_runtime_refresh(
    UmiProductExecutionRuntime *runtime)
{
    if (runtime == NULL || !runtime->loaded) return UMI_STATUS_INVALID_STATE;
    runtime->revision++;
    return umi_product_execution_scheduler_refresh_ready(
        &runtime->queue, &runtime->dependencies, &runtime->policy);
}
UmiStatus umi_product_execution_runtime_step(
    UmiProductExecutionRuntime *runtime)
{
    size_t index;
    UmiStatus status;
    if (runtime == NULL || !runtime->loaded || !runtime->adapter_configured) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_product_execution_runtime_refresh(runtime);
    if (status != UMI_STATUS_OK) return status;
    status = umi_product_execution_scheduler_next(&runtime->queue, &index);
    if (status != UMI_STATUS_OK) return status;
    runtime->current_item = index;
    runtime->has_current_item = 1;
    status = umi_product_execution_execute_item(
        &runtime->queue, index, &runtime->policy, &runtime->adapter,
        &runtime->cancellation, &runtime->history);
    runtime->revision++;
    return status;
}
UmiStatus umi_product_execution_runtime_run(
    UmiProductExecutionRuntime *runtime,
    size_t maximum_steps)
{
    size_t i;
    UmiStatus status = UMI_STATUS_OK;
    if (runtime == NULL || maximum_steps == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < maximum_steps; ++i) {
        status = umi_product_execution_runtime_step(runtime);
        if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_INTERNAL_ERROR &&
            status != UMI_STATUS_IO_ERROR &&
            status != UMI_STATUS_UNAVAILABLE) return status;
    }
    return status;
}
UmiStatus umi_product_execution_runtime_checkpoint(
    UmiProductExecutionRuntime *runtime,
    const char *label)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_execution_checkpoint_capture(
        &runtime->checkpoints, &runtime->queue, label);
    if (status == UMI_STATUS_OK) {
        runtime->revision++;
        (void)umi_product_execution_history_append(
            &runtime->history, runtime->has_current_item ? runtime->current_item : 0U,
            UMI_PRODUCT_EXECUTION_EVENT_CHECKPOINT, UMI_STATUS_OK, label);
    }
    return status;
}
void umi_product_execution_runtime_cancel(UmiProductExecutionRuntime *runtime)
{
    if (runtime != NULL) {
        umi_product_execution_cancellation_request(&runtime->cancellation);
        runtime->revision++;
    }
}
UmiProductExecutionSnapshot umi_product_execution_runtime_snapshot(
    const UmiProductExecutionRuntime *runtime)
{
    UmiProductExecutionSnapshot snapshot;
    (void)memset(&snapshot, 0, sizeof(snapshot));
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
