/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/provider_registry.c
 *
 * PURPOSE:
 *   Implement the bounded provider-neutral broker adapter catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/provider_registry.h"

#include <string.h>

void umi_broker_provider_registry_init(UmiBrokerProviderRegistry *registry)
{
    if (registry == NULL) return;
    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
}

const UmiBrokerProviderDescriptor *umi_broker_provider_registry_find(
    const UmiBrokerProviderRegistry *registry,
    const char *id)
{
    size_t index;
    if (registry == NULL || id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->providers[index].id, id) == 0) {
            return &registry->providers[index];
        }
    }
    return NULL;
}

UmiStatus umi_broker_provider_registry_register(
    UmiBrokerProviderRegistry *registry,
    const UmiBrokerProviderDescriptor *descriptor)
{
    if (registry == NULL || descriptor == NULL ||
        descriptor->id[0] == '\0' || descriptor->displayName[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_broker_provider_registry_find(registry, descriptor->id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= UMI_BROKER_PROVIDER_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->providers[registry->count++] = *descriptor;
    registry->revision =
        registry->revision == UINT64_MAX ? 1U : registry->revision + 1U;
    return UMI_STATUS_OK;
}
