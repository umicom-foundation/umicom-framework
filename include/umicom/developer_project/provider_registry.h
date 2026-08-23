/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/provider_registry.h
 *
 * PURPOSE:
 *   Register and select language/build providers without hard-wiring providers
 *   into Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_PROVIDER_REGISTRY_H
#define UMICOM_DEVELOPER_PROJECT_PROVIDER_REGISTRY_H

#include "umicom/developer_project/language_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROJECT_PROVIDER_CAPACITY 64U

typedef struct UmiDeveloperProjectProviderRegistry
    UmiDeveloperProjectProviderRegistry;

UmiStatus umi_developer_project_provider_registry_create(
    UmiDeveloperProjectProviderRegistry **out_registry);

void umi_developer_project_provider_registry_destroy(
    UmiDeveloperProjectProviderRegistry *registry);

UmiStatus umi_developer_project_provider_registry_register(
    UmiDeveloperProjectProviderRegistry *registry,
    const UmiDeveloperProjectLanguageProvider *provider);

const UmiDeveloperProjectLanguageProvider *
umi_developer_project_provider_registry_find(
    const UmiDeveloperProjectProviderRegistry *registry,
    const char *provider_id);

const UmiDeveloperProjectLanguageProvider *
umi_developer_project_provider_registry_select(
    const UmiDeveloperProjectProviderRegistry *registry,
    const UmiDeveloperProjectModel *model);

size_t umi_developer_project_provider_registry_count(
    const UmiDeveloperProjectProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
