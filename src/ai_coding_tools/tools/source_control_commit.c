/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/source_control_commit.c
 *
 * PURPOSE:
 *   Commit already-staged changes. A model cannot commit without explicit
 *   source-control mutation policy and approval.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/source_control_commit.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_commit_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "source-control.commit",
        .label = "Commit Staged Changes",
        .description = "Commit staged changes with an explicit message.",
        .permission = "source-control.commit",
        .risk = UMI_AI_CODING_TOOL_RISK_SOURCE_CONTROL_MUTATION,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_WRITE,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_source_control_commit_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeJsonWriter writer;
    char message[1024];
    char commit_id[256];
    UmiStatus status;

    if (environment == NULL || environment->source_control == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_required_string(
            &document, "message", message, sizeof(message));
    }
    if (status != UMI_STATUS_OK) return status;
    if (message[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_developer_source_control_commit(
        environment->source_control,
        message,
        commit_id,
        sizeof(commit_id));
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"commitId\":");
    (void)umi_language_runtime_json_writer_string(&writer, commit_id);
    return umi_ai_coding_tool_write_status_end(&writer);
}
