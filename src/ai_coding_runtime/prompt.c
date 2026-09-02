/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/prompt.c
 *
 * PURPOSE:
 *   Build bounded provider messages while keeping repository context provenance
 *   visible and splitting large source files across owned message records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/prompt.h"

#include <stdio.h>
#include <string.h>

/* Provide the add message operation used by this module and its client applications. */
static UmiStatus add_message(
    UmiAiRequest *request,
    UmiAiRole role,
    const char *name,
    const char *text)
{
    UmiAiMessage message;
    UmiStatus status;

    status = umi_ai_message_set(&message, role, name, text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_request_add_message(request, &message);
}

/* Provide the add chunked operation used by this module and its client applications. */
static UmiStatus add_chunked(
    UmiAiRequest *request,
    const char *name,
    const char *text,
    size_t length)
{
    size_t offset = 0U;
    const size_t chunk_capacity = UMI_AI_TEXT_CAPACITY - 1U;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (offset < length) {
        char chunk[UMI_AI_TEXT_CAPACITY];
        const size_t remaining = length - offset;
        const size_t count =
            remaining < chunk_capacity ? remaining : chunk_capacity;
        UmiStatus status;

        (void)memcpy(chunk, text + offset, count);
        chunk[count] = '\0';

        status = add_message(
            request,
            UMI_AI_ROLE_USER,
            name,
            chunk);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        offset += count;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U) {
        return add_message(
            request,
            UMI_AI_ROLE_USER,
            name,
            "");
    }

    return UMI_STATUS_OK;
}

/* Provide the add context file operation used by this module and its client applications. */
static UmiStatus add_context_file(
    UmiAiRequest *request,
    const UmiAiCodingMaterializedFile *file)
{
    /*
     * UmiAiMessage.name carries the repository-relative path. Keeping source
     * text in chunk messages avoids spending an additional message solely on
     * metadata and preserves request capacity for actual code.
     */
    return add_chunked(
        request,
        file->path,
        file->text,
        file->length);
}

/*
 * Provide the add active selection operation used by this module and its client
 * applications.
 */
static UmiStatus add_active_selection(
    UmiAiRequest *request,
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingMaterializedContext *context)
{
    UmiAiCodingMaterializedFile file;
    char selection[UMI_AI_TEXT_CAPACITY];
    size_t length = 0U;
    UmiStatus status;

    /* Apply this operation only while the related capability or state is available. */
    if (plan->request.active_path[0] == '\0' ||
        plan->request.selection_start_line == 0U ||
        plan->request.selection_end_line <
            plan->request.selection_start_line) {
        return UMI_STATUS_OK;
    }

    status = umi_ai_coding_materialized_context_find(
        context,
        plan->request.active_path,
        &file);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_extract_selection(
        file.text,
        plan->request.selection_start_line,
        plan->request.selection_end_line,
        selection,
        sizeof(selection),
        &length);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND ||
        status == UMI_STATUS_CAPACITY_EXCEEDED) {
        return UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return add_chunked(
        request,
        "active-selection",
        selection,
        length);
}

/*
 * Provide the ai coding build provider request operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_build_provider_request(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingMaterializedContext *context,
    const UmiAiCodingRuntimeConfig *config,
    const char *validation_feedback,
    UmiAiRequest *out_request)
{
    const char *system_prompt;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || context == NULL ||
        config == NULL || out_request == NULL ||
        !plan->ready) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_runtime_config_validate(config);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    system_prompt = umi_ai_coding_prompt_system(plan->request.task);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (system_prompt == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_ai_request_init(out_request);

    (void)snprintf(
        out_request->request_id,
        sizeof(out_request->request_id),
        "%s",
        plan->request.request_id);
    (void)snprintf(
        out_request->model_id,
        sizeof(out_request->model_id),
        "%s",
        config->model_id);
    out_request->max_output_tokens = config->max_output_tokens;
    out_request->temperature = config->temperature;
    out_request->allow_tools = config->allow_tools;

    status = add_message(
        out_request,
        UMI_AI_ROLE_SYSTEM,
        "umicom-coding-policy",
        system_prompt);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ai_coding_prompt_task_may_modify(plan->request.task)) {
        status = add_message(
            out_request,
            UMI_AI_ROLE_SYSTEM,
            "umicom-response-protocol",
            umi_ai_coding_prompt_response_protocol());
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    status = add_message(
        out_request,
        UMI_AI_ROLE_USER,
        "instruction",
        plan->request.instruction);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = add_active_selection(out_request, plan, context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < context->file_count; ++index) {
        status = add_context_file(out_request, &context->files[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (validation_feedback != NULL &&
        validation_feedback[0] != '\0') {
        status = add_chunked(
            out_request,
            "validation-feedback",
            validation_feedback,
            strlen(validation_feedback));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
