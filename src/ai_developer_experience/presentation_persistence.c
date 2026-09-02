/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/presentation_persistence.c
 *
 * PURPOSE:
 *   Implement compact presentation-state persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/presentation_persistence.h"

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
    uint64_t default_value,
    uint64_t *out_value)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_developer_persistence_get_uint64(
        store, key, default_value, out_value);
}

/*
 * Write ai developer presentation in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_ai_developer_presentation_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiDeveloperPresentationState *state)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key_prefix == NULL || state == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = set_number(store, key_prefix, "pane", state->active_pane);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, key_prefix, "chat", state->active_chat_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, key_prefix, "task", state->active_task_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, key_prefix, "approval", state->active_approval_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, key_prefix, "patch", state->active_patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, key_prefix, "diffPath", state->active_diff_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "diffLine", state->active_diff_line);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "contextRow", state->selected_context_row);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "toolDetails",
            state->show_tool_details ? 1U : 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "validationOutput",
            state->show_validation_output ? 1U : 0U);

    return status;
}

/*
 * Provide the ai developer presentation restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_presentation_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiDeveloperPresentationState *state,
    int *out_restored)
{
    uint64_t value = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key_prefix == NULL ||
        state == NULL || out_restored == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored = 0;
    umi_ai_developer_presentation_state_init(state);

    status = get_number(store, key_prefix, "pane", 0U, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value == 0U) return UMI_STATUS_OK;

    /* Apply this branch only when its contract condition is satisfied. */
    if (value < UMI_AI_DEVELOPER_PANE_OVERVIEW ||
        value > UMI_AI_DEVELOPER_PANE_CHECKPOINTS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    state->active_pane = (UmiAiDeveloperPaneKind)value;

    status = get_string(
        store, key_prefix, "chat",
        state->active_chat_id, sizeof(state->active_chat_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) state->active_chat_id[0] = '\0';
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;

    status = get_string(
        store, key_prefix, "task",
        state->active_task_id, sizeof(state->active_task_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) state->active_task_id[0] = '\0';
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;

    status = get_string(
        store, key_prefix, "approval",
        state->active_approval_id, sizeof(state->active_approval_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) state->active_approval_id[0] = '\0';
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;

    status = get_string(
        store, key_prefix, "patch",
        state->active_patch_id, sizeof(state->active_patch_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) state->active_patch_id[0] = '\0';
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;

    status = get_string(
        store, key_prefix, "diffPath",
        state->active_diff_path, sizeof(state->active_diff_path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) state->active_diff_path[0] = '\0';
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;

    status = get_number(store, key_prefix, "diffLine", 0U, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    state->active_diff_line = (size_t)value;

    status = get_number(store, key_prefix, "contextRow", 0U, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    state->selected_context_row = (size_t)value;

    status = get_number(store, key_prefix, "toolDetails", 1U, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    state->show_tool_details = value != 0U;

    status = get_number(store, key_prefix, "validationOutput", 1U, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    state->show_validation_output = value != 0U;

    state->revision += 1U;
    *out_restored = 1;
    return UMI_STATUS_OK;
}
