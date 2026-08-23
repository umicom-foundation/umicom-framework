/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/checkpoint_list.c
 *
 * PURPOSE:
 *   Return checkpoint metadata without returning the retained source content.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/checkpoint_list.h"
#include "umicom/ai_coding_tools/checkpoint.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_checkpoint_list_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "workspace.checkpoint-list",
        .label = "List Workspace Checkpoints",
        .description = "List rollback checkpoint metadata without source content.",
        .permission = "workspace.checkpoint",
        .risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_CHECKPOINT,
        .requires_approval = 0,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_checkpoint_list_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonWriter writer;
    size_t index;
    UmiStatus status;

    (void)arguments_json;

    if (environment == NULL || environment->checkpoints == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_write_ok_begin(
        &writer, output, output_capacity);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"checkpoints\":[");

    for (index = 0U; index < environment->checkpoints->count; ++index) {
        const UmiAiCodingCheckpoint *checkpoint =
            &environment->checkpoints->checkpoints[index];

        if (index > 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }

        (void)umi_language_runtime_json_writer_raw(&writer, "{\"id\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, checkpoint->checkpoint_id);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"label\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, checkpoint->label);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"files\":");
        (void)umi_language_runtime_json_writer_uint64(
            &writer, checkpoint->file_count);
        (void)umi_language_runtime_json_writer_raw(&writer, "}");
    }

    (void)umi_language_runtime_json_writer_raw(&writer, "],\"count\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, environment->checkpoints->count);
    return umi_ai_coding_tool_write_status_end(&writer);
}
