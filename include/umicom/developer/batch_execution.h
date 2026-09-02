/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/batch_execution.h
 *
 * PURPOSE:
 *   Execute all currently reachable operations from a developer pipeline using
 *   the standard executor boundary and return one deterministic batch summary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_BATCH_EXECUTION_H
#define UMICOM_DEVELOPER_BATCH_EXECUTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/developer/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_BATCH_EXECUTION_API_VERSION 1U

/**
 * Represent the developer batch execution request data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperBatchExecutionRequest {
    uint32_t struct_size;
    uint32_t api_version;
    size_t maximum_operations;
    int continue_on_failure;
} UmiDeveloperBatchExecutionRequest;

/**
 * Represent the developer batch execution snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperBatchExecutionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t attempted_count;
    size_t succeeded_count;
    size_t failed_count;
    size_t cancelled_count;
    size_t timed_out_count;
    char last_operation_id[UMI_DEVELOPER_ID_CAPACITY];
    int last_exit_code;
    UmiStatus last_status;
    UmiDeveloperPipelineSnapshot pipeline;
} UmiDeveloperBatchExecutionSnapshot;

/**
 * Perform developer batch through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_developer_batch_execute(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperExecutor *executor,
    const UmiDeveloperBatchExecutionRequest *request,
    UmiDeveloperBatchExecutionSnapshot *out_snapshot);

/**
 * Provide the developer batch execute process operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_batch_execute_process(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperBatchExecutionRequest *request,
    UmiDeveloperBatchExecutionSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
