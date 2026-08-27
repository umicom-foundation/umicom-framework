/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/environment.c
 *
 * PURPOSE:
 *   Implement deterministic public-variable storage and {{name}} expansion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/environment.h"

#include <string.h>

static int valid_name(const char *name)
{
    const unsigned char *cursor = (const unsigned char *)name;
    if (cursor == NULL || *cursor == 0U) return 0;
    while (*cursor != 0U) {
        if (!((*cursor >= (unsigned char)'a' && *cursor <= (unsigned char)'z') ||
              (*cursor >= (unsigned char)'A' && *cursor <= (unsigned char)'Z') ||
              (*cursor >= (unsigned char)'0' && *cursor <= (unsigned char)'9') ||
              *cursor == (unsigned char)'_' || *cursor == (unsigned char)'-' ||
              *cursor == (unsigned char)'.')) return 0;
        ++cursor;
    }
    return 1;
}

static size_t find_index(
    const UmiWebWorkbenchEnvironment *environment,
    const char *name)
{
    size_t index;
    for (index = 0U; index < environment->value_count; ++index) {
        if (strcmp(environment->values[index].name, name) == 0) return index;
    }
    return environment->value_count;
}

void umi_web_workbench_environment_init(
    UmiWebWorkbenchEnvironment *environment,
    const char *environment_id,
    const char *name)
{
    if (environment == NULL) return;
    memset(environment, 0, sizeof(*environment));
    (void)umi_web_workbench_copy_text(environment->environment_id,
        sizeof(environment->environment_id), environment_id != NULL ? environment_id : "default");
    (void)umi_web_workbench_copy_text(environment->name,
        sizeof(environment->name), name != NULL ? name : "Default");
    environment->revision = 1U;
}

UmiStatus umi_web_workbench_environment_set(
    UmiWebWorkbenchEnvironment *environment,
    const char *name,
    const char *value,
    bool enabled)
{
    size_t index;
    UmiStatus status;
    if (environment == NULL || value == NULL || !valid_name(name)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(environment, name);
    if (index == environment->value_count) {
        if (index >= UMI_WEB_WORKBENCH_MAX_ENVIRONMENT_VALUES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        memset(&environment->values[index], 0, sizeof(environment->values[index]));
        status = umi_web_workbench_copy_text(environment->values[index].name,
            sizeof(environment->values[index].name), name);
        if (status != UMI_STATUS_OK) return status;
        environment->value_count++;
    }
    status = umi_web_workbench_copy_text(environment->values[index].value,
        sizeof(environment->values[index].value), value);
    if (status != UMI_STATUS_OK) return status;
    environment->values[index].enabled = enabled;
    environment->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_web_workbench_environment_remove(
    UmiWebWorkbenchEnvironment *environment,
    const char *name)
{
    size_t index;
    if (environment == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(environment, name);
    if (index == environment->value_count) return UMI_STATUS_NOT_FOUND;
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

const UmiWebWorkbenchEnvironmentValue *umi_web_workbench_environment_find(
    const UmiWebWorkbenchEnvironment *environment,
    const char *name)
{
    size_t index;
    if (environment == NULL || name == NULL) return NULL;
    index = find_index(environment, name);
    return index < environment->value_count ? &environment->values[index] : NULL;
}

static UmiStatus append_text(
    char *out_text,
    size_t capacity,
    size_t *length,
    const char *text,
    size_t text_length)
{
    if (*length + text_length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out_text + *length, text, text_length);
    *length += text_length;
    out_text[*length] = '\0';
    return UMI_STATUS_OK;
}

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
    if (source == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_text[0] = '\0';
    while (source[cursor] != '\0') {
        if (source[cursor] == '{' && source[cursor + 1U] == '{') {
            const char *end = strstr(source + cursor + 2U, "}}");
            if (end != NULL) {
                size_t name_length = (size_t)(end - (source + cursor + 2U));
                char name[UMI_WEB_WORKBENCH_ID_CAPACITY];
                const UmiWebWorkbenchEnvironmentValue *value = NULL;
                if (name_length > 0U && name_length < sizeof(name)) {
                    memcpy(name, source + cursor + 2U, name_length);
                    name[name_length] = '\0';
                    value = environment != NULL
                        ? umi_web_workbench_environment_find(environment, name) : NULL;
                }
                if (value != NULL && value->enabled) {
                    UmiStatus status = append_text(out_text, capacity, &length,
                        value->value, strlen(value->value));
                    if (status != UMI_STATUS_OK) return status;
                } else {
                    size_t placeholder_length = (size_t)(end - (source + cursor)) + 2U;
                    UmiStatus status = append_text(out_text, capacity, &length,
                        source + cursor, placeholder_length);
                    if (status != UMI_STATUS_OK) return status;
                    unresolved++;
                }
                cursor = (size_t)(end - source) + 2U;
                continue;
            }
        }
        if (append_text(out_text, capacity, &length, source + cursor, 1U)
                != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        cursor++;
    }
    if (out_unresolved_count != NULL) *out_unresolved_count = unresolved;
    return UMI_STATUS_OK;
}
