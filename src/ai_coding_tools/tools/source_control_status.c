/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/source_control_status.c
 *
 * PURPOSE:
 *   Refresh and return provider-neutral source-control status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/source_control_status.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_status_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "source-control.status",
        .label = "Source Control Status",
        .description = "Refresh and read branch/change/staged counts.",
        .permission = "source-control.read",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_READ,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_source_control_status_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiDeveloperSourceControlSnapshot snapshot;
    UmiLanguageRuntimeJsonWriter writer;
    UmiStatus status;

    (void)arguments_json;

    if (environment == NULL || environment->source_control == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_developer_source_control_refresh(
        environment->source_control);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_source_control_snapshot(
        environment->source_control,
        &snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(&writer, ",\"root\":");
    (void)umi_language_runtime_json_writer_string(&writer, snapshot.root);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"provider\":");
    (void)umi_language_runtime_json_writer_string(
        &writer, snapshot.provider_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"branch\":");
    (void)umi_language_runtime_json_writer_string(
        &writer, snapshot.branch.name);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"changes\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.change_count);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"staged\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, snapshot.staged_count);
    return umi_ai_coding_tool_write_status_end(&writer);
}
