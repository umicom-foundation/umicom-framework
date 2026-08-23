/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/tools_policy.c
 *
 * PURPOSE:
 *   Return the active tool risk/capability policy for transparent agent behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/tools_policy.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_tools_policy_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "tools.policy",
        .label = "AI Coding Tool Policy",
        .description = "Read active capability, approval and network policy.",
        .permission = "tools.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = 0U,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_tools_policy_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonWriter writer;
    UmiStatus status;

    (void)arguments_json;

    if (environment == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"allowedCapabilities\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, environment->policy.allowed_capabilities);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"autoApprovedCapabilities\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, environment->policy.auto_approved_capabilities);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"allowNetwork\":");
    (void)umi_language_runtime_json_writer_bool(
        &writer, environment->policy.allow_network);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"allowProcessExecution\":");
    (void)umi_language_runtime_json_writer_bool(
        &writer, environment->policy.allow_process_execution);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"allowSourceControlMutation\":");
    (void)umi_language_runtime_json_writer_bool(
        &writer, environment->policy.allow_source_control_mutation);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"allowDebugControl\":");
    (void)umi_language_runtime_json_writer_bool(
        &writer, environment->policy.allow_debug_control);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"allowCheckpointRestore\":");
    (void)umi_language_runtime_json_writer_bool(
        &writer, environment->policy.allow_checkpoint_restore);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"maxPlanSteps\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, environment->policy.maximum_plan_steps);
    return umi_ai_coding_tool_write_status_end(&writer);
}
