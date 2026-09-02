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

/*
 * The path check is deliberately independent of the host operating system.
 * An application may use either slash style, but an AI response cannot escape
 * the workspace through an absolute path, drive prefix or '..' segment.
 */
#include "umicom/ai/coding_types.h"

#include <ctype.h>
#include <string.h>

/*
 * Initialise ai coding request from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_coding_request_init(UmiAiCodingRequest *request,
                                UmiAiCodingTaskKind task)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->structure_size = (uint32_t)sizeof(*request);
    request->abi_version = UMI_AI_CODING_ABI_VERSION;
    request->task = task;
    request->classification = UMI_AI_DATA_INTERNAL;
    request->context_token_budget = 16384U;
    request->maximum_context_files = UMI_AI_CODING_CONTEXT_PLAN_MAX;
}

/* Check that task satisfies its contract before another service relies on it. */
static int task_valid(UmiAiCodingTaskKind task)
{
    return task >= UMI_AI_CODING_TASK_CHAT &&
           task <= UMI_AI_CODING_TASK_GENERATE_TESTS;
}

/* Check that ai coding request satisfies its contract before another service relies on it. */
UmiStatus umi_ai_coding_request_validate(const UmiAiCodingRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this operation only while the related capability or state is available. */
    if (request->active_path[0] != '\0' &&
        !umi_ai_coding_path_is_safe_relative(request->active_path)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding path is safe relative operation used by this module and its client
 * applications.
 */
int umi_ai_coding_path_is_safe_relative(const char *path)
{
    const char *segment;
    const char *cursor;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0' || path[0] == '/' || path[0] == '\\') {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (isalpha((unsigned char)path[0]) && path[1] == ':') return 0;
    segment = path;
    cursor = path;
    /* Visit each bounded item once so every record receives the same rule. */
    for (;;) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '/' || *cursor == '\\' || *cursor == '\0') {
            length = (size_t)(cursor - segment);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (length == 0U ||
                (length == 1U && segment[0] == '.') ||
                (length == 2U && segment[0] == '.' && segment[1] == '.')) {
                return 0;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (*cursor == '\0') break;
            segment = cursor + 1;
        } else /* Apply this branch only when its contract condition is satisfied. */ if ((unsigned char)*cursor < 32U || *cursor == ':') {
            return 0;
        }
        ++cursor;
    }
    return 1;
}

/*
 * Provide the ai coding text hash operation used by this module and its client
 * applications.
 */
uint64_t umi_ai_coding_text_hash(const char *text, size_t length)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL && length != 0U) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

/*
 * Provide the ai coding task kind text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_task_kind_text(UmiAiCodingTaskKind task)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (task) {
        case UMI_AI_CODING_TASK_CHAT: return "code chat";
        case UMI_AI_CODING_TASK_COMPLETE: return "completion";
        case UMI_AI_CODING_TASK_EXPLAIN: return "explanation";
        case UMI_AI_CODING_TASK_REFACTOR: return "refactoring";
        case UMI_AI_CODING_TASK_GENERATE_TESTS: return "test generation";
        default: return "unknown";
    }
}

/*
 * Provide the ai coding patch operation text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_patch_operation_text(
    UmiAiCodingPatchOperation operation)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (operation) {
        case UMI_AI_CODING_PATCH_CREATE: return "create";
        case UMI_AI_CODING_PATCH_MODIFY: return "modify";
        case UMI_AI_CODING_PATCH_DELETE: return "delete";
        default: return "unknown";
    }
}

/*
 * Provide the ai coding patch state text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_patch_state_text(UmiAiCodingPatchState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_AI_CODING_PATCH_DRAFT: return "draft";
        case UMI_AI_CODING_PATCH_APPROVED: return "approved";
        case UMI_AI_CODING_PATCH_APPLIED: return "applied";
        case UMI_AI_CODING_PATCH_REVERTED: return "reverted";
        case UMI_AI_CODING_PATCH_REJECTED: return "rejected";
        default: return "unknown";
    }
}
