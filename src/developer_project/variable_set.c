/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/variable_set.c
 *
 * PURPOSE:
 *   Implement bounded template-variable storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/variable_set.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise developer project variable set from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_project_variable_set_init(
    UmiDeveloperProjectVariableSet *variables)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (variables == NULL) return;
    (void)memset(variables, 0, sizeof(*variables));
    variables->revision = 1U;
}

/*
 * Copy developer project variable into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_developer_project_variable_set(
    UmiDeveloperProjectVariableSet *variables,
    const char *name,
    const char *value)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (variables == NULL || name == NULL || name[0] == '\0' ||
        value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < variables->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(variables->variables[index].name, name) == 0) {
            status = copy_text(
                variables->variables[index].value,
                sizeof(variables->variables[index].value),
                value);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) variables->revision += 1U;
            return status;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (variables->count >= UMI_DEVELOPER_PROJECT_VARIABLE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = copy_text(
        variables->variables[variables->count].name,
        sizeof(variables->variables[variables->count].name),
        name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        variables->variables[variables->count].value,
        sizeof(variables->variables[variables->count].value),
        value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    variables->count += 1U;
    variables->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer project variable get operation used by this module and its client
 * applications.
 */
const char *umi_developer_project_variable_get(
    const UmiDeveloperProjectVariableSet *variables,
    const char *name)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (variables == NULL || name == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < variables->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(variables->variables[index].name, name) == 0) {
            return variables->variables[index].value;
        }
    }

    return NULL;
}
