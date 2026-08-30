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

size_t umi_developer_project_builtin_provider_count(void);

const UmiDeveloperProjectLanguageProvider *
umi_developer_project_builtin_provider_at(size_t index);

UmiStatus umi_developer_project_builtin_providers_register(
    UmiDeveloperProjectProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
