/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/batch_execution.c
 *
 * PURPOSE:
 *   Implement bounded sequential execution over the dependency-aware pipeline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/batch_execution.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Perform developer batch through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_developer_batch_execute(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperExecutor *executor,
    const UmiDeveloperBatchExecutionRequest *request,
    UmiDeveloperBatchExecutionSnapshot *out_snapshot)
{
    UmiDeveloperBatchExecutionSnapshot summary;
    UmiDeveloperOperationSnapshot operation;
    UmiDeveloperExecutionResult result;
    size_t limit;
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || executor == NULL || executor->execute == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(&summary, 0, sizeof(summary));
    summary.struct_size = (uint32_t)sizeof(summary);
    summary.api_version = UMI_DEVELOPER_BATCH_EXECUTION_API_VERSION;
    summary.last_status = UMI_STATUS_OK;
    limit = request == NULL || request->maximum_operations == 0U
                ? UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY
                : request->maximum_operations;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (summary.attempted_count < limit) {
        memset(&operation, 0, sizeof(operation));
        memset(&result, 0, sizeof(result));
        status = umi_developer_runtime_execute_next(
            runtime, executor, &operation, &result);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) {
            status = UMI_STATUS_OK;
            break;
        }

        summary.attempted_count += 1U;
        summary.last_status = status;
        summary.last_exit_code = result.exit_code;
        copy_text(summary.last_operation_id,
                  sizeof(summary.last_operation_id),
                  operation.id);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result.cancelled != 0) summary.cancelled_count += 1U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result.timed_out != 0) summary.timed_out_count += 1U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && result.exit_code == 0 &&
            result.cancelled == 0 && result.timed_out == 0) {
            summary.succeeded_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            summary.failed_count += 1U;
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (request == NULL || request->continue_on_failure == 0) {
                break;
            }
        }
    }

    (void)umi_developer_pipeline_snapshot(
        umi_developer_runtime_pipeline(runtime), &summary.pipeline);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_snapshot != NULL) *out_snapshot = summary;
    return status;
}

/*
 * Provide the developer batch execute process operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_batch_execute_process(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperBatchExecutionRequest *request,
    UmiDeveloperBatchExecutionSnapshot *out_snapshot)
{
    UmiDeveloperExecutor executor;
    umi_developer_executor_init(&executor, umi_developer_process_execute, NULL);
    return umi_developer_batch_execute(runtime, &executor, request, out_snapshot);
}
