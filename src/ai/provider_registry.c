/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/provider_registry.c
 *
 * PURPOSE:
 *   Register and resolve AI providers by identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Registration copies the small function table, while each provider keeps its private implementation behind the opaque instance pointer.
 */

#include "umicom/ai/provider_registry.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai provider registry from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_provider_registry_init(UmiAiProviderRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
    }
}

/*
 * Release or reset state held by ai provider registry so the same storage can be reused
 * safely.
 */
void umi_ai_provider_registry_destroy(UmiAiProviderRegistry *registry)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    /* Reverse registration order mirrors dependency-friendly stack cleanup. */
    for (index = registry->count; index > 0U; --index) {
        UmiAiProvider *provider = &registry->providers[index - 1U];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (provider->destroy != NULL && provider->instance != NULL) {
            provider->destroy(provider->instance);
            provider->instance = NULL;
        }
    }
    (void)memset(registry, 0, sizeof(*registry));
}

/* Add ai provider registry only after its inputs and available capacity have been checked. */
UmiStatus umi_ai_provider_registry_add(UmiAiProviderRegistry *registry,
                                       const UmiAiProvider *provider)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || umi_ai_provider_validate(provider) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ai_provider_registry_find(registry, provider->provider_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_AI_MAX_PROVIDERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->providers[registry->count++] = *provider;
    return UMI_STATUS_OK;
}

/*
 * Find ai provider registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiAiProvider *umi_ai_provider_registry_find(UmiAiProviderRegistry *registry,
                                               const char *provider_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->providers[index].provider_id, provider_id) == 0) {
            return &registry->providers[index];
        }
    }
    return NULL;
}
