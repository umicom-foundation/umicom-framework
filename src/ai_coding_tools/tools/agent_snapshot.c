/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/agent_snapshot.c
 *
 * PURPOSE:
 *   Return current coding-agent task, iteration, patch and validation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/agent_snapshot.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_agent_snapshot_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "agent.snapshot",
        .label = "AI Coding Agent Snapshot",
        .description = "Read coding-agent task, patch and validation state.",
        .permission = "agent.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_AGENT_READ,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_agent_snapshot_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiAiCodingRuntimePlatformSnapshot snapshot;
    UmiLanguageRuntimeJsonWriter writer;
    UmiStatus status;

    (void)arguments_json;

    if (environment == NULL || environment->coding_runtime == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_runtime_platform_snapshot(
        environment->coding_runtime,
        &snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"taskId\":");
    (void)umi_language_runtime_json_writer_string(
        &writer, snapshot.agent.task_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"state\":");
    (void)umi_language_runtime_json_writer_string(
        &writer, umi_ai_coding_runtime_state_text(snapshot.agent.state));
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"iteration\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.agent.iteration);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"maxIterations\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.agent.maximum_iterations);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"patchId\":");
    (void)umi_language_runtime_json_writer_string(
        &writer, snapshot.agent.patch_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"patchApplied\":");
    (void)umi_language_runtime_json_writer_bool(
        &writer, snapshot.agent.patch_applied);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"validationPassed\":");
    (void)umi_language_runtime_json_writer_bool(
        &writer, snapshot.agent.validation_passed);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"queuedTasks\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.queued_tasks);
    return umi_ai_coding_tool_write_status_end(&writer);
}
