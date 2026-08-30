/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/provider_registry.c
 *
 * PURPOSE:
 *   Implement the provider registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/provider_registry.c
 *
 * PURPOSE:
 *   Register and rank service-provider modules deterministically.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/provider_registry.h"
#include "umicom/runtime/bootstrap/provider_descriptor.h"


#include <string.h>
void umi_bootstrap_provider_registry_init(UmiBootstrapProviderRegistry *registry) {
    if (registry != NULL) memset(registry, 0, sizeof(*registry));
}
UmiStatus umi_bootstrap_provider_registry_add(
    UmiBootstrapProviderRegistry *registry,
    const UmiBootstrapProviderDescriptor *descriptor) {
    size_t i;
    if (registry == NULL || !umi_bootstrap_provider_descriptor_valid(descriptor))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < registry->count; ++i)
        if (strcmp(registry->items[i].provider_id, descriptor->provider_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *descriptor;
    return UMI_STATUS_OK;
}
const UmiBootstrapProviderDescriptor *umi_bootstrap_provider_registry_best(
    const UmiBootstrapProviderRegistry *registry) {
    const UmiBootstrapProviderDescriptor *best = NULL;
    size_t i;
    if (registry == NULL) return NULL;
    for (i = 0U; i < registry->count; ++i) {
        const UmiBootstrapProviderDescriptor *item = &registry->items[i];
        if (!item->enabled) continue;
        if (best == NULL || item->priority > best->priority ||
            (item->priority == best->priority &&
             strcmp(item->provider_id, best->provider_id) < 0)) best = item;
    }
    return best;
}
