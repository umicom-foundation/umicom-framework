/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/session_bundle.c
 *
 * PURPOSE:
 *   Coordinate durable AI chat/task/approval/presentation/preferences state
 *   while delegating actual storage to the existing Session Store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/session_bundle.h"

#include <stdio.h>

/* Provide the make prefix operation used by this module and its client applications. */
static UmiStatus make_prefix(
    const char *root,
    const char *suffix,
    char *out_prefix,
    size_t capacity)
{
    const int written = snprintf(
        out_prefix,
        capacity,
        "%s.%s",
        root,
        suffix);

    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Provide the schema key operation used by this module and its client applications. */
static UmiStatus schema_key(
    const char *root,
    char *out_key,
    size_t capacity)
{
    return make_prefix(root, "schema", out_key, capacity);
}

/*
 * Write ai developer session bundle in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_ai_developer_session_bundle_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiCodingToolChatRegistry *chats,
    const UmiAiDeveloperTaskRegistry *tasks,
    const UmiAiDeveloperApprovalQueue *approvals,
    const UmiAiDeveloperPresentationState *presentation,
    const UmiAiDeveloperPreferences *preferences)
{
    char prefix[UMI_SESSION_KEY_CAPACITY];
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key_prefix == NULL ||
        chats == NULL || tasks == NULL || approvals == NULL ||
        presentation == NULL || preferences == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = schema_key(key_prefix, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_set_uint64(
            store,
            key,
            UMI_AI_DEVELOPER_STATE_SCHEMA_VERSION);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = make_prefix(key_prefix, "chat", prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_chat_registry_save(
            store, prefix, chats);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = make_prefix(key_prefix, "task", prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_tasks_save(
            store, prefix, tasks);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = make_prefix(key_prefix, "approval", prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_approvals_save(
            store, prefix, approvals);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = make_prefix(key_prefix, "presentation", prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_presentation_save(
            store, prefix, presentation);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = make_prefix(key_prefix, "preferences", prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_preferences_save(
        store, prefix, preferences);
}

/*
 * Provide the ai developer session bundle restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_session_bundle_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiCodingToolChatRegistry *chats,
    UmiAiDeveloperTaskRegistry *tasks,
    UmiAiDeveloperApprovalQueue *approvals,
    UmiAiDeveloperPresentationState *presentation,
    UmiAiDeveloperPreferences *preferences,
    UmiAiDeveloperRestoreReport *out_report)
{
    char prefix[UMI_SESSION_KEY_CAPACITY];
    char key[UMI_SESSION_KEY_CAPACITY];
    uint64_t schema = 0U;
    int restored = 0;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key_prefix == NULL ||
        chats == NULL || tasks == NULL || approvals == NULL ||
        presentation == NULL || preferences == NULL ||
        out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_ai_developer_restore_report_init(out_report);

    status = schema_key(key_prefix, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_get_uint64(
            store, key, 0U, &schema);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto done;

    /* Apply this branch only when its contract condition is satisfied. */
    if (schema == 0U) {
        status = UMI_STATUS_OK;
        goto done;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (schema > UMI_AI_DEVELOPER_STATE_SCHEMA_VERSION) {
        status = UMI_STATUS_NOT_IMPLEMENTED;
        goto done;
    }

    out_report->schema_version = (uint32_t)schema;

    status = make_prefix(key_prefix, "chat", prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_chat_registry_restore(
            store, prefix, chats, &out_report->chat_sessions);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto done;

    status = make_prefix(key_prefix, "task", prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_tasks_restore(
            store, prefix, tasks, &out_report->tasks);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto done;

    status = make_prefix(key_prefix, "approval", prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_approvals_restore(
            store, prefix, approvals, &out_report->approvals);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto done;

    status = make_prefix(
        key_prefix, "presentation", prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_presentation_restore(
            store, prefix, presentation, &restored);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto done;
    out_report->presentation_restored = restored;

    restored = 0;
    status = make_prefix(
        key_prefix, "preferences", prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_preferences_restore(
            store, prefix, preferences, &restored);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto done;
    out_report->preferences_restored = restored;

done:
    out_report->status = status;
    return status;
}
