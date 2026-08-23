/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/debug_snapshot.c
 *
 * PURPOSE:
 *   Return the current persistent DAP runtime state and major inspection counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/debug_snapshot.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_debug_snapshot_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "debug.snapshot",
        .label = "Debug Snapshot",
        .description = "Read the current DAP debugger state.",
        .permission = "debug.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_DEBUG_READ,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_debug_snapshot_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiDebugRuntimePlatformSnapshot snapshot;
    UmiLanguageRuntimeJsonWriter writer;
    UmiStatus status;

    (void)arguments_json;

    if (environment == NULL || environment->debug_runtime == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_debug_runtime_platform_snapshot(
        environment->debug_runtime,
        &snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"active\":");
    (void)umi_language_runtime_json_writer_bool(&writer, snapshot.active);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"paused\":");
    (void)umi_language_runtime_json_writer_bool(&writer, snapshot.paused);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"sessionId\":");
    (void)umi_language_runtime_json_writer_string(
        &writer, snapshot.active_session_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"threadId\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.active_thread_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"frameId\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.active_frame_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"threads\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.service.thread_count);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"frames\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.service.stack_frame_count);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"variables\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.service.variable_count);
    return umi_ai_coding_tool_write_status_end(&writer);
}
