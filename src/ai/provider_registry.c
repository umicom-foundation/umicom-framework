/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/provider_registry.c
 *
 * PURPOSE:
 *   Register and resolve AI providers by identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Registration copies the small function table, while each provider keeps its private implementation behind the opaque instance pointer.
 */

#include "umicom/ai/provider_registry.h"
#include <stddef.h>

#include <string.h>

void umi_ai_provider_registry_init(UmiAiProviderRegistry *registry)
{
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
    }
}

UmiStatus umi_ai_provider_registry_add(UmiAiProviderRegistry *registry,
                                       const UmiAiProviderV1 *provider)
{
    if (registry == NULL || umi_ai_provider_validate(provider) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_ai_provider_registry_find(registry, provider->provider_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= UMI_AI_MAX_PROVIDERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->providers[registry->count++] = *provider;
    return UMI_STATUS_OK;
}

UmiAiProviderV1 *umi_ai_provider_registry_find(UmiAiProviderRegistry *registry,
                                               const char *provider_id)
{
    size_t index;
    if (registry == NULL || provider_id == NULL) {
        return NULL;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->providers[index].provider_id, provider_id) == 0) {
            return &registry->providers[index];
        }
    }
    return NULL;
}
