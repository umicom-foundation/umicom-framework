/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/chat_persistence.c
 *
 * PURPOSE:
 *   Implement recent coding-chat persistence without introducing a second
 *   storage engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/chat_persistence.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus make_key(
    const char *prefix,
    const char *suffix,
    char *out_key,
    size_t capacity)
{
    int written = snprintf(
        out_key,
        capacity,
        "%s.%s",
        prefix,
        suffix);

    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

static UmiStatus set_text(
    UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    const char *text)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_set(store, key, text);
}

static UmiStatus get_text(
    const UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    char *out_text,
    size_t capacity)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_get(store, key, out_text, capacity);
}

static UmiStatus save_message(
    UmiSessionStore *store,
    const char *prefix,
    size_t persisted_index,
    const UmiAiMessage *message)
{
    char message_prefix[UMI_SESSION_KEY_CAPACITY];
    char key[UMI_SESSION_KEY_CAPACITY];
    char role[32];
    size_t chunk_count = 0U;
    int written;
    UmiStatus status;

    written = snprintf(
        message_prefix,
        sizeof(message_prefix),
        "%s.m%zu",
        prefix,
        persisted_index);
    if (written < 0 || (size_t)written >= sizeof(message_prefix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    written = snprintf(role, sizeof(role), "%u", (unsigned)message->role);
    if (written < 0 || (size_t)written >= sizeof(role)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = set_text(store, message_prefix, "role", role);
    if (status == UMI_STATUS_OK) {
        status = set_text(store, message_prefix, "name", message->name);
    }
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_persistence_save_text(
        store,
        message_prefix,
        message->text,
        strlen(message->text),
        &chunk_count);
    if (status != UMI_STATUS_OK) return status;

    status = make_key(
        message_prefix,
        "chunks",
        key,
        sizeof(key));
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_persistence_set_uint64(
        store,
        key,
        chunk_count);
}

static UmiStatus restore_message(
    const UmiSessionStore *store,
    const char *prefix,
    size_t persisted_index,
    UmiAiMessage *out_message)
{
    char message_prefix[UMI_SESSION_KEY_CAPACITY];
    char role_text[32];
    char key[UMI_SESSION_KEY_CAPACITY];
    char name[UMI_AI_ID_CAPACITY];
    char text[UMI_AI_TEXT_CAPACITY];
    uint64_t role = 0U;
    uint64_t chunks = 0U;
    size_t length = 0U;
    char *end = NULL;
    int written;
    UmiStatus status;

    written = snprintf(
        message_prefix,
        sizeof(message_prefix),
        "%s.m%zu",
        prefix,
        persisted_index);
    if (written < 0 || (size_t)written >= sizeof(message_prefix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = get_text(
        store,
        message_prefix,
        "role",
        role_text,
        sizeof(role_text));
    if (status != UMI_STATUS_OK) return status;

    role = strtoull(role_text, &end, 10);
    if (end == role_text || *end != '\0') return UMI_STATUS_PARSE_ERROR;

    status = get_text(
        store,
        message_prefix,
        "name",
        name,
        sizeof(name));
    if (status != UMI_STATUS_OK) return status;

    status = make_key(message_prefix, "chunks", key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_persistence_get_uint64(
        store,
        key,
        0U,
        &chunks);
    if (status != UMI_STATUS_OK || chunks == 0U) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_PARSE_ERROR;
    }

    status = umi_ai_developer_persistence_load_text(
        store,
        message_prefix,
        (size_t)chunks,
        text,
        sizeof(text),
        &length);
    if (status != UMI_STATUS_OK) return status;

    (void)length;
    return umi_ai_message_set(
        out_message,
        (UmiAiRole)role,
        name,
        text);
}

UmiStatus umi_ai_developer_chat_session_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiCodingToolChatSession *session)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    size_t first;
    size_t index;
    size_t persisted_count;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL ||
        session == NULL || session->session_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = set_text(store, key_prefix, "id", session->session_id);
    if (status == UMI_STATUS_OK) {
        status = set_text(
            store, key_prefix, "provider", session->provider_id);
    }
    if (status == UMI_STATUS_OK) {
        status = set_text(
            store, key_prefix, "model", session->model_id);
    }
    if (status != UMI_STATUS_OK) return status;

    persisted_count =
        session->message_count > UMI_AI_DEVELOPER_PERSISTED_CHAT_MESSAGES
            ? UMI_AI_DEVELOPER_PERSISTED_CHAT_MESSAGES
            : session->message_count;

    first = session->message_count - persisted_count;

    status = make_key(key_prefix, "messageCount", key, sizeof(key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_set_uint64(
            store, key, persisted_count);
    }
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < persisted_count; ++index) {
        status = save_message(
            store,
            key_prefix,
            index,
            &session->messages[first + index]);
        if (status != UMI_STATUS_OK) return status;
    }

    status = make_key(key_prefix, "turnCount", key, sizeof(key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_set_uint64(
            store, key, session->turn_count);
    }
    if (status != UMI_STATUS_OK) return status;

    status = make_key(key_prefix, "toolResultCount", key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_persistence_set_uint64(
        store, key, session->tool_result_count);
}

UmiStatus umi_ai_developer_chat_session_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiCodingToolChatSession *out_session,
    int *out_restored)
{
    char id[UMI_AI_ID_CAPACITY];
    char provider[UMI_AI_ID_CAPACITY];
    char model[UMI_AI_ID_CAPACITY];
    char key[UMI_SESSION_KEY_CAPACITY];
    uint64_t message_count = 0U;
    uint64_t turn_count = 0U;
    uint64_t tool_result_count = 0U;
    size_t index;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL ||
        out_session == NULL || out_restored == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored = 0;

    status = get_text(store, key_prefix, "id", id, sizeof(id));
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;

    status = get_text(
        store, key_prefix, "provider", provider, sizeof(provider));
    if (status == UMI_STATUS_OK) {
        status = get_text(
            store, key_prefix, "model", model, sizeof(model));
    }
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tool_chat_session_init(
        out_session, id, provider, model);
    if (status != UMI_STATUS_OK) return status;

    status = make_key(key_prefix, "messageCount", key, sizeof(key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_get_uint64(
            store, key, 0U, &message_count);
    }
    if (status != UMI_STATUS_OK ||
        message_count > UMI_AI_DEVELOPER_PERSISTED_CHAT_MESSAGES) {
        return status != UMI_STATUS_OK
            ? status
            : UMI_STATUS_PARSE_ERROR;
    }

    for (index = 0U; index < (size_t)message_count; ++index) {
        UmiAiMessage message;

        status = restore_message(
            store, key_prefix, index, &message);
        if (status != UMI_STATUS_OK) return status;

        if (out_session->message_count >=
            UMI_AI_CODING_TOOL_CHAT_MESSAGE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        out_session->messages[out_session->message_count++] = message;
    }

    status = make_key(key_prefix, "turnCount", key, sizeof(key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_get_uint64(
            store, key, 0U, &turn_count);
    }
    if (status != UMI_STATUS_OK) return status;

    status = make_key(key_prefix, "toolResultCount", key, sizeof(key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_get_uint64(
            store, key, 0U, &tool_result_count);
    }
    if (status != UMI_STATUS_OK) return status;

    out_session->turn_count = turn_count;
    out_session->tool_result_count = tool_result_count;
    out_session->revision += 1U;
    *out_restored = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_chat_registry_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiCodingToolChatRegistry *registry)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    size_t count;
    size_t first;
    size_t index;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL || registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    count = umi_ai_coding_tool_chat_registry_count(registry);
    if (count > UMI_AI_DEVELOPER_PERSISTED_CHAT_SESSIONS) {
        count = UMI_AI_DEVELOPER_PERSISTED_CHAT_SESSIONS;
    }

    first =
        umi_ai_coding_tool_chat_registry_count(registry) - count;

    status = make_key(key_prefix, "count", key, sizeof(key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_set_uint64(
            store, key, count);
    }
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < count; ++index) {
        UmiAiCodingToolChatSession session;
        char session_prefix[UMI_SESSION_KEY_CAPACITY];
        int written;

        status = umi_ai_coding_tool_chat_registry_at(
            registry,
            first + index,
            &session);
        if (status != UMI_STATUS_OK) return status;

        written = snprintf(
            session_prefix,
            sizeof(session_prefix),
            "%s.s%zu",
            key_prefix,
            index);
        if (written < 0 ||
            (size_t)written >= sizeof(session_prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = umi_ai_developer_chat_session_save(
            store, session_prefix, &session);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_chat_registry_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiCodingToolChatRegistry *registry,
    size_t *out_restored_count)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    uint64_t count = 0U;
    size_t index;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL ||
        registry == NULL || out_restored_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored_count = 0U;

    status = make_key(key_prefix, "count", key, sizeof(key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_get_uint64(
            store, key, 0U, &count);
    }
    if (status != UMI_STATUS_OK) return status;

    if (count > UMI_AI_DEVELOPER_PERSISTED_CHAT_SESSIONS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    umi_ai_coding_tool_chat_registry_init(registry);

    for (index = 0U; index < (size_t)count; ++index) {
        UmiAiCodingToolChatSession session;
        UmiAiCodingToolChatSession *target = NULL;
        char session_prefix[UMI_SESSION_KEY_CAPACITY];
        int restored = 0;
        int written = snprintf(
            session_prefix,
            sizeof(session_prefix),
            "%s.s%zu",
            key_prefix,
            index);

        if (written < 0 ||
            (size_t)written >= sizeof(session_prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = umi_ai_developer_chat_session_restore(
            store,
            session_prefix,
            &session,
            &restored);
        if (status != UMI_STATUS_OK) return status;
        if (!restored) continue;

        status = umi_ai_coding_tool_chat_registry_open(
            registry,
            session.session_id,
            session.provider_id,
            session.model_id,
            &target);
        if (status != UMI_STATUS_OK || target == NULL) {
            return status != UMI_STATUS_OK
                ? status
                : UMI_STATUS_INTERNAL_ERROR;
        }

        *target = session;
        *out_restored_count += 1U;
    }

    return UMI_STATUS_OK;
}
