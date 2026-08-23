/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/debug_step_into.c
 *
 * PURPOSE:
 *   Step Into through the persistent DAP runtime. Debuggee control always requires
 *   explicit debug-control policy and approval.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/debug_step_into.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_debug_step_into_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "debug.step-into",
        .label = "Step Into",
        .description = "Step Into through the Framework DAP runtime.",
        .permission = "debug.control",
        .risk = UMI_AI_CODING_TOOL_RISK_DEBUG_CONTROL,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_DEBUG_CONTROL,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_debug_step_into_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    UmiDebugRuntimePlatformSnapshot snapshot;
    uint64_t thread_id = 0U;
    uint64_t timeout = 1000U;
    UmiStatus status;

    if (environment == NULL || environment->debug_runtime == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_uint64(
            &document, "threadId", 0U, &thread_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_uint64(
            &document, "timeoutMs", 1000U, &timeout);
    }
    if (status != UMI_STATUS_OK || timeout > UINT32_MAX) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_CAPACITY_EXCEEDED;
    }

    if (thread_id == 0U) {
        status = umi_debug_runtime_platform_snapshot(
            environment->debug_runtime, &snapshot);
        if (status != UMI_STATUS_OK) return status;
        thread_id = snapshot.active_thread_id;
    }

    if (thread_id == 0U) return UMI_STATUS_NOT_FOUND;

    status = umi_debug_runtime_platform_step_into(
        environment->debug_runtime,
        thread_id,
        (uint32_t)timeout);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output, output_capacity, UMI_STATUS_OK, "Step Into completed.");
}
