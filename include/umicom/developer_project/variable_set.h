/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/variable_set.h
 *
 * PURPOSE:
 *   Store bounded project-template variables by value.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_VARIABLE_SET_H
#define UMICOM_DEVELOPER_PROJECT_VARIABLE_SET_H

#include "umicom/developer_project/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProjectVariableSet {
    UmiDeveloperProjectVariable
        variables[UMI_DEVELOPER_PROJECT_VARIABLE_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiDeveloperProjectVariableSet;

void umi_developer_project_variable_set_init(
    UmiDeveloperProjectVariableSet *variables);

UmiStatus umi_developer_project_variable_set(
    UmiDeveloperProjectVariableSet *variables,
    const char *name,
    const char *value);

const char *umi_developer_project_variable_get(
    const UmiDeveloperProjectVariableSet *variables,
    const char *name);

#ifdef __cplusplus
}
#endif

#endif
