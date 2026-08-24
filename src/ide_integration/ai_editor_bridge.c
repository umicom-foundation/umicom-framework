/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/ai_editor_bridge.c
 *
 * PURPOSE:
 *   Implement active-editor to governed coding-request projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/ai_editor_bridge.h"

#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;

    if (out == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    length = strlen(text);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ide_ai_request_from_selection(
    const UmiIdeEditorSelection *selection,
    UmiAiCodingTaskKind task_kind,
    const char *request_id,
    const char *session_id,
    const char *workspace_root,
    const char *instruction,
    UmiAiCodingRequest *out_request)
{
    UmiStatus status;

    if (selection == NULL || request_id == NULL ||
        session_id == NULL || workspace_root == NULL ||
        instruction == NULL || out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_editor_selection_validate(selection);
    if (status != UMI_STATUS_OK) return status;

    umi_ai_coding_request_init(out_request, task_kind);

    status = copy_text(
        out_request->request_id,
        sizeof(out_request->request_id),
        request_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->session_id,
            sizeof(out_request->session_id),
            session_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->workspace_root,
            sizeof(out_request->workspace_root),
            workspace_root);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->active_path,
            sizeof(out_request->active_path),
            selection->path);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->language_id,
            sizeof(out_request->language_id),
            selection->language_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_request->instruction,
            sizeof(out_request->instruction),
            instruction);
    }
    if (status != UMI_STATUS_OK) return status;

    out_request->selection_start_line = selection->start_line;
    out_request->selection_end_line = selection->end_line;
    out_request->context_token_budget = 8192U;
    out_request->maximum_context_files = 24U;

    return umi_ai_coding_request_validate(out_request);
}
