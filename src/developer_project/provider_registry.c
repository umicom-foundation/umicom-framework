/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/provider_registry.c
 *
 * PURPOSE:
 *   Implement provider registration and deterministic priority selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/provider_registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperProjectProviderRegistry {
    const UmiDeveloperProjectLanguageProvider *
        providers[UMI_DEVELOPER_PROJECT_PROVIDER_CAPACITY];
    size_t count;
};

UmiStatus umi_developer_project_provider_registry_create(
    UmiDeveloperProjectProviderRegistry **out_registry)
{
    UmiDeveloperProjectProviderRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiDeveloperProjectProviderRegistry *)calloc(
        1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_developer_project_provider_registry_destroy(
    UmiDeveloperProjectProviderRegistry *registry)
{
    free(registry);
}

UmiStatus umi_developer_project_provider_registry_register(
    UmiDeveloperProjectProviderRegistry *registry,
    const UmiDeveloperProjectLanguageProvider *provider)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_language_provider_validate(provider);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->providers[index]->provider_id,
                   provider->provider_id) == 0) {
            registry->providers[index] = provider;
            return UMI_STATUS_OK;
        }
    }

    if (registry->count >= UMI_DEVELOPER_PROJECT_PROVIDER_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->providers[registry->count++] = provider;
    return UMI_STATUS_OK;
}

const UmiDeveloperProjectLanguageProvider *
umi_developer_project_provider_registry_find(
    const UmiDeveloperProjectProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;

    if (registry == NULL || provider_id == NULL) return NULL;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->providers[index]->provider_id,
                   provider_id) == 0) {
            return registry->providers[index];
        }
    }

    return NULL;
}

const UmiDeveloperProjectLanguageProvider *
umi_developer_project_provider_registry_select(
    const UmiDeveloperProjectProviderRegistry *registry,
    const UmiDeveloperProjectModel *model)
{
    const UmiDeveloperProjectLanguageProvider *selected = NULL;
    size_t index;

    if (registry == NULL || model == NULL) return NULL;

    for (index = 0U; index < registry->count; ++index) {
        const UmiDeveloperProjectLanguageProvider *candidate =
            registry->providers[index];

        if (candidate->build_system != model->build_system) continue;

        if (strcmp(candidate->primary_language_id, "*") != 0 &&
            strcmp(candidate->primary_language_id,
                   model->primary_language_id) != 0) {
            continue;
        }

        if (selected == NULL || candidate->priority > selected->priority) {
            selected = candidate;
        }
    }

    return selected;
}

size_t umi_developer_project_provider_registry_count(
    const UmiDeveloperProjectProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
