/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/environment.c
 *
 * PURPOSE:
 *   Implement a bounded terminal environment override map.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/terminal/environment.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct UmiTerminalEnvironmentEntry {
    char name[256];
    char value[UMI_TERMINAL_PATH_CAPACITY];
} UmiTerminalEnvironmentEntry;

struct UmiTerminalEnvironment {
    UmiTerminalEnvironmentEntry entries[UMI_TERMINAL_MAX_ENVIRONMENT];
    size_t count;
};

static size_t find_entry(const UmiTerminalEnvironment *environment,
                         const char *name)
{
    size_t index;
    for (index = 0U; index < environment->count; ++index) {
        if (strcmp(environment->entries[index].name, name) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_terminal_environment_create(
    UmiTerminalEnvironment **out_environment)
{
    if (out_environment == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_environment = (UmiTerminalEnvironment *)calloc(
        1U,
        sizeof(UmiTerminalEnvironment)
    );
    return *out_environment != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_terminal_environment_destroy(UmiTerminalEnvironment *environment)
{
    free(environment);
}

UmiStatus umi_terminal_environment_set(UmiTerminalEnvironment *environment,
                                       const char *name,
                                       const char *value)
{
    size_t index;
    if (environment == NULL || name == NULL || value == NULL ||
        name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_entry(environment, name);
    if (index == SIZE_MAX) {
        if (environment->count >= UMI_TERMINAL_MAX_ENVIRONMENT) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = environment->count++;
    }
    if (strlen(name) + 1U > sizeof(environment->entries[index].name) ||
        strlen(value) + 1U > sizeof(environment->entries[index].value)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(environment->entries[index].name,
                   sizeof(environment->entries[index].name),
                   "%s",
                   name);
    (void)snprintf(environment->entries[index].value,
                   sizeof(environment->entries[index].value),
                   "%s",
                   value);
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_environment_remove(
    UmiTerminalEnvironment *environment,
    const char *name)
{
    size_t index;
    if (environment == NULL || name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_entry(environment, name);
    if (index == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (index + 1U < environment->count) {
        (void)memmove(&environment->entries[index],
                      &environment->entries[index + 1U],
                      (environment->count - index - 1U) *
                          sizeof(environment->entries[0]));
    }
    environment->count -= 1U;
    return UMI_STATUS_OK;
}

const char *umi_terminal_environment_get(
    const UmiTerminalEnvironment *environment,
    const char *name)
{
    size_t index;
    if (environment == NULL || name == NULL) {
        return NULL;
    }
    index = find_entry(environment, name);
    return index != SIZE_MAX ? environment->entries[index].value : NULL;
}

size_t umi_terminal_environment_count(
    const UmiTerminalEnvironment *environment)
{
    return environment != NULL ? environment->count : 0U;
}

UmiStatus umi_terminal_environment_export(
    const UmiTerminalEnvironment *environment,
    UmiEnvironmentVariable *out_variables,
    size_t capacity,
    size_t *out_count)
{
    size_t index;
    if (environment == NULL || out_variables == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (capacity < environment->count) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < environment->count; ++index) {
        out_variables[index].name = environment->entries[index].name;
        out_variables[index].value = environment->entries[index].value;
    }
    if (out_count != NULL) {
        *out_count = environment->count;
    }
    return UMI_STATUS_OK;
}
