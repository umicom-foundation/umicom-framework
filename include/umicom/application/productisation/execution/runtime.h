/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/runtime.h
 *
 * PURPOSE:
 *   Own the Framework completion execution control plane over canonical productisation plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_RUNTIME_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_RUNTIME_H
#include "umicom/application/productisation/execution/checkpoint.h"
#include "umicom/application/productisation/execution/executor.h"
#include "umicom/application/productisation/execution/rollback.h"
#include "umicom/application/productisation/execution/scheduler.h"
#include "umicom/application/productisation/execution/snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionRuntime {
    UmiProductExecutionPolicy policy;
    UmiProductExecutionAdapter adapter;
    UmiProductExecutionWorkQueue queue;
    UmiProductExecutionDependencyGraph dependencies;
    UmiProductExecutionHistory history;
    UmiProductExecutionCheckpointLedger checkpoints;
    UmiProductExecutionRollbackPlan rollback;
    UmiProductExecutionCancellation cancellation;
    uint64_t revision;
    size_t current_item;
    int adapter_configured;
    int loaded;
    int has_current_item;
} UmiProductExecutionRuntime;
/** Initialise all bounded execution services and their default policy. */
void umi_product_execution_runtime_init(UmiProductExecutionRuntime *runtime);
/** Replace execution limits after validating the supplied policy. */
UmiStatus umi_product_execution_runtime_set_policy(
    UmiProductExecutionRuntime *runtime,
    const UmiProductExecutionPolicy *policy);
/** Attach the host callbacks that perform owner-specific work. */
UmiStatus umi_product_execution_runtime_set_adapter(
    UmiProductExecutionRuntime *runtime,
    const UmiProductExecutionAdapter *adapter);
/** Convert a completion plan into a dependency-ordered execution queue. */
UmiStatus umi_product_execution_runtime_load_plan(
    UmiProductExecutionRuntime *runtime,
    const UmiProductisationCompletionPlan *plan);
/** Recalculate readiness, metrics and acceptance without executing work. */
UmiStatus umi_product_execution_runtime_refresh(
    UmiProductExecutionRuntime *runtime);
/** Execute at most one ready work item and update runtime evidence. */
UmiStatus umi_product_execution_runtime_step(
    UmiProductExecutionRuntime *runtime);
/** Continue stepping until completion, cancellation, blocking or failure. */
UmiStatus umi_product_execution_runtime_run(
    UmiProductExecutionRuntime *runtime,
    size_t maximum_steps);
/** Capture the current queue and history position for later recovery. */
UmiStatus umi_product_execution_runtime_checkpoint(
    UmiProductExecutionRuntime *runtime,
    const char *label);
/** Request cooperative cancellation at the next safe execution boundary. */
void umi_product_execution_runtime_cancel(UmiProductExecutionRuntime *runtime);
/** Return a value snapshot of progress, metrics, acceptance and status. */
UmiProductExecutionSnapshot umi_product_execution_runtime_snapshot(
    const UmiProductExecutionRuntime *runtime);
#ifdef __cplusplus
}
#endif
#endif
