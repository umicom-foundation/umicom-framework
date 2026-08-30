/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/approval_persistence.c
 *
 * PURPOSE:
 *   Implement bounded approval persistence. Tool arguments are chunked and
 *   capped independently so one oversized request cannot exhaust Session Store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/approval_persistence.h"

#include <stdio.h>
#include <string.h>

static UmiStatus make_key(
    const char *prefix,
    const char *suffix,
    char *out_key,
    size_t capacity)
{
    const int written = snprintf(out_key, capacity, "%s.%s", prefix, suffix);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

static UmiStatus set_string(
    UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    const char *value)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_set(store, key, value);
}

static UmiStatus get_string(
    const UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    char *out_value,
    size_t capacity)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_get(store, key, out_value, capacity);
}

static UmiStatus save_text_field(
    UmiSessionStore *store,
    const char *prefix,
    const char *field,
    const char *text)
{
    char text_prefix[UMI_SESSION_KEY_CAPACITY];
    char count_key[UMI_SESSION_KEY_CAPACITY];
    size_t chunks = 0U;
    int written;
    UmiStatus status;

    written = snprintf(
        text_prefix,
        sizeof(text_prefix),
        "%s.%s",
        prefix,
        field);
    if (written < 0 || (size_t)written >= sizeof(text_prefix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_ai_developer_persistence_save_text(
        store,
        text_prefix,
        text,
        strlen(text),
        &chunks);
    if (status != UMI_STATUS_OK) return status;

    status = make_key(text_prefix, "count", count_key, sizeof(count_key));
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_persistence_set_uint64(
        store, count_key, chunks);
}

static UmiStatus load_text_field(
    const UmiSessionStore *store,
    const char *prefix,
    const char *field,
    char *out_text,
    size_t capacity)
{
    char text_prefix[UMI_SESSION_KEY_CAPACITY];
    char count_key[UMI_SESSION_KEY_CAPACITY];
    uint64_t chunks = 0U;
    size_t length = 0U;
    int written;
    UmiStatus status;

    written = snprintf(
        text_prefix,
        sizeof(text_prefix),
        "%s.%s",
        prefix,
        field);
    if (written < 0 || (size_t)written >= sizeof(text_prefix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = make_key(text_prefix, "count", count_key, sizeof(count_key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_get_uint64(
            store, count_key, 0U, &chunks);
    }
    if (status != UMI_STATUS_OK || chunks == 0U) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_PARSE_ERROR;
    }

    return umi_ai_developer_persistence_load_text(
        store,
        text_prefix,
        (size_t)chunks,
        out_text,
        capacity,
        &length);
}

static UmiStatus set_number(
    UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    uint64_t value)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_developer_persistence_set_uint64(store, key, value);
}

static UmiStatus get_number(
    const UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    uint64_t *out_value)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_developer_persistence_get_uint64(
        store, key, 0U, out_value);
}

static UmiStatus save_request(
    UmiSessionStore *store,
    const char *prefix,
    const UmiAiDeveloperApprovalRequest *request)
{
    char arguments_prefix[UMI_SESSION_KEY_CAPACITY];
    size_t chunks = 0U;
    UmiStatus status;

    status = set_string(store, prefix, "id", request->approval_id);
    if (status == UMI_STATUS_OK)
        status = save_text_field(store, prefix, "title", request->title);
    if (status == UMI_STATUS_OK)
        status = save_text_field(store, prefix, "summary", request->summary);
    if (status == UMI_STATUS_OK)
        status = set_string(store, prefix, "permission", request->permission);
    if (status == UMI_STATUS_OK)
        status = set_string(store, prefix, "subject", request->subject_id);
    if (status != UMI_STATUS_OK) return status;

    status = set_number(store, prefix, "kind", request->kind);
    if (status == UMI_STATUS_OK)
        status = set_number(store, prefix, "state", request->state);
    if (status == UMI_STATUS_OK)
        status = set_number(store, prefix, "risk", request->risk);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "requested", request->requested_sequence);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "decided", request->decided_sequence);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "executable",
            request->executable ? 1U : 0U);
    if (status != UMI_STATUS_OK) return status;

    {
        int written = snprintf(
            arguments_prefix,
            sizeof(arguments_prefix),
            "%s.arguments",
            prefix);

        if (written < 0 ||
            (size_t)written >= sizeof(arguments_prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }

    if (strlen(request->tool_call.arguments_json) >
        UMI_AI_DEVELOPER_APPROVAL_ARGUMENT_CHUNKS *
        UMI_AI_DEVELOPER_PERSISTENCE_CHUNK_BYTES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_ai_developer_persistence_save_text(
        store,
        arguments_prefix,
        request->tool_call.arguments_json,
        strlen(request->tool_call.arguments_json),
        &chunks);
    if (status != UMI_STATUS_OK) return status;

    status = set_number(store, prefix, "argumentChunks", chunks);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "toolCallId", request->tool_call.call_id);
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, prefix, "toolId", request->tool_call.tool_id);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "toolSequence", request->tool_call.sequence);

    return status;
}

