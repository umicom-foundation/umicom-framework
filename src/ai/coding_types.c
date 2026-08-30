/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/coding_types.c
 *
 * PURPOSE:
 *   Validate coding requests, repository-relative paths and stable text hashes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The path check is deliberately independent of the host operating system.
 * An application may use either slash style, but an AI response cannot escape
 * the workspace through an absolute path, drive prefix or '..' segment.
 */
#include "umicom/ai/coding_types.h"

#include <ctype.h>
#include <string.h>

void umi_ai_coding_request_init(UmiAiCodingRequest *request,
                                UmiAiCodingTaskKind task)
{
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->structure_size = (uint32_t)sizeof(*request);
    request->abi_version = UMI_AI_CODING_ABI_VERSION;
    request->task = task;
    request->classification = UMI_AI_DATA_INTERNAL;
    request->context_token_budget = 16384U;
    request->maximum_context_files = UMI_AI_CODING_CONTEXT_PLAN_MAX;
}

static int task_valid(UmiAiCodingTaskKind task)
{
    return task >= UMI_AI_CODING_TASK_CHAT &&
           task <= UMI_AI_CODING_TASK_GENERATE_TESTS;
}

UmiStatus umi_ai_coding_request_validate(const UmiAiCodingRequest *request)
{
    if (request == NULL ||
        request->structure_size < sizeof(UmiAiCodingRequest) ||
        request->abi_version != UMI_AI_CODING_ABI_VERSION ||
        !task_valid(request->task) || request->request_id[0] == '\0' ||
        request->session_id[0] == '\0' || request->runtime_id[0] == '\0' ||
        request->workspace_root[0] == '\0' || request->instruction[0] == '\0' ||
        request->context_token_budget == 0U ||
        request->maximum_context_files == 0U ||
        request->maximum_context_files > UMI_AI_CODING_CONTEXT_PLAN_MAX ||
        request->selection_end_line < request->selection_start_line) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (request->active_path[0] != '\0' &&
        !umi_ai_coding_path_is_safe_relative(request->active_path)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}

int umi_ai_coding_path_is_safe_relative(const char *path)
{
    const char *segment;
    const char *cursor;
    size_t length;
    if (path == NULL || path[0] == '\0' || path[0] == '/' || path[0] == '\\') {
        return 0;
    }
    if (isalpha((unsigned char)path[0]) && path[1] == ':') return 0;
    segment = path;
    cursor = path;
    for (;;) {
        if (*cursor == '/' || *cursor == '\\' || *cursor == '\0') {
            length = (size_t)(cursor - segment);
            if (length == 0U ||
                (length == 1U && segment[0] == '.') ||
                (length == 2U && segment[0] == '.' && segment[1] == '.')) {
                return 0;
            }
            if (*cursor == '\0') break;
            segment = cursor + 1;
        } else if ((unsigned char)*cursor < 32U || *cursor == ':') {
            return 0;
        }
        ++cursor;
    }
    return 1;
}

uint64_t umi_ai_coding_text_hash(const char *text, size_t length)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t index;
    if (text == NULL && length != 0U) return 0U;
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

const char *umi_ai_coding_task_kind_text(UmiAiCodingTaskKind task)
{
    switch (task) {
        case UMI_AI_CODING_TASK_CHAT: return "code chat";
        case UMI_AI_CODING_TASK_COMPLETE: return "completion";
        case UMI_AI_CODING_TASK_EXPLAIN: return "explanation";
        case UMI_AI_CODING_TASK_REFACTOR: return "refactoring";
        case UMI_AI_CODING_TASK_GENERATE_TESTS: return "test generation";
        default: return "unknown";
    }
}

const char *umi_ai_coding_patch_operation_text(
    UmiAiCodingPatchOperation operation)
{
    switch (operation) {
        case UMI_AI_CODING_PATCH_CREATE: return "create";
        case UMI_AI_CODING_PATCH_MODIFY: return "modify";
        case UMI_AI_CODING_PATCH_DELETE: return "delete";
        default: return "unknown";
    }
}

const char *umi_ai_coding_patch_state_text(UmiAiCodingPatchState state)
{
    switch (state) {
        case UMI_AI_CODING_PATCH_DRAFT: return "draft";
        case UMI_AI_CODING_PATCH_APPROVED: return "approved";
        case UMI_AI_CODING_PATCH_APPLIED: return "applied";
        case UMI_AI_CODING_PATCH_REVERTED: return "reverted";
        case UMI_AI_CODING_PATCH_REJECTED: return "rejected";
        default: return "unknown";
    }
}
