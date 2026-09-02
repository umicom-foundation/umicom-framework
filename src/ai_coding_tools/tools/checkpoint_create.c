/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/tools/checkpoint_create.c
 *
 * PURPOSE:
 *   Capture a named complete-file checkpoint through the controlled workspace
 *   adapter. The call requires explicit approval because source contents are
 *   retained for future rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/tools/checkpoint_create.h"
#include "umicom/ai_coding_tools/checkpoint.h"
#include "../tool_support.h"

#include <string.h>

/*
 * Provide the ai coding tool checkpoint create descriptor operation used by this module
 * and its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_checkpoint_create_descriptor(void)
{
    static const UmiAiCodingToolDescriptor descriptor = {
        .tool_id = "workspace.checkpoint-create",
        .label = "Create Workspace Checkpoint",
        .description = "Capture bounded complete-file state for rollback.",
        .permission = "workspace.checkpoint",
        .risk = UMI_AI_CODING_TOOL_RISK_LOCAL_MUTATION,
        .required_capabilities = UMI_AI_CODING_TOOL_CAP_CHECKPOINT |
                                 UMI_AI_CODING_TOOL_CAP_WORKSPACE_READ,
        .requires_approval = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &descriptor;
}

/*
 * Provide the ai coding tool checkpoint create invoke operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tool_checkpoint_create_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiCodingToolEnvironment *environment =
        (UmiAiCodingToolEnvironment *)user_data;
    UmiLanguageRuntimeJsonDocument document;
    char id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    char label[256];
    char path_storage[UMI_AI_CODING_TOOL_CHECKPOINT_FILE_CAPACITY]
                     [UMI_AI_TEXT_CAPACITY];
    const char *paths[UMI_AI_CODING_TOOL_CHECKPOINT_FILE_CAPACITY];
    int array_token;
    size_t count;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || environment->checkpoints == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tool_json_parse_object(arguments_json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_required_string(
            &document, "id", id, sizeof(id));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_tool_json_optional_string(
            &document, "label", id, label, sizeof(label));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    array_token = umi_language_runtime_json_object_get(
        &document, 0, "paths");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (array_token < 0) return UMI_STATUS_NOT_FOUND;

    count = umi_language_runtime_json_array_count(
        &document, array_token);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count == 0U ||
        count > UMI_AI_CODING_TOOL_CHECKPOINT_FILE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        int token = umi_language_runtime_json_array_at(
            &document, array_token, index);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (token < 0) return UMI_STATUS_PARSE_ERROR;

        status = umi_language_runtime_json_string(
            &document,
            token,
            path_storage[index],
            sizeof(path_storage[index]));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        {
            char normalized[UMI_AI_TEXT_CAPACITY];

            status = umi_ai_coding_runtime_path_normalize_relative(
                path_storage[index],
                normalized,
                sizeof(normalized));
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;

            (void)strcpy(path_storage[index], normalized);
        }

        paths[index] = path_storage[index];
    }

    status = umi_ai_coding_checkpoint_capture(
        environment->checkpoints,
        &environment->workspace,
        id,
        label,
        paths,
        count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_tool_json_status(
        output,
        output_capacity,
        UMI_STATUS_OK,
        "Workspace checkpoint captured.");
}
