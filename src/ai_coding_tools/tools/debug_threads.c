/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/debug_threads.c
 *
 * PURPOSE:
 *   Refresh the active DAP thread model and return current thread metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/debug_threads.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_debug_threads_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "debug.threads",
        .label = "Refresh Debug Threads",
        .description = "Refresh the DAP thread model.",
        .permission = "debug.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_DEBUG_READ,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_debug_threads_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiDebugRuntimePlatformSnapshot snapshot;
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeJsonWriter writer;
    uint64_t timeout = 1000U;
    UmiStatus status;

    if (environment == NULL || environment->debug_runtime == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_uint64(
            &document, "timeoutMs", 1000U, &timeout);
    }
    if (status != UMI_STATUS_OK || timeout > UINT32_MAX) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_debug_runtime_platform_refresh_threads(
        environment->debug_runtime, (uint32_t)timeout);
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_platform_snapshot(
        environment->debug_runtime, &snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"threads\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.service.thread_count);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"activeThreadId\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.active_thread_id);
    return umi_ai_coding_tool_write_status_end(&writer);
}
