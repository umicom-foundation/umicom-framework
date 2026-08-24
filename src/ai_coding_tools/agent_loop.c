/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/agent_loop.c
 *
 * PURPOSE:
 *   Implement bounded provider/tool/provider interaction with explicit
 *   approval mediation and tool-result feedback.
 *
 * MEMORY POLICY:
 *   AI plan/request structures intentionally own bounded copies of arguments,
 *   messages and tool output.  Several of those structures exceed the default
 *   Windows thread stack by themselves.  Phase 5 therefore allocates the
 *   largest temporary aggregates on the heap while preserving the stable C ABI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/agent_loop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int starts_with(const char *text, const char *prefix)
{
    return text != NULL && prefix != NULL &&
        strncmp(text, prefix, strlen(prefix)) == 0;
}

static UmiStatus approve_call_if_needed(
    UmiAiCodingToolEnvironment *environment,
    const UmiAiCodingToolLoopConfig *config,
    UmiAiCodingToolCall *call,
    int *out_approval_stop)
{
    const UmiAiCodingToolDescriptor *descriptor;
    int approval_required = 0;
    UmiStatus status;

    *out_approval_stop = 0;

    /*
     * A provider is never an approval authority. Even if textual model output
     * says APPROVED|1, discard that value before consulting host policy.
     */
    call->approved = 0;

    descriptor = umi_ai_coding_tool_catalogue_find(call->tool_id);
    if (descriptor == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_ai_coding_tool_policy_check(
        &environment->policy,
        descriptor,
        0,
        &approval_required);

    if (status == UMI_STATUS_OK) return UMI_STATUS_OK;

    if (!approval_required) return status;

    if (config->approval == NULL) {
        *out_approval_stop = 1;
        return UMI_STATUS_OK;
    }

    {
        int approved = 0;

        status = config->approval(
            config->approval_user_data,
            descriptor,
            call,
            &approved);
        if (status != UMI_STATUS_OK) return status;

        call->approved = approved != 0;

        if (!call->approved) {
            *out_approval_stop = 1;
        }
    }

    return UMI_STATUS_OK;
}

static UmiStatus add_tool_result_message(
    UmiAiCodingToolChatSession *session,
    const char *name,
    const char *text)
{
    return umi_ai_coding_tool_chat_add_chunked(
        session,
        UMI_AI_ROLE_TOOL,
        name,
        text);
}

static UmiStatus format_plan_result(
    const UmiAiCodingToolPlanResult *result,
    char *out_text,
    size_t capacity)
{
    size_t index;
    size_t used = 0U;
    int written;

    if (result == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(
        out_text,
        capacity,
        "Tool plan %s completed=%d succeeded=%d passed=%zu failed=%zu rejected=%zu\n",
        result->plan_id,
        result->completed,
        result->succeeded,
        result->passed_count,
        result->failed_count,
        result->rejected_count);

    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;

    for (index = 0U; index < result->result_count; ++index) {
        const UmiAiCodingToolResult *item = &result->results[index];

        written = snprintf(
            out_text + used,
            capacity - used,
            "[%zu] %s state=%s status=%u\n%s\n",
            index,
            item->tool_id,
            umi_ai_coding_tool_call_state_text(item->state),
            (unsigned)item->status,
            item->output);

        if (written < 0 || (size_t)written >= capacity - used) {
            out_text[capacity - 1U] = '\0';
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        used += (size_t)written;
    }

    return UMI_STATUS_OK;
}

/*
 * Keep multi-megabyte plan state off the thread stack.  A dedicated helper
 * makes ownership and cleanup visible and ensures every error path releases all
 * temporary storage before returning to the provider/tool loop.
 */
static UmiStatus execute_plan_turn(
    UmiAiCodingToolEnvironment *environment,
    UmiAiCodingToolExecutor *executor,
    UmiAiCodingToolChatSession *session,
    const UmiAiCodingToolLoopConfig *config,
    const char *response_text,
    uint64_t *next_call_id,
    UmiAiCodingToolLoopResult *out_result)
{
    UmiAiCodingToolPlan *plan = NULL;
    UmiAiCodingToolPlanResult *plan_result = NULL;
    char *plan_text = NULL;
    size_t index;
    int approval_stop = 0;
    UmiStatus status;
    UmiStatus execution_status;

    if (environment == NULL || executor == NULL || session == NULL ||
        config == NULL || response_text == NULL || next_call_id == NULL ||
        out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    plan = (UmiAiCodingToolPlan *)calloc(1U, sizeof(*plan));
    plan_result = (UmiAiCodingToolPlanResult *)calloc(1U, sizeof(*plan_result));
    plan_text = (char *)calloc(
        UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES,
        sizeof(*plan_text));
    if (plan == NULL || plan_result == NULL || plan_text == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }

    status = umi_ai_coding_tool_chat_add_chunked(
        session,
        UMI_AI_ROLE_ASSISTANT,
        "assistant-tool-plan",
        response_text);
    if (status != UMI_STATUS_OK) goto cleanup;

    status = umi_ai_coding_tool_plan_parse(
        response_text,
        *next_call_id,
        plan);
    if (status != UMI_STATUS_OK) goto cleanup;

    *next_call_id += plan->step_count;

    for (index = 0U; index < plan->step_count; ++index) {
        int step_stop = 0;

        status = approve_call_if_needed(
            environment,
            config,
            &plan->steps[index].call,
            &step_stop);
        if (status != UMI_STATUS_OK) goto cleanup;
        if (step_stop) approval_stop = 1;
    }

    execution_status = umi_ai_coding_tool_plan_execute(
        executor,
        plan,
        plan_result);

    out_result->tool_plans += 1U;
    out_result->last_plan_result = *plan_result;

    {
        UmiStatus format_status = format_plan_result(
            plan_result,
            plan_text,
            UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES);
        if (format_status != UMI_STATUS_OK &&
            format_status != UMI_STATUS_CAPACITY_EXCEEDED) {
            status = format_status;
            goto cleanup;
        }
    }

    status = add_tool_result_message(
        session,
        "tool-plan-result",
        plan_text);
    if (status != UMI_STATUS_OK) goto cleanup;

    if (approval_stop || plan_result->rejected_count > 0U) {
        out_result->approval_stops += 1U;
        out_result->status = UMI_STATUS_PERMISSION_DENIED;
        status = UMI_STATUS_PERMISSION_DENIED;
        goto cleanup;
    }

    /*
     * Preserve the previous agent-loop contract: individual plan execution
     * failure is represented in plan_result and fed back to the model rather
     * than terminating the whole loop at this boundary.
     */
    (void)execution_status;
    status = UMI_STATUS_OK;

cleanup:
    free(plan_text);
    free(plan_result);
    free(plan);
    return status;
}

void umi_ai_coding_tool_loop_config_init(
    UmiAiCodingToolLoopConfig *config)
{
    if (config == NULL) return;

    (void)memset(config, 0, sizeof(*config));
    config->maximum_tool_turns = 4U;
    config->max_output_tokens = 4096U;
    config->temperature = 0.1;
    config->provider_approved = 0;
}

UmiStatus umi_ai_coding_tool_agent_loop_run(
    UmiAiRuntime *runtime,
    UmiAiCodingToolEnvironment *environment,
    UmiAiCodingToolExecutor *executor,
    UmiAiCodingToolChatSession *session,
    const UmiAiCodingToolLoopConfig *config,
    const char *user_message,
    UmiAiCodingToolLoopResult *out_result)
{
    char tool_prompt[UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES];
    uint64_t next_call_id = 1U;
    uint32_t turn;
    UmiStatus status;

    if (runtime == NULL || environment == NULL ||
        executor == NULL || session == NULL ||
        config == NULL || user_message == NULL ||
        out_result == NULL ||
        config->maximum_tool_turns == 0U ||
        config->maximum_tool_turns > UMI_AI_CODING_TOOL_LOOP_MAX_TURNS ||
        config->max_output_tokens == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->revision = 1U;

    status = umi_ai_coding_tool_prompt_build(
        environment,
        tool_prompt,
        sizeof(tool_prompt));
    if (status != UMI_STATUS_OK) return status;

    if (session->message_count == 0U) {
        status = umi_ai_coding_tool_chat_add_chunked(
            session,
            UMI_AI_ROLE_SYSTEM,
            "umicom-developer-tools",
            tool_prompt);
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_ai_coding_tool_chat_add_chunked(
        session,
        UMI_AI_ROLE_USER,
        "user",
        user_message);
    if (status != UMI_STATUS_OK) return status;

    for (turn = 0U; turn < config->maximum_tool_turns; ++turn) {
        UmiAiRequest *request = NULL;
        UmiAiResponse response;

        request = (UmiAiRequest *)calloc(1U, sizeof(*request));
        if (request == NULL) return UMI_STATUS_OUT_OF_MEMORY;

        status = umi_ai_coding_tool_chat_build_request(
            session,
            config->max_output_tokens,
            config->temperature,
            request);
        if (status != UMI_STATUS_OK) {
            free(request);
            return status;
        }

        umi_ai_response_init(&response);

        status = umi_ai_runtime_generate(
            runtime,
            session->provider_id,
            config->provider_approved,
            request,
            &response);
        free(request);
        request = NULL;

        if (status != UMI_STATUS_OK) {
            out_result->status = status;
            return status;
        }

        out_result->provider_turns += 1U;

        if (starts_with(response.text, "UMICOM-TOOL-CALL/1")) {
            UmiAiCodingToolCall call;
            UmiAiCodingToolResult tool_result;
            int approval_stop = 0;

            status = umi_ai_coding_tool_chat_add_chunked(
                session,
                UMI_AI_ROLE_ASSISTANT,
                "assistant-tool-call",
                response.text);
            if (status != UMI_STATUS_OK) return status;

            status = umi_ai_coding_tool_call_parse(
                response.text,
                next_call_id++,
                &call);
            if (status != UMI_STATUS_OK) return status;

            status = approve_call_if_needed(
                environment,
                config,
                &call,
                &approval_stop);
            if (status != UMI_STATUS_OK) return status;

            status = umi_ai_coding_tool_execute(
                executor,
                &call,
                &tool_result);

            out_result->tool_calls += 1U;
            out_result->last_tool_result = tool_result;

            if (tool_result.output[0] != '\0') {
                UmiStatus message_status = add_tool_result_message(
                    session,
                    call.tool_id,
                    tool_result.output);
                if (message_status != UMI_STATUS_OK) return message_status;
            }

            if (approval_stop ||
                tool_result.state ==
                    UMI_AI_CODING_TOOL_CALL_APPROVAL_REQUIRED) {
                out_result->approval_stops += 1U;
                out_result->status = UMI_STATUS_PERMISSION_DENIED;
                return UMI_STATUS_PERMISSION_DENIED;
            }

            if (status != UMI_STATUS_OK &&
                tool_result.state != UMI_AI_CODING_TOOL_CALL_FAILED) {
                out_result->status = status;
                return status;
            }

            continue;
        }

        if (starts_with(response.text, "UMICOM-TOOL-PLAN/1")) {
            UmiAiCodingToolPlan plan;
            UmiAiCodingToolPlanResult plan_result;
            char plan_text[UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES];
            size_t index;
            int approval_stop = 0;

            status = umi_ai_coding_tool_chat_add_chunked(
                session,
                UMI_AI_ROLE_ASSISTANT,
                "assistant-tool-plan",
                response.text);
            if (status != UMI_STATUS_OK) return status;

            status = umi_ai_coding_tool_plan_parse(
                response.text,
                next_call_id,
                &plan);
            if (status != UMI_STATUS_OK) return status;

            next_call_id += plan.step_count;

            for (index = 0U; index < plan.step_count; ++index) {
                int step_stop = 0;

                status = approve_call_if_needed(
                    environment,
                    config,
                    &plan.steps[index].call,
                    &step_stop);
                if (status != UMI_STATUS_OK) return status;
                if (step_stop) approval_stop = 1;
            }

            status = umi_ai_coding_tool_plan_execute(
                executor,
                &plan,
                &plan_result);

            out_result->tool_plans += 1U;
            out_result->last_plan_result = plan_result;

            {
                UmiStatus format_status = format_plan_result(
                    &plan_result,
                    plan_text,
                    sizeof(plan_text));
                if (format_status != UMI_STATUS_OK &&
                    format_status != UMI_STATUS_CAPACITY_EXCEEDED) {
                    return format_status;
                }
            }

            {
                UmiStatus message_status = add_tool_result_message(
                    session,
                    "tool-plan-result",
                    plan_text);
                if (message_status != UMI_STATUS_OK) return message_status;
            }

            if (approval_stop || plan_result.rejected_count > 0U) {
                out_result->approval_stops += 1U;
                out_result->status = UMI_STATUS_PERMISSION_DENIED;
                return UMI_STATUS_PERMISSION_DENIED;
            }

            (void)status;
            continue;
        }

        status = umi_ai_coding_tool_chat_add_chunked(
            session,
            UMI_AI_ROLE_ASSISTANT,
            "assistant",
            response.text);
        if (status != UMI_STATUS_OK) return status;

        if (strlen(response.text) >= sizeof(out_result->final_text)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)strcpy(out_result->final_text, response.text);
        out_result->status = UMI_STATUS_OK;
        out_result->completed = 1;
        out_result->revision += 1U;
        return UMI_STATUS_OK;
    }

    out_result->status = UMI_STATUS_TIMEOUT;
    return UMI_STATUS_TIMEOUT;
}
