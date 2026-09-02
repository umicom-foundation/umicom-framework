/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/executor.c
 *
 * PURPOSE:
 *   Implement developer executors, including the native structured process
 *   adapter that delegates to Umicom platform process execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/executor.h"

#include <string.h>

#include "umicom/platform/process.h"

/*
 * Initialise developer executor from caller-provided values so later operations receive a
 * known state.
 */
void umi_developer_executor_init(
    UmiDeveloperExecutor *executor,
    UmiDeveloperExecuteFunction execute,
    void *user_data)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL) {
        return;
    }

    memset(executor, 0, sizeof(*executor));
    executor->struct_size = (uint32_t)sizeof(*executor);
    executor->api_version = UMI_DEVELOPER_EXECUTION_API_VERSION;
    executor->user_data = user_data;
    executor->execute = execute;
}

/*
 * Perform developer process through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_developer_process_execute(
    void *user_data,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out_result)
{
    const char *arguments[UMI_DEVELOPER_MAX_ARGUMENTS];
    UmiProcessRequest request;
    UmiProcessResult process_result;
    UmiStatus status;
    size_t index;
    size_t output_length;

    (void)user_data;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL || out_result == NULL ||
        operation->program[0] == '\0' ||
        operation->argument_count > UMI_DEVELOPER_MAX_ARGUMENTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < operation->argument_count; ++index) {
        arguments[index] = operation->arguments[index];
    }

    memset(&request, 0, sizeof(request));
    request.program = operation->program;
    request.arguments = arguments;
    request.argument_count = operation->argument_count;
    request.working_directory =
        operation->working_directory[0] != '\0'
            ? operation->working_directory
            : NULL;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = operation->timeout_ms;
    request.poll_interval_ms = 20U;

    memset(&process_result, 0, sizeof(process_result));
    status = umi_process_execute(&request, &process_result);

    memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_DEVELOPER_EXECUTION_API_VERSION;
    out_result->exit_code = process_result.exit_code;
    out_result->launched = process_result.launched;
    out_result->cancelled = process_result.cancelled;
    out_result->timed_out = process_result.timed_out;
    out_result->output_truncated = process_result.output_truncated;
    out_result->duration_ms = process_result.duration_ms;

    output_length = strlen(process_result.output);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (output_length >= sizeof(out_result->output)) {
        output_length = sizeof(out_result->output) - 1U;
        out_result->output_truncated = 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (output_length > 0U) {
        memcpy(out_result->output, process_result.output, output_length);
    }
    out_result->output[output_length] = '\0';

    return status;
}
