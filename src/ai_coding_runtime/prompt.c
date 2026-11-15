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
#include "umicom/base/utf8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Provide the add message operation used by this module and its client applications. */
/* Reuse Framework Base UTF-8 validation before creating provider message strings. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus add_message(
    UmiAiRequest *request,
    UmiAiRole role,
    const char *name,
    const char *text)
{
    UmiAiMessage message;
    UmiStatus status;

    /* Provider-neutral message strings must be independently valid UTF-8. */
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiUtf8Validate((const unsigned char *)text, strlen(text), NULL) ||
        (name != NULL && !UmiUtf8Validate((const unsigned char *)name, strlen(name), NULL)))
        return UMI_STATUS_PARSE_ERROR;

    status = umi_ai_message_set(&message, role, name, text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_request_add_message(request, &message);
}

/* Provide the add chunked operation used by this module and its client applications. */
/* Preserve complete UTF-8 scalars at message boundaries while retaining identical source bytes and existing empty-text behavior. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus add_chunked(
    UmiAiRequest *request,
    const char *name,
    const char *text,
    size_t length)
{
    size_t offset = 0U;
    const size_t chunk_capacity = UMI_AI_TEXT_CAPACITY - 1U;
    /* Validate the full span using Base's canonical scalar decoder. Once
     * admitted, a continuation byte identifies a split inside a scalar. */
    if (!UmiUtf8Validate((const unsigned char *)text, length, NULL))
        return UMI_STATUS_PARSE_ERROR;
    while (offset < length) {
        char chunk[UMI_AI_TEXT_CAPACITY];
        const size_t remaining = length - offset;
        size_t count = remaining < chunk_capacity ? remaining : chunk_capacity;
        UmiStatus status;
        while (count != 0U && count < remaining &&
            (((unsigned char)text[offset + count] & 0xc0U) == 0x80U)) --count;
        if (count == 0U) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(chunk, text + offset, count);
        chunk[count] = '\0';
        status = add_message(request, UMI_AI_ROLE_USER, name, chunk);
        if (status != UMI_STATUS_OK) return status;
        offset += count;
    }
    if (length == 0U) return add_message(request, UMI_AI_ROLE_USER, name, "");
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

    /* The owned file already carries a validated byte extent. Retain the
     * former string call for review while avoiding a repeated length scan. */
#if 0
    status = umi_ai_coding_extract_selection(
        file.text,
        plan->request.selection_start_line,
        plan->request.selection_end_line,
        selection,
        sizeof(selection),
        &length);
#endif
    status = UmiAiCodingExtractSelectionSpan(
        file.text,
        file.length,
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
/* Framework now validates complete prompt inputs and stages request publication; the existing message builder is retained behind that boundary. The former implementation is retained for owner review. */
#if 0
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

#endif
static UmiStatus build_provider_request_candidate(
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


/* Compare the owned subset with its selection before constructing messages.
 * Matching revisions are provenance checks, not execution or network consent. */
static UmiStatus prompt_inputs_validate(const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingMaterializedContext *context,
    const UmiAiCodingRuntimeConfig *config)
{
    UmiStatus status = UmiAiCodingMaterializationPlanValidate(plan);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_request_validate(&plan->request);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_coding_runtime_config_validate(config);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiCodingMaterializedContextValidate(context);
    if (status != UMI_STATUS_OK) return status;
    /* IDs are serialized outside the message array and need the same text
     * contract. Their bounded terminators were checked by domain admission. */
    if (!UmiUtf8Validate((const unsigned char *)plan->request.request_id,
            strlen(plan->request.request_id), NULL) ||
        !UmiUtf8Validate((const unsigned char *)config->model_id, strlen(config->model_id), NULL))
        return UMI_STATUS_PARSE_ERROR;
    if (context->revision != plan->revision ||
        context->file_count + context->skipped_count != plan->repository_context.file_count)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < context->file_count; ++i) {
        const UmiAiCodingMaterializedFile *file = &context->files[i];
        const UmiAiCodingContextFile *selected = NULL;
        for (size_t j = 0U; j < plan->repository_context.file_count; ++j) {
            if (strcmp(file->path, plan->repository_context.files[j].path) == 0) {
                selected = &plan->repository_context.files[j];
                break;
            }
        }
        if (selected == NULL || file->estimated_tokens != selected->estimated_tokens ||
            strcmp(file->language_id, selected->language_id) != 0 ||
            !!file->active != (plan->request.active_path[0] != '\0' &&
                strcmp(file->path, plan->request.active_path) == 0))
            return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* No valid request can contain more feedback bytes than all message slots
 * combined. This ceiling bounds the compatibility string scan without
 * narrowing any feedback that could previously fit a complete request. */
static size_t feedback_capacity(void)
{
    return (size_t)UMI_AI_MAX_MESSAGES * (UMI_AI_TEXT_CAPACITY - 1U);
}

UmiStatus UmiAiCodingBuildProviderRequestWithFeedback(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingMaterializedContext *context,
    const UmiAiCodingRuntimeConfig *config,
    const char *feedback,
    size_t feedback_length,
    UmiAiRequest *out_request)
{
    UmiAiRequest *candidate;
    char *owned_feedback = NULL;
    UmiStatus status;
    if (out_request == NULL || (feedback == NULL && feedback_length != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (feedback_length > feedback_capacity()) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (feedback_length != 0U && memchr(feedback, '\0', feedback_length) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = prompt_inputs_validate(plan, context, config);
    if (status != UMI_STATUS_OK) return status;
    candidate = malloc(sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (feedback_length != 0U) {
        owned_feedback = malloc(feedback_length + 1U);
        if (owned_feedback == NULL) {
            free(candidate);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        memcpy(owned_feedback, feedback, feedback_length);
        owned_feedback[feedback_length] = '\0';
    }
    /* The retained builder preserves established message order and chunking.
     * All borrowed inputs, including aliases into the previous destination,
     * remain readable until it has completed. */
    status = build_provider_request_candidate(plan, context, config, owned_feedback, candidate);
    if (status == UMI_STATUS_OK) memcpy(out_request, candidate, sizeof *out_request);
    free(owned_feedback);
    free(candidate);
    return status;
}

UmiStatus umi_ai_coding_build_provider_request(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingMaterializedContext *context,
    const UmiAiCodingRuntimeConfig *config,
    const char *validation_feedback,
    UmiAiRequest *out_request)
{
    size_t length = 0U;
    if (out_request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (validation_feedback != NULL) {
        while (length <= feedback_capacity() && validation_feedback[length] != '\0') ++length;
        if (length > feedback_capacity()) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UmiAiCodingBuildProviderRequestWithFeedback(
        plan, context, config, validation_feedback, length, out_request);
}
