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

/*
 * Initialise developer project provider registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_project_provider_registry_create(
    UmiDeveloperProjectProviderRegistry **out_registry)
{
    UmiDeveloperProjectProviderRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiDeveloperProjectProviderRegistry *)calloc(
        1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer project provider registry so the same storage
 * can be reused safely.
 */
void umi_developer_project_provider_registry_destroy(
    UmiDeveloperProjectProviderRegistry *registry)
{
    free(registry);
}

/*
 * Add developer project provider registry only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_project_provider_registry_register(
    UmiDeveloperProjectProviderRegistry *registry,
    const UmiDeveloperProjectLanguageProvider *provider)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_language_provider_validate(provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->providers[index]->provider_id,
                   provider->provider_id) == 0) {
            registry->providers[index] = provider;
            return UMI_STATUS_OK;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_DEVELOPER_PROJECT_PROVIDER_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->providers[registry->count++] = provider;
    return UMI_STATUS_OK;
}

/*
 * Find developer project provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperProjectLanguageProvider *
umi_developer_project_provider_registry_find(
    const UmiDeveloperProjectProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->providers[index]->provider_id,
                   provider_id) == 0) {
            return registry->providers[index];
        }
    }

    return NULL;
}

/*
 * Provide the developer project provider registry select operation used by this module and
 * its client applications.
 */
const UmiDeveloperProjectLanguageProvider *
umi_developer_project_provider_registry_select(
    const UmiDeveloperProjectProviderRegistry *registry,
    const UmiDeveloperProjectModel *model)
{
    const UmiDeveloperProjectLanguageProvider *selected = NULL;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || model == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        const UmiDeveloperProjectLanguageProvider *candidate =
            registry->providers[index];

        /* Create this optional product surface only when its build option is enabled. */
        if (candidate->build_system != model->build_system) continue;

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(candidate->primary_language_id, "*") != 0 &&
            strcmp(candidate->primary_language_id,
                   model->primary_language_id) != 0) {
            continue;
        }

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (selected == NULL || candidate->priority > selected->priority) {
            selected = candidate;
        }
    }

    return selected;
}

/*
 * Return the number of records represented by developer project provider registry without
 * changing their state.
 */
size_t umi_developer_project_provider_registry_count(
    const UmiDeveloperProjectProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
