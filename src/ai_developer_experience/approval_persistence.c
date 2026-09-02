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

/* Provide the make key operation used by this module and its client applications. */
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

/* Provide the set string operation used by this module and its client applications. */
static UmiStatus set_string(
    UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    const char *value)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_set(store, key, value);
}

/* Provide the get string operation used by this module and its client applications. */
static UmiStatus get_string(
    const UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    char *out_value,
    size_t capacity)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_get(store, key, out_value, capacity);
}

/* Provide the save text field operation used by this module and its client applications. */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(text_prefix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_ai_developer_persistence_save_text(
        store,
        text_prefix,
        text,
        strlen(text),
        &chunks);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = make_key(text_prefix, "count", count_key, sizeof(count_key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_persistence_set_uint64(
        store, count_key, chunks);
}

/* Provide the load text field operation used by this module and its client applications. */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(text_prefix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = make_key(text_prefix, "count", count_key, sizeof(count_key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_get_uint64(
            store, count_key, 0U, &chunks);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/* Provide the set number operation used by this module and its client applications. */
static UmiStatus set_number(
    UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    uint64_t value)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_developer_persistence_set_uint64(store, key, value);
}

/* Provide the get number operation used by this module and its client applications. */
static UmiStatus get_number(
    const UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    uint64_t *out_value)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_developer_persistence_get_uint64(
        store, key, 0U, out_value);
}

/* Provide the save request operation used by this module and its client applications. */
static UmiStatus save_request(
    UmiSessionStore *store,
    const char *prefix,
    const UmiAiDeveloperApprovalRequest *request)
{
    char arguments_prefix[UMI_SESSION_KEY_CAPACITY];
    size_t chunks = 0U;
    UmiStatus status;

    status = set_string(store, prefix, "id", request->approval_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = save_text_field(store, prefix, "title", request->title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = save_text_field(store, prefix, "summary", request->summary);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(store, prefix, "permission", request->permission);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(store, prefix, "subject", request->subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = set_number(store, prefix, "kind", request->kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(store, prefix, "state", request->state);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(store, prefix, "risk", request->risk);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "requested", request->requested_sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "decided", request->decided_sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "executable",
            request->executable ? 1U : 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    {
        int written = snprintf(
            arguments_prefix,
            sizeof(arguments_prefix),
            "%s.arguments",
            prefix);

        /* Apply this branch only when its contract condition is satisfied. */
        if (written < 0 ||
            (size_t)written >= sizeof(arguments_prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = set_number(store, prefix, "argumentChunks", chunks);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "toolCallId", request->tool_call.call_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, prefix, "toolId", request->tool_call.tool_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "toolSequence", request->tool_call.sequence);

    return status;
}

/* Provide the restore request operation used by this module and its client applications. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = load_text_field(
            store, prefix, "title",
            request->title, sizeof(request->title));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = load_text_field(
            store, prefix, "summary",
            request->summary, sizeof(request->summary));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = get_string(
            store, prefix, "permission",
            request->permission, sizeof(request->permission));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = get_string(
            store, prefix, "subject",
            request->subject_id, sizeof(request->subject_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = get_number(store, prefix, "kind", &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    request->kind = (UmiAiDeveloperApprovalKind)value;

    status = get_number(store, prefix, "state", &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    request->state = (UmiAiDeveloperApprovalState)value;

    status = get_number(store, prefix, "risk", &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    request->risk = (UmiAiCodingToolRisk)value;

    status = get_number(
        store, prefix, "requested", &request->requested_sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = get_number(
            store, prefix, "decided", &request->decided_sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = get_number(store, prefix, "executable", &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    request->executable = value != 0U;

    status = get_number(store, prefix, "argumentChunks", &chunks);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (chunks > UMI_AI_DEVELOPER_APPROVAL_ARGUMENT_CHUNKS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    {
        int written = snprintf(
            arguments_prefix,
            sizeof(arguments_prefix),
            "%s.arguments",
            prefix);

        /* Apply this branch only when its contract condition is satisfied. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)length;

    status = get_number(
        store, prefix, "toolCallId", &request->tool_call.call_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = get_string(
            store, prefix, "toolId",
            request->tool_call.tool_id,
            sizeof(request->tool_call.tool_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = get_number(
            store, prefix, "toolSequence",
            &request->tool_call.sequence);

    request->tool_call.approved = 0;
    request->tool_call.state = UMI_AI_CODING_TOOL_CALL_PENDING;
    request->tool_call.status = UMI_STATUS_OK;
    return status;
}

/*
 * Write ai developer approvals in its stable representation and report capacity or input
 * failures to the caller.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_persistence_set_uint64(
            store, count_key, count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiAiDeveloperApprovalRequest request;
        char prefix[UMI_SESSION_KEY_CAPACITY];
        int written;

        status = umi_ai_developer_approval_queue_at(
            queue, first + index, &request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        written = snprintf(
            prefix,
            sizeof(prefix),
            "%s.a%zu",
            key_prefix,
            index);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = save_request(store, prefix, &request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer approvals restore operation used by this module and its client
 * applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key_prefix == NULL ||
        queue == NULL || out_restored_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored_count = 0U;

    status = make_key(key_prefix, "count", count_key, sizeof(count_key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_persistence_get_uint64(
            store, count_key, 0U, &count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_AI_DEVELOPER_PERSISTED_APPROVALS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    umi_ai_developer_approval_queue_clear(queue);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < (size_t)count; ++index) {
        UmiAiDeveloperApprovalRequest request;
        char prefix[UMI_SESSION_KEY_CAPACITY];
        int written = snprintf(
            prefix, sizeof(prefix), "%s.a%zu", key_prefix, index);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = restore_request(store, prefix, &request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_approval_queue_add(queue, &request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        *out_restored_count += 1U;
    }

    return UMI_STATUS_OK;
}
