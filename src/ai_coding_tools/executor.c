/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/executor.c
 *
 * PURPOSE:
 *   Implement policy-gated dispatch through the existing UmiAiRuntime tool
 *   registry. A model never calls a service implementation directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/executor.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise ai coding tool executor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_tool_executor_init(
    UmiAiCodingToolExecutor *executor,
    UmiAiCodingToolEnvironment *environment)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL || environment == NULL ||
        environment->ai_runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(executor, 0, sizeof(*executor));
    executor->environment = environment;

    status = umi_ai_coding_tool_result_history_create(&executor->history);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    executor->sequence = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding tool executor deinit operation used by this module and its client
 * applications.
 */
void umi_ai_coding_tool_executor_deinit(
    UmiAiCodingToolExecutor *executor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL) return;

    umi_ai_coding_tool_result_history_destroy(executor->history);
    (void)memset(executor, 0, sizeof(*executor));
}

/*
 * Perform ai coding tool through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ai_coding_tool_execute(
    UmiAiCodingToolExecutor *executor,
    const UmiAiCodingToolCall *call,
    UmiAiCodingToolResult *out_result)
{
    const UmiAiCodingToolDescriptor *descriptor;
    int approval_required = 0;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL || call == NULL || out_result == NULL ||
        call->tool_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->call_id = call->call_id;
    (void)snprintf(
        out_result->tool_id,
        sizeof(out_result->tool_id),
        "%s",
        call->tool_id);
    out_result->sequence = executor->sequence++;

    descriptor = umi_ai_coding_tool_catalogue_find(call->tool_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) {
        out_result->status = UMI_STATUS_NOT_FOUND;
        out_result->state = UMI_AI_CODING_TOOL_CALL_FAILED;
        status = UMI_STATUS_NOT_FOUND;
        goto record;
    }

    status = umi_ai_coding_tool_policy_check(
        &executor->environment->policy,
        descriptor,
        call->approved,
        &approval_required);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        out_result->status = status;
        out_result->state = approval_required
            ? UMI_AI_CODING_TOOL_CALL_APPROVAL_REQUIRED
            : UMI_AI_CODING_TOOL_CALL_REJECTED;
        (void)snprintf(
            out_result->output,
            sizeof(out_result->output),
            "{\"ok\":false,\"status\":%u,\"approvalRequired\":%s,"
            "\"tool\":\"%s\",\"risk\":\"%s\"}",
            (unsigned)status,
            approval_required ? "true" : "false",
            descriptor->tool_id,
            umi_ai_coding_tool_risk_text(descriptor->risk));
        out_result->output_length = strlen(out_result->output);
        goto record;
    }

    status = umi_ai_runtime_invoke_tool(
        executor->environment->ai_runtime,
        call->tool_id,
        1,
        call->arguments_json[0] != '\0' ? call->arguments_json : "{}",
        out_result->output,
        sizeof(out_result->output));

    out_result->status = status;
    out_result->state = status == UMI_STATUS_OK
        ? UMI_AI_CODING_TOOL_CALL_SUCCEEDED
        : UMI_AI_CODING_TOOL_CALL_FAILED;
    out_result->output_length = strlen(out_result->output);

record:
    {
        UmiStatus record_status =
            umi_ai_coding_tool_result_history_record(
                executor->history,
                out_result);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (record_status != UMI_STATUS_OK) return record_status;
    }

    return status;
}

/*
 * Provide the ai coding tool executor history operation used by this module and its client
 * applications.
 */
UmiAiCodingToolResultHistory *umi_ai_coding_tool_executor_history(
    UmiAiCodingToolExecutor *executor)
{
    return executor != NULL ? executor->history : NULL;
}
