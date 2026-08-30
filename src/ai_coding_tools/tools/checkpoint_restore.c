/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/checkpoint_restore.c
 *
 * PURPOSE:
 *   Restore complete-file checkpoint state. This is deliberately a local
 *   mutation and requires explicit policy plus approval.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/checkpoint_restore.h"
#include "umicom/ai_coding_tools/checkpoint.h"
#include "../tool_support.h"

const UmiAiCodingToolDescriptor *umi_ai_coding_tool_checkpoint_restore_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "workspace.checkpoint-restore",
        .label = "Restore Workspace Checkpoint",
        .description = "Restore complete-file state captured by a checkpoint.",
        .permission = "workspace.checkpoint.restore",
        .risk = UMI_AI_CODING_TOOL_RISK_LOCAL_MUTATION,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_CHECKPOINT,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

UmiStatus umi_ai_coding_tool_checkpoint_restore_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    char id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    UmiStatus status;

    if (environment == NULL || environment->checkpoints == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    if (!environment->policy.allow_checkpoint_restore) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_required_string(
            &document, "id", id, sizeof(id));
    }
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_checkpoint_restore(
        environment->checkpoints,
        &environment->workspace,
        id);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output,
        output_capacity,
        UMI_STATUS_OK,
        "Workspace checkpoint restored.");
}
