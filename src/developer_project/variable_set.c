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

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

void umi_developer_project_variable_set_init(
    UmiDeveloperProjectVariableSet *variables)
{
    if (variables == NULL) return;
    (void)memset(variables, 0, sizeof(*variables));
    variables->revision = 1U;
}

UmiStatus umi_developer_project_variable_set(
    UmiDeveloperProjectVariableSet *variables,
    const char *name,
    const char *value)
{
    size_t index;
    UmiStatus status;

    if (variables == NULL || name == NULL || name[0] == '\0' ||
        value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < variables->count; ++index) {
        if (strcmp(variables->variables[index].name, name) == 0) {
            status = copy_text(
                variables->variables[index].value,
                sizeof(variables->variables[index].value),
                value);
            if (status == UMI_STATUS_OK) variables->revision += 1U;
            return status;
        }
    }

    if (variables->count >= UMI_DEVELOPER_PROJECT_VARIABLE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = copy_text(
        variables->variables[variables->count].name,
        sizeof(variables->variables[variables->count].name),
        name);
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        variables->variables[variables->count].value,
        sizeof(variables->variables[variables->count].value),
        value);
    if (status != UMI_STATUS_OK) return status;

    variables->count += 1U;
    variables->revision += 1U;
    return UMI_STATUS_OK;
}

const char *umi_developer_project_variable_get(
    const UmiDeveloperProjectVariableSet *variables,
    const char *name)
{
    size_t index;

    if (variables == NULL || name == NULL) return NULL;

    for (index = 0U; index < variables->count; ++index) {
        if (strcmp(variables->variables[index].name, name) == 0) {
            return variables->variables[index].value;
        }
    }

    return NULL;
}
