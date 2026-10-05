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
#include "../base/value_archive_internal.h"

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
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->request_id, '\0', sizeof(request->request_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->session_id, '\0', sizeof(request->session_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->runtime_id, '\0', sizeof(request->runtime_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->workspace_root, '\0', sizeof(request->workspace_root)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->active_path, '\0', sizeof(request->active_path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->language_id, '\0', sizeof(request->language_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->instruction, '\0', sizeof(request->instruction)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

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

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAiCodingRequestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x243d644547ad46bb);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiCodingRequest *)0)->request_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiCodingRequest *)0)->session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiCodingRequest *)0)->runtime_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiCodingRequest *)0)->workspace_root)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiCodingRequest *)0)->active_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiCodingRequest *)0)->language_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiCodingRequest *)0)->instruction)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAiCodingRequestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiAiCodingRequest *)0)->request_id) - 1U +
        8U + sizeof(((UmiAiCodingRequest *)0)->session_id) - 1U +
        8U + sizeof(((UmiAiCodingRequest *)0)->runtime_id) - 1U +
        8U + sizeof(((UmiAiCodingRequest *)0)->workspace_root) - 1U +
        8U + sizeof(((UmiAiCodingRequest *)0)->active_path) - 1U +
        8U + sizeof(((UmiAiCodingRequest *)0)->language_id) - 1U +
        8U + sizeof(((UmiAiCodingRequest *)0)->instruction) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAiCodingRequestArchiveWrite(UmiArchiveWriter *writer, const UmiAiCodingRequest *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->abi_version);
    UmiArchiveWriteSigned(writer, (int64_t)value->task);
    UmiArchiveWriteSigned(writer, (int64_t)value->classification);
    UmiArchiveWriteText(writer, value->request_id, sizeof(value->request_id));
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteText(writer, value->runtime_id, sizeof(value->runtime_id));
    UmiArchiveWriteText(writer, value->workspace_root, sizeof(value->workspace_root));
    UmiArchiveWriteText(writer, value->active_path, sizeof(value->active_path));
    UmiArchiveWriteText(writer, value->language_id, sizeof(value->language_id));
    UmiArchiveWriteText(writer, value->instruction, sizeof(value->instruction));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->selection_start_line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->selection_end_line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->context_token_budget);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_context_files);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ns);
    UmiArchiveWriteSigned(writer, (int64_t)value->sensitive_approved);
}
static void UmiAiCodingRequestArchiveRead(UmiArchiveReader *reader, UmiAiCodingRequest *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    value->abi_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->task = (UmiAiCodingTaskKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->classification = (UmiAiDataClassification)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->request_id, sizeof(value->request_id));
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    UmiArchiveReadText(reader, value->runtime_id, sizeof(value->runtime_id));
    UmiArchiveReadText(reader, value->workspace_root, sizeof(value->workspace_root));
    UmiArchiveReadText(reader, value->active_path, sizeof(value->active_path));
    UmiArchiveReadText(reader, value->language_id, sizeof(value->language_id));
    UmiArchiveReadText(reader, value->instruction, sizeof(value->instruction));
    value->selection_start_line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->selection_end_line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->context_token_budget = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->maximum_context_files = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->timestamp_ns = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->sensitive_approved = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAiCodingRequestArchiveValidate(const UmiAiCodingRequest *value)
{
    return umi_ai_coding_request_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ai_coding_request_archive_encode, umi_ai_coding_request_archive_decode,
    UmiAiCodingRequest, UmiAiCodingRequestArchiveSchema, UmiAiCodingRequestArchiveBound, UmiAiCodingRequestArchiveWrite, UmiAiCodingRequestArchiveRead, UmiAiCodingRequestArchiveValidate)
