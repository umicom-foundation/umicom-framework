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
void umi_product_execution_runtime_init(UmiProductExecutionRuntime *runtime);
UmiStatus umi_product_execution_runtime_set_policy(
    UmiProductExecutionRuntime *runtime,
    const UmiProductExecutionPolicy *policy);
UmiStatus umi_product_execution_runtime_set_adapter(
    UmiProductExecutionRuntime *runtime,
    const UmiProductExecutionAdapter *adapter);
UmiStatus umi_product_execution_runtime_load_plan(
    UmiProductExecutionRuntime *runtime,
    const UmiProductisationCompletionPlan *plan);
UmiStatus umi_product_execution_runtime_refresh(
    UmiProductExecutionRuntime *runtime);
UmiStatus umi_product_execution_runtime_step(
    UmiProductExecutionRuntime *runtime);
UmiStatus umi_product_execution_runtime_run(
    UmiProductExecutionRuntime *runtime,
    size_t maximum_steps);
UmiStatus umi_product_execution_runtime_checkpoint(
    UmiProductExecutionRuntime *runtime,
    const char *label);
void umi_product_execution_runtime_cancel(UmiProductExecutionRuntime *runtime);
UmiProductExecutionSnapshot umi_product_execution_runtime_snapshot(
    const UmiProductExecutionRuntime *runtime);
#ifdef __cplusplus
}
#endif
#endif
