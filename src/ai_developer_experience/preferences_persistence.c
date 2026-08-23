/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/preferences_persistence.c
 *
 * PURPOSE:
 *   Implement compact preference persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/preferences_persistence.h"

#include <stdio.h>

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

UmiStatus umi_ai_developer_preferences_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiDeveloperPreferences *preferences)
{
    UmiStatus status;

    if (store == NULL || key_prefix == NULL || preferences == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_developer_preferences_validate(preferences);
    if (status != UMI_STATUS_OK) return status;

    status = set_number(
        store, key_prefix, "diffLayout", preferences->diff_layout);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "contextLines",
            preferences->diff_context_lines);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "visibleRows",
            preferences->visible_rows);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "autoFollow",
            preferences->auto_follow_active_task ? 1U : 0U);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "autoReview",
            preferences->auto_open_review ? 1U : 0U);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "toolArguments",
            preferences->show_tool_arguments ? 1U : 0U);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "validationOutput",
            preferences->show_validation_output ? 1U : 0U);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, key_prefix, "contextTokens",
            preferences->show_context_token_estimates ? 1U : 0U);

    return status;
}

UmiStatus umi_ai_developer_preferences_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiDeveloperPreferences *preferences,
    int *out_restored)
{
    uint64_t value = 0U;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL ||
        preferences == NULL || out_restored == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored = 0;
    umi_ai_developer_preferences_init(preferences);

    status = get_number(store, key_prefix, "diffLayout", UINT64_MAX, &value);
    if (status != UMI_STATUS_OK) return status;

    if (value == UINT64_MAX) return UMI_STATUS_OK;

    preferences->diff_layout = (UmiAiDeveloperDiffLayout)value;

    status = get_number(
        store, key_prefix, "contextLines", 3U, &value);
    if (status != UMI_STATUS_OK || value > SIZE_MAX) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    preferences->diff_context_lines = (size_t)value;

    status = get_number(
        store, key_prefix, "visibleRows", 24U, &value);
    if (status != UMI_STATUS_OK || value > SIZE_MAX) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    preferences->visible_rows = (size_t)value;

    status = get_number(
        store, key_prefix, "autoFollow", 1U, &value);
    if (status != UMI_STATUS_OK || value > 1U) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_PARSE_ERROR;
    }
    preferences->auto_follow_active_task = value != 0U;

    status = get_number(
        store, key_prefix, "autoReview", 1U, &value);
    if (status != UMI_STATUS_OK || value > 1U) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_PARSE_ERROR;
    }
    preferences->auto_open_review = value != 0U;

    status = get_number(
        store, key_prefix, "toolArguments", 0U, &value);
    if (status != UMI_STATUS_OK || value > 1U) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_PARSE_ERROR;
    }
    preferences->show_tool_arguments = value != 0U;

    status = get_number(
        store, key_prefix, "validationOutput", 1U, &value);
    if (status != UMI_STATUS_OK || value > 1U) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_PARSE_ERROR;
    }
    preferences->show_validation_output = value != 0U;

    status = get_number(
        store, key_prefix, "contextTokens", 1U, &value);
    if (status != UMI_STATUS_OK || value > 1U) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_PARSE_ERROR;
    }
    preferences->show_context_token_estimates = value != 0U;

    preferences->revision += 1U;

    status = umi_ai_developer_preferences_validate(preferences);
    if (status != UMI_STATUS_OK) return status;

    *out_restored = 1;
    return UMI_STATUS_OK;
}
