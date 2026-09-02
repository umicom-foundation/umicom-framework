/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/variable_set.h
 *
 * PURPOSE:
 *   Store bounded project-template variables by value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_VARIABLE_SET_H
#define UMICOM_DEVELOPER_PROJECT_VARIABLE_SET_H

#include "umicom/developer_project/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer project variable set data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProjectVariableSet {
    UmiDeveloperProjectVariable
        variables[UMI_DEVELOPER_PROJECT_VARIABLE_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiDeveloperProjectVariableSet;

/**
 * Initialise developer project variable set from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_project_variable_set_init(
    UmiDeveloperProjectVariableSet *variables);

/**
 * Copy developer project variable into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_developer_project_variable_set(
    UmiDeveloperProjectVariableSet *variables,
    const char *name,
    const char *value);

/**
 * Provide the developer project variable get operation used by this module and its client
 * applications.
 */
const char *umi_developer_project_variable_get(
    const UmiDeveloperProjectVariableSet *variables,
    const char *name);

#ifdef __cplusplus
}
#endif

#endif