static UmiStatus restore_request(
    const UmiSessionStore *store,
    const char *prefix,
    UmiAiDeveloperApprovalRequest *request)
{
    char arguments_prefix[UMI_SESSION_KEY_CAPACITY];
    uint64_t value = 0U;
    uint64_t chunks = 0U;
    size_t length = 0U;
    UmiStatus status;

    (void)memset(request, 0, sizeof(*request));

    status = get_string(
        store, prefix, "id",
        request->approval_id, sizeof(request->approval_id));
    if (status == UMI_STATUS_OK)
        status = load_text_field(
            store, prefix, "title",
            request->title, sizeof(request->title));
    if (status == UMI_STATUS_OK)
        status = load_text_field(
            store, prefix, "summary",
            request->summary, sizeof(request->summary));
    if (status == UMI_STATUS_OK)
        status = get_string(
            store, prefix, "permission",
            request->permission, sizeof(request->permission));
    if (status == UMI_STATUS_OK)
        status = get_string(
            store, prefix, "subject",
            request->subject_id, sizeof(request->subject_id));
    if (status != UMI_STATUS_OK) return status;

    status = get_number(store, prefix, "kind", &value);
    if (status != UMI_STATUS_OK) return status;
    request->kind = (UmiAiDeveloperApprovalKind)value;

    status = get_number(store, prefix, "state", &value);
    if (status != UMI_STATUS_OK) return status;
    request->state = (UmiAiDeveloperApprovalState)value;

    status = get_number(store, prefix, "risk", &value);
    if (status != UMI_STATUS_OK) return status;
    request->risk = (UmiAiCodingToolRisk)value;

    status = get_number(
        store, prefix, "requested", &request->requested_sequence);
    if (status == UMI_STATUS_OK)
        status = get_number(
            store, prefix, "decided", &request->decided_sequence);
    if (status != UMI_STATUS_OK) return status;

    status = get_number(store, prefix, "executable", &value);
    if (status != UMI_STATUS_OK) return status;
    request->executable = value != 0U;

    status = get_number(store, prefix, "argumentChunks", &chunks);
    if (status != UMI_STATUS_OK) return status;

    if (chunks > UMI_AI_DEVELOPER_APPROVAL_ARGUMENT_CHUNKS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    {
        int written = snprintf(
            arguments_prefix,
            sizeof(arguments_prefix),
            "%s.arguments",
            prefix);

        if (written < 0 ||
            (size_t)written >= sizeof(arguments_prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }

    status = umi_ai_developer_persistence_load_text(
        store,
        arguments_prefix,
        (size_t)chunks,
        request->tool_call.arguments_json,
        sizeof(request->tool_call.arguments_json),
        &length);
    if (status != UMI_STATUS_OK) return status;

    (void)length;

    status = get_number(
        store, prefix, "toolCallId", &request->tool_call.call_id);
    if (status == UMI_STATUS_OK)
        status = get_string(
            store, prefix, "toolId",
            request->tool_call.tool_id,
            sizeof(request->tool_call.tool_id));
    if (status == UMI_STATUS_OK)
        status = get_number(
            store, prefix, "toolSequence",
            &request->tool_call.sequence);

    request->tool_call.approved = 0;
    request->tool_call.state = UMI_AI_CODING_TOOL_CALL_PENDING;
    request->tool_call.status = UMI_STATUS_OK;
    return status;
}

UmiStatus umi_ai_developer_approvals_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiDeveloperApprovalQueue *queue)
{
    char count_key[UMI_SESSION_KEY_CAPACITY];
    size_t total;
    size_t count;
    size_t first;
    size_t index;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL || queue == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    total = umi_ai_developer_approval_queue_count(queue);
    count =
        total > UMI_AI_DEVELOPER_PERSISTED_APPROVALS
            ? UMI_AI_DEVELOPER_PERSISTED_APPROVALS
            : total;
    first = total - count;

    status = make_key(key_prefix, "count", count_key, sizeof(count_key));
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_persistence_set_uint64(
            store, count_key, count);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < count; ++index) {
        UmiAiDeveloperApprovalRequest request;
        char prefix[UMI_SESSION_KEY_CAPACITY];
        int written;

        status = umi_ai_developer_approval_queue_at(
            queue, first + index, &request);
        if (status != UMI_STATUS_OK) return status;

        written = snprintf(
            prefix,
            sizeof(prefix),
            "%s.a%zu",
            key_prefix,
            index);
        if (written < 0 || (size_t)written >= sizeof(prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = save_request(store, prefix, &request);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_approvals_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiDeveloperApprovalQueue *queue,
    size_t *out_restored_count)
{
    char count_key[UMI_SESSION_KEY_CAPACITY];
    uint64_t count = 0U;
    size_t index;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL ||
        queue == NULL || out_restored_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored_count = 0U;

    status = make_key(key_prefix, "count", count_key, sizeof(count_key));
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_persistence_get_uint64(
            store, count_key, 0U, &count);
    if (status != UMI_STATUS_OK) return status;

    if (count > UMI_AI_DEVELOPER_PERSISTED_APPROVALS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    umi_ai_developer_approval_queue_clear(queue);

    for (index = 0U; index < (size_t)count; ++index) {
        UmiAiDeveloperApprovalRequest request;
        char prefix[UMI_SESSION_KEY_CAPACITY];
        int written = snprintf(
            prefix, sizeof(prefix), "%s.a%zu", key_prefix, index);

        if (written < 0 || (size_t)written >= sizeof(prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = restore_request(store, prefix, &request);
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_approval_queue_add(queue, &request);
        if (status != UMI_STATUS_OK) return status;

        *out_restored_count += 1U;
    }

    return UMI_STATUS_OK;
}
