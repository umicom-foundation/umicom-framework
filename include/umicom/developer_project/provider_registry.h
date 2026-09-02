/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/provider_registry.h
 *
 * PURPOSE:
 *   Register and select language/build providers without hard-wiring providers
 *   into Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_PROVIDER_REGISTRY_H
#define UMICOM_DEVELOPER_PROJECT_PROVIDER_REGISTRY_H

#include "umicom/developer_project/language_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROJECT_PROVIDER_CAPACITY 64U

/**
 * Represent the developer project provider registry data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperProjectProviderRegistry
    UmiDeveloperProjectProviderRegistry;

/**
 * Initialise developer project provider registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_project_provider_registry_create(
    UmiDeveloperProjectProviderRegistry **out_registry);

/**
 * Release or reset state held by developer project provider registry so the same storage
 * can be reused safely.
 */
void umi_developer_project_provider_registry_destroy(
    UmiDeveloperProjectProviderRegistry *registry);

/**
 * Add developer project provider registry only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_project_provider_registry_register(
    UmiDeveloperProjectProviderRegistry *registry,
    const UmiDeveloperProjectLanguageProvider *provider);

/**
 * Find developer project provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectLanguageProvider *
umi_developer_project_provider_registry_find(
    const UmiDeveloperProjectProviderRegistry *registry,
    const char *provider_id);

/**
 * Provide the developer project provider registry select operation used by this module and
 * its client applications.
 */
const UmiDeveloperProjectLanguageProvider *
umi_developer_project_provider_registry_select(
    const UmiDeveloperProjectProviderRegistry *registry,
    const UmiDeveloperProjectModel *model);

/**
 * Return the number of records represented by developer project provider registry without
 * changing their state.
 */
size_t umi_developer_project_provider_registry_count(
    const UmiDeveloperProjectProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
