/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/builtin_providers.h
 *
 * PURPOSE:
 *   Enumerate and register built-in project build/run providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_BUILTIN_PROVIDERS_H
#define UMICOM_DEVELOPER_PROJECT_BUILTIN_PROVIDERS_H

#include "umicom/developer_project/provider_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by developer project builtin provider without
 * changing their state.
 */
size_t umi_developer_project_builtin_provider_count(void);

/**
 * Find developer project builtin provider while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectLanguageProvider *
umi_developer_project_builtin_provider_at(size_t index);

/**
 * Add developer project builtin providers only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_project_builtin_providers_register(
    UmiDeveloperProjectProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
