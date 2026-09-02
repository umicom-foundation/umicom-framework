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

/* Provide the find entry operation used by this module and its client applications. */
static size_t find_entry(const UmiTerminalEnvironment *environment,
                         const char *name)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < environment->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(environment->entries[index].name, name) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Initialise terminal environment from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_terminal_environment_create(
    UmiTerminalEnvironment **out_environment)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Release or reset state held by terminal environment so the same storage can be reused
 * safely.
 */
void umi_terminal_environment_destroy(UmiTerminalEnvironment *environment)
{
    free(environment);
}

/*
 * Copy terminal environment into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_terminal_environment_set(UmiTerminalEnvironment *environment,
                                       const char *name,
                                       const char *value)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || name == NULL || value == NULL ||
        name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_entry(environment, name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (environment->count >= UMI_TERMINAL_MAX_ENVIRONMENT) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = environment->count++;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Remove terminal environment while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_terminal_environment_remove(
    UmiTerminalEnvironment *environment,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_entry(environment, name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < environment->count) {
        (void)memmove(&environment->entries[index],
                      &environment->entries[index + 1U],
                      (environment->count - index - 1U) *
                          sizeof(environment->entries[0]));
    }
    environment->count -= 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the terminal environment get operation used by this module and its client
 * applications.
 */
const char *umi_terminal_environment_get(
    const UmiTerminalEnvironment *environment,
    const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || name == NULL) {
        return NULL;
    }
    index = find_entry(environment, name);
    return index != SIZE_MAX ? environment->entries[index].value : NULL;
}

/*
 * Return the number of records represented by terminal environment without changing their
 * state.
 */
size_t umi_terminal_environment_count(
    const UmiTerminalEnvironment *environment)
{
    return environment != NULL ? environment->count : 0U;
}

/*
 * Provide the terminal environment export operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_environment_export(
    const UmiTerminalEnvironment *environment,
    UmiEnvironmentVariable *out_variables,
    size_t capacity,
    size_t *out_count)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || out_variables == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity < environment->count) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < environment->count; ++index) {
        out_variables[index].name = environment->entries[index].name;
        out_variables[index].value = environment->entries[index].value;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_count != NULL) {
        *out_count = environment->count;
    }
    return UMI_STATUS_OK;
}
