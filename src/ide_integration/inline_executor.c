/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/inline_executor.c
 *
 * PURPOSE:
 *   Implement provider-backed inline code suggestion generation.
 *
 * SECURITY:
 *   This is generation only. The provider never writes to the editor. Acceptance
 *   crosses a separate revision-checked edit adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/inline_executor.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise ide inline executor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ide_inline_executor_init(
    UmiIdeInlineExecutor *executor,
    UmiAiRuntime *runtime,
    const char *provider_id,
    const char *model_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL || runtime == NULL ||
        provider_id == NULL || provider_id[0] == '\0' ||
        model_id == NULL || model_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(executor, 0, sizeof(*executor));
    executor->runtime = runtime;

    status = copy_text(
        executor->provider_id,
        sizeof(executor->provider_id),
        provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            executor->model_id,
            sizeof(executor->model_id),
            model_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    executor->max_output_tokens = 1024U;
    executor->temperature = 0.0;
    return UMI_STATUS_OK;
}

/*
 * Perform ide inline through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_ide_inline_execute(
    UmiIdeInlineExecutor *executor,
    const UmiIdeEditorSelection *context,
    uint64_t request_sequence,
    UmiIdeInlineSuggestion *out_suggestion)
{
    UmiAiRequest request;
    UmiAiMessage system_message;
    UmiAiMessage user_message;
    UmiAiResponse response;
    char prompt[UMI_AI_TEXT_CAPACITY];
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL || executor->runtime == NULL ||
        context == NULL || out_suggestion == NULL ||
        request_sequence == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_editor_selection_validate(context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    written = snprintf(
        prompt,
        sizeof(prompt),
        "Complete code at %.500s:%u:%u. Language=%.80s. "
        "Return only the text to insert, no markdown or explanation. "
        "Nearby selected/prefix text:\n%.1200s",
        context->path,
        context->end_line,
        context->end_column,
        context->language_id,
        context->text);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(prompt)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    umi_ai_request_init(&request);

    (void)snprintf(
        request.request_id,
        sizeof(request.request_id),
        "inline.%llu",
        (unsigned long long)request_sequence);
    (void)snprintf(
        request.model_id,
        sizeof(request.model_id),
        "%s",
        executor->model_id);

    request.max_output_tokens = executor->max_output_tokens;
    request.temperature = executor->temperature;
    request.allow_tools = 0;

    status = umi_ai_message_set(
        &system_message,
        UMI_AI_ROLE_SYSTEM,
        "inline-coding",
        "You are the Umicom inline coding completion engine. Return only code text.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_message_set(
            &user_message,
            UMI_AI_ROLE_USER,
            "editor",
            prompt);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_request_add_message(
            &request,
            &system_message);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_request_add_message(
            &request,
            &user_message);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_ai_response_init(&response);

    status = umi_ai_runtime_generate(
        executor->runtime,
        executor->provider_id,
        executor->provider_approved,
        &request,
        &response);

    (void)memset(out_suggestion, 0, sizeof(*out_suggestion));
    out_suggestion->request_sequence = request_sequence;
    out_suggestion->document_revision = context->document_revision;
    out_suggestion->line = context->end_line;
    out_suggestion->column = context->end_column;
    out_suggestion->status = status;

    (void)snprintf(
        out_suggestion->suggestion_id,
        sizeof(out_suggestion->suggestion_id),
        "inline.%llu",
        (unsigned long long)request_sequence);
    (void)snprintf(
        out_suggestion->document_id,
        sizeof(out_suggestion->document_id),
        "%s",
        context->document_id);
    (void)snprintf(
        out_suggestion->path,
        sizeof(out_suggestion->path),
        "%s",
        context->path);
    (void)snprintf(
        out_suggestion->language_id,
        sizeof(out_suggestion->language_id),
        "%s",
        context->language_id);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        out_suggestion->state = UMI_IDE_INLINE_FAILED;
        return status;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(response.text) >= sizeof(out_suggestion->text)) {
        out_suggestion->state = UMI_IDE_INLINE_FAILED;
        out_suggestion->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)strcpy(out_suggestion->text, response.text);
    out_suggestion->state = UMI_IDE_INLINE_READY;
    return UMI_STATUS_OK;
}
