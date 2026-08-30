/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/workbench_bridge.c
 *
 * PURPOSE:
 *   Execute Explain Selection and Fix Problems through the Framework coding
 *   runtime instead of leaving those Developer Workbench commands as inert menu
 *   placeholders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/workbench_bridge.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiCodingWorkbenchBridge {
    UmiDeveloperWorkbench *workbench;
    UmiAiCodingRuntimePlatform *platform;
    UmiAiCodingWorkbenchContext context;
    uint64_t task_sequence;
};

static int has_active_selection(void *user_data, const char *argument)
{
    UmiAiCodingWorkbenchBridge *bridge =
        (UmiAiCodingWorkbenchBridge *)user_data;
    (void)argument;

    return bridge != NULL &&
        bridge->context.active_path[0] != '\0' &&
        bridge->context.selection_start_line > 0U &&
        bridge->context.selection_end_line >=
            bridge->context.selection_start_line;
}

static int has_problems(void *user_data, const char *argument)
{
    UmiAiCodingWorkbenchBridge *bridge =
        (UmiAiCodingWorkbenchBridge *)user_data;
    (void)argument;

    return bridge != NULL &&
        bridge->context.problems_summary[0] != '\0';
}

static UmiStatus prepare_request(
    UmiAiCodingWorkbenchBridge *bridge,
    UmiAiCodingTaskKind task,
    const char *instruction,
    UmiAiCodingRequest *out_request)
{
    const char *workspace_root;
    int written;

    if (bridge == NULL || instruction == NULL ||
        out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    workspace_root =
        umi_ai_coding_runtime_platform_workspace_root(bridge->platform);
    if (workspace_root == NULL) return UMI_STATUS_INVALID_STATE;

    umi_ai_coding_request_init(out_request, task);

    bridge->task_sequence += 1U;

    written = snprintf(
        out_request->request_id,
        sizeof(out_request->request_id),
        "workbench.ai.%llu",
        (unsigned long long)bridge->task_sequence);
    if (written < 0 ||
        (size_t)written >= sizeof(out_request->request_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)snprintf(
        out_request->session_id,
        sizeof(out_request->session_id),
        "%s",
        bridge->context.session_id);
    (void)snprintf(
        out_request->runtime_id,
        sizeof(out_request->runtime_id),
        "%s",
        bridge->context.runtime_id);
    (void)snprintf(
        out_request->workspace_root,
        sizeof(out_request->workspace_root),
        "%s",
        workspace_root);
    (void)snprintf(
        out_request->active_path,
        sizeof(out_request->active_path),
        "%s",
        bridge->context.active_path);
    (void)snprintf(
        out_request->language_id,
        sizeof(out_request->language_id),
        "%s",
        bridge->context.language_id);
    (void)snprintf(
        out_request->instruction,
        sizeof(out_request->instruction),
        "%s",
        instruction);

    out_request->classification = bridge->context.classification;
    out_request->selection_start_line =
        bridge->context.selection_start_line;
    out_request->selection_end_line =
        bridge->context.selection_end_line;
    out_request->context_token_budget = 16000U;
    out_request->maximum_context_files = 12U;
    out_request->timestamp_ns = bridge->context.timestamp_ns;
    out_request->sensitive_approved =
        bridge->context.sensitive_approved;

    return umi_ai_coding_request_validate(out_request);
}

static UmiStatus run_task(
    UmiAiCodingWorkbenchBridge *bridge,
    UmiAiCodingTaskKind task,
    const char *instruction,
    char *out_message,
    size_t message_capacity)
{
    UmiAiCodingRequest request;
    UmiAiCodingAgentSnapshot snapshot;
    char task_id[UMI_AI_CODING_RUNTIME_ID_CAPACITY];
    int written;
    UmiStatus status;

    status = prepare_request(bridge, task, instruction, &request);
    if (status != UMI_STATUS_OK) return status;

    written = snprintf(
        task_id,
        sizeof(task_id),
        "workbench-task.%llu",
        (unsigned long long)bridge->task_sequence);
    if (written < 0 || (size_t)written >= sizeof(task_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_ai_coding_runtime_platform_run(
        bridge->platform,
        task_id,
        &request);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_agent_snapshot(
        umi_ai_coding_runtime_platform_agent(bridge->platform),
        &snapshot);
    if (status != UMI_STATUS_OK) return status;

    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "AI task state: %s",
            umi_ai_coding_runtime_state_text(snapshot.state));
    }

    return UMI_STATUS_OK;
}

static UmiStatus explain_selection(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiAiCodingWorkbenchBridge *bridge =
        (UmiAiCodingWorkbenchBridge *)user_data;
    const char *instruction =
        argument != NULL && argument[0] != '\0'
            ? argument
            : (bridge->context.instruction[0] != '\0'
                ? bridge->context.instruction
                : "Explain the selected code and its architectural role.");

    return run_task(
        bridge,
        UMI_AI_CODING_TASK_EXPLAIN,
        instruction,
        out_message,
        message_capacity);
}

static UmiStatus fix_problems(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiAiCodingWorkbenchBridge *bridge =
        (UmiAiCodingWorkbenchBridge *)user_data;
    char instruction[UMI_AI_TEXT_CAPACITY];
    int written;

    if (argument != NULL && argument[0] != '\0') {
        written = snprintf(
            instruction,
            sizeof(instruction),
            "%s\nProblems:\n%s",
            argument,
            bridge->context.problems_summary);
    } else {
        written = snprintf(
            instruction,
            sizeof(instruction),
            "Fix the following confirmed developer problems with the smallest "
            "safe complete-file changes. Preserve unrelated features and "
            "comments.\nProblems:\n%s",
            bridge->context.problems_summary);
    }

    if (written < 0 || (size_t)written >= sizeof(instruction)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return run_task(
        bridge,
        UMI_AI_CODING_TASK_REFACTOR,
        instruction,
        out_message,
        message_capacity);
}

UmiStatus umi_ai_coding_workbench_bridge_create(
    UmiDeveloperWorkbench *workbench,
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingWorkbenchBridge **out_bridge)
{
    UmiAiCodingWorkbenchBridge *bridge;

    if (workbench == NULL || platform == NULL ||
        out_bridge == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_bridge = NULL;
    bridge = (UmiAiCodingWorkbenchBridge *)calloc(
        1U, sizeof(*bridge));
    if (bridge == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    bridge->workbench = workbench;
    bridge->platform = platform;
    bridge->context.classification = UMI_AI_DATA_INTERNAL;

    *out_bridge = bridge;
    return UMI_STATUS_OK;
}

void umi_ai_coding_workbench_bridge_destroy(
    UmiAiCodingWorkbenchBridge *bridge)
{
    free(bridge);
}

UmiStatus umi_ai_coding_workbench_bridge_set_context(
    UmiAiCodingWorkbenchBridge *bridge,
    const UmiAiCodingWorkbenchContext *context)
{
    if (bridge == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bridge->context = *context;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_workbench_bridge_bind(
    UmiAiCodingWorkbenchBridge *bridge)
{
    UmiStatus status;

    if (bridge == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_workbench_bind_action(
        bridge->workbench,
        "ai.explain-selection",
        explain_selection,
        has_active_selection,
        bridge);
    if (status != UMI_STATUS_OK) return status;

    return umi_developer_workbench_bind_action(
        bridge->workbench,
        "ai.fix-problems",
        fix_problems,
        has_problems,
        bridge);
}
