/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/presentation_persistence.c
 *
 * PURPOSE:
 *   Implement compact presentation-state persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/presentation_persistence.h"

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
    uint64_t default_value,
    uint64_t *out_value)
{
    char key[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = make_key(prefix, suffix, key, sizeof(key));
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_developer_persistence_get_uint64(
        store, key, default_value, out_value);
}

UmiStatus umi_ai_developer_presentation_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiDeveloperPresentationState *state)
{
    UmiStatus status;

    if (store == NULL || key_prefix == NULL || state == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = set_number(store, key_prefix, "pane", state->active_pane);
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, key_prefix, "chat", state->active_chat_id);
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, key_prefix, "task", state->active_task_id);
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, key_prefix, "approval", state->active_approval_id);
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, key_prefix, "patch", state->active_patch_id);
    if (status == UMI_STATUS_OK)
        status = set_string(
            store, key_prefix, "diffPath", state->active_diff_path);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "diffLine", state->active_diff_line);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "contextRow", state->selected_context_row);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "toolDetails",
            state->show_tool_details ? 1U : 0U);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "validationOutput",
            state->show_validation_output ? 1U : 0U);

    return status;
}

UmiStatus umi_ai_developer_presentation_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiDeveloperPresentationState *state,
    int *out_restored)
{
    uint64_t value = 0U;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL ||
        state == NULL || out_restored == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored = 0;
    umi_ai_developer_presentation_state_init(state);

    status = get_number(store, key_prefix, "pane", 0U, &value);
    if (status != UMI_STATUS_OK) return status;

    if (value == 0U) return UMI_STATUS_OK;

    if (value < UMI_AI_DEVELOPER_PANE_OVERVIEW ||
        value > UMI_AI_DEVELOPER_PANE_CHECKPOINTS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    state->active_pane = (UmiAiDeveloperPaneKind)value;

    status = get_string(
        store, key_prefix, "chat",
        state->active_chat_id, sizeof(state->active_chat_id));
    if (status == UMI_STATUS_NOT_FOUND) state->active_chat_id[0] = '\0';
    else if (status != UMI_STATUS_OK) return status;

    status = get_string(
        store, key_prefix, "task",
        state->active_task_id, sizeof(state->active_task_id));
    if (status == UMI_STATUS_NOT_FOUND) state->active_task_id[0] = '\0';
    else if (status != UMI_STATUS_OK) return status;

    status = get_string(
        store, key_prefix, "approval",
        state->active_approval_id, sizeof(state->active_approval_id));
    if (status == UMI_STATUS_NOT_FOUND) state->active_approval_id[0] = '\0';
    else if (status != UMI_STATUS_OK) return status;

    status = get_string(
        store, key_prefix, "patch",
        state->active_patch_id, sizeof(state->active_patch_id));
    if (status == UMI_STATUS_NOT_FOUND) state->active_patch_id[0] = '\0';
    else if (status != UMI_STATUS_OK) return status;

    status = get_string(
        store, key_prefix, "diffPath",
        state->active_diff_path, sizeof(state->active_diff_path));
    if (status == UMI_STATUS_NOT_FOUND) state->active_diff_path[0] = '\0';
    else if (status != UMI_STATUS_OK) return status;

    status = get_number(store, key_prefix, "diffLine", 0U, &value);
    if (status != UMI_STATUS_OK) return status;
    state->active_diff_line = (size_t)value;

    status = get_number(store, key_prefix, "contextRow", 0U, &value);
    if (status != UMI_STATUS_OK) return status;
    state->selected_context_row = (size_t)value;

    status = get_number(store, key_prefix, "toolDetails", 1U, &value);
    if (status != UMI_STATUS_OK) return status;
    state->show_tool_details = value != 0U;

    status = get_number(store, key_prefix, "validationOutput", 1U, &value);
    if (status != UMI_STATUS_OK) return status;
    state->show_validation_output = value != 0U;

    state->revision += 1U;
    *out_restored = 1;
    return UMI_STATUS_OK;
}
