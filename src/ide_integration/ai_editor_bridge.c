/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/ai_editor_bridge.c
 *
 * PURPOSE:
 *   Implement active-editor to governed coding-request projection.
 *
 * ARCHITECTURE:
 *   Current AI coding requests require request, session and runtime identities.
 *   The explicit runtime-aware bridge is authoritative. The established API is
 *   retained as a compatibility wrapper and uses session_id as a stable runtime
 *   identity when an older caller has no separate runtime identifier to supply.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/ai_editor_bridge.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the ide ai request from selection runtime operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_ai_request_from_selection_runtime(
    const UmiIdeEditorSelection *selection,
    UmiAiCodingTaskKind task_kind,
    const char *request_id,
    const char *session_id,
    const char *runtime_id,
    const char *workspace_root,
    const char *instruction,
    UmiAiCodingRequest *out_request)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || request_id == NULL ||
        session_id == NULL || runtime_id == NULL ||
        workspace_root == NULL || instruction == NULL ||
        out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_editor_selection_validate(selection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_ai_coding_request_init(out_request, task_kind);

    status = copy_text(
        out_request->request_id,
        sizeof(out_request->request_id),
        request_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->session_id,
            sizeof(out_request->session_id),
            session_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->runtime_id,
            sizeof(out_request->runtime_id),
            runtime_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->workspace_root,
            sizeof(out_request->workspace_root),
            workspace_root);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->active_path,
            sizeof(out_request->active_path),
            selection->path);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->language_id,
            sizeof(out_request->language_id),
            selection->language_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->instruction,
            sizeof(out_request->instruction),
            instruction);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_request->selection_start_line = selection->start_line;
    out_request->selection_end_line = selection->end_line;
    out_request->context_token_budget = 8192U;
    out_request->maximum_context_files = 24U;

    return umi_ai_coding_request_validate(out_request);
}

/*
 * Provide the ide ai request from selection operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_ai_request_from_selection(
    const UmiIdeEditorSelection *selection,
    UmiAiCodingTaskKind task_kind,
    const char *request_id,
    const char *session_id,
    const char *workspace_root,
    const char *instruction,
    UmiAiCodingRequest *out_request)
{
    /*
     * Compatibility path: before runtime_id became mandatory this bridge only
     * received session identity. Reusing the non-empty session identifier is a
     * deterministic, auditable default and keeps all existing callers working.
     */
    return umi_ide_ai_request_from_selection_runtime(
        selection,
        task_kind,
        request_id,
        session_id,
        session_id,
        workspace_root,
        instruction,
        out_request);
}
