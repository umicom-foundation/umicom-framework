/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/environment.c
 *
 * PURPOSE:
 *   Implement deterministic public-variable storage and {{name}} expansion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/environment.h"

#include <string.h>

/* Provide the valid name operation used by this module and its client applications. */
static int valid_name(const char *name)
{
    const unsigned char *cursor = (const unsigned char *)name;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL || *cursor == 0U) return 0;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!((*cursor >= (unsigned char)'a' && *cursor <= (unsigned char)'z') ||
              (*cursor >= (unsigned char)'A' && *cursor <= (unsigned char)'Z') ||
              (*cursor >= (unsigned char)'0' && *cursor <= (unsigned char)'9') ||
              *cursor == (unsigned char)'_' || *cursor == (unsigned char)'-' ||
              *cursor == (unsigned char)'.')) return 0;
        ++cursor;
    }
    return 1;
}

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(
    const UmiWebWorkbenchEnvironment *environment,
    const char *name)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < environment->value_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(environment->values[index].name, name) == 0) return index;
    }
    return environment->value_count;
}

/*
 * Initialise web workbench environment from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_environment_init(
    UmiWebWorkbenchEnvironment *environment,
    const char *environment_id,
    const char *name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL) return;
    memset(environment, 0, sizeof(*environment));
    (void)umi_web_workbench_copy_text(environment->environment_id,
        sizeof(environment->environment_id), environment_id != NULL ? environment_id : "default");
    (void)umi_web_workbench_copy_text(environment->name,
        sizeof(environment->name), name != NULL ? name : "Default");
    environment->revision = 1U;
}

/*
 * Copy web workbench environment into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_web_workbench_environment_set(
    UmiWebWorkbenchEnvironment *environment,
    const char *name,
    const char *value,
    bool enabled)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || value == NULL || !valid_name(name)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(environment, name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == environment->value_count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index >= UMI_WEB_WORKBENCH_MAX_ENVIRONMENT_VALUES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        memset(&environment->values[index], 0, sizeof(environment->values[index]));
        status = umi_web_workbench_copy_text(environment->values[index].name,
            sizeof(environment->values[index].name), name);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        environment->value_count++;
    }
    status = umi_web_workbench_copy_text(environment->values[index].value,
        sizeof(environment->values[index].value), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    environment->values[index].enabled = enabled;
    environment->revision++;
    return UMI_STATUS_OK;
}

/*
 * Remove web workbench environment while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_web_workbench_environment_remove(
    UmiWebWorkbenchEnvironment *environment,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(environment, name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == environment->value_count) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < environment->value_count) {
        memmove(&environment->values[index], &environment->values[index + 1U],
            (environment->value_count - index - 1U) * sizeof(environment->values[0]));
    }
    environment->value_count--;
    memset(&environment->values[environment->value_count], 0,
        sizeof(environment->values[0]));
    environment->revision++;
    return UMI_STATUS_OK;
}

/*
 * Find web workbench environment while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWebWorkbenchEnvironmentValue *umi_web_workbench_environment_find(
    const UmiWebWorkbenchEnvironment *environment,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || name == NULL) return NULL;
    index = find_index(environment, name);
    return index < environment->value_count ? &environment->values[index] : NULL;
}

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(
    char *out_text,
    size_t capacity,
    size_t *length,
    const char *text,
    size_t text_length)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*length + text_length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out_text + *length, text, text_length);
    *length += text_length;
    out_text[*length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the web workbench environment resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_environment_resolve(
    const UmiWebWorkbenchEnvironment *environment,
    const char *source,
    char *out_text,
    size_t capacity,
    size_t *out_unresolved_count)
{
    size_t cursor = 0U;
    size_t length = 0U;
    size_t unresolved = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_text[0] = '\0';
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (source[cursor] != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (source[cursor] == '{' && source[cursor + 1U] == '{') {
            const char *end = strstr(source + cursor + 2U, "}}");
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (end != NULL) {
                size_t name_length = (size_t)(end - (source + cursor + 2U));
                char name[UMI_WEB_WORKBENCH_ID_CAPACITY];
                const UmiWebWorkbenchEnvironmentValue *value = NULL;
                /* Apply this branch only when its contract condition is satisfied. */
                if (name_length > 0U && name_length < sizeof(name)) {
                    memcpy(name, source + cursor + 2U, name_length);
                    name[name_length] = '\0';
                    value = environment != NULL
                        ? umi_web_workbench_environment_find(environment, name) : NULL;
                }
                /*
                 * Protect caller-owned memory by checking that required state is available before it is
                 * used.
                 */
                if (value != NULL && value->enabled) {
                    UmiStatus status = append_text(out_text, capacity, &length,
                        value->value, strlen(value->value));
                    /* Preserve the original failure result so the caller can respond to the correct cause. */
                    if (status != UMI_STATUS_OK) return status;
                } /* Use this fallback path when the earlier condition does not apply. */ else {
                    size_t placeholder_length = (size_t)(end - (source + cursor)) + 2U;
                    UmiStatus status = append_text(out_text, capacity, &length,
                        source + cursor, placeholder_length);
                    /* Preserve the original failure result so the caller can respond to the correct cause. */
                    if (status != UMI_STATUS_OK) return status;
                    unresolved++;
                }
                cursor = (size_t)(end - source) + 2U;
                continue;
            }
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (append_text(out_text, capacity, &length, source + cursor, 1U)
                != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        cursor++;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_unresolved_count != NULL) *out_unresolved_count = unresolved;
    return UMI_STATUS_OK;
}
