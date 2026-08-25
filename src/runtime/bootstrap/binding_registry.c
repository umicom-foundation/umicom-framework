/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/binding_registry.c
 *
 * PURPOSE:
 *   Maintain deterministic binding candidates and select the highest-priority match.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/binding_registry.h"
#include "umicom/runtime/bootstrap/binding_descriptor.h"
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>

void umi_bootstrap_binding_registry_init(UmiBootstrapBindingRegistry *registry) {
    if (registry != NULL) memset(registry, 0, sizeof(*registry));
}

UmiStatus umi_bootstrap_binding_registry_add(
    UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapBindingDescriptor *binding) {
    size_t index;
    if (registry == NULL || !umi_bootstrap_binding_descriptor_valid(binding)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (umi_bootstrap_service_key_equal(&registry->items[index].key, &binding->key) &&
            registry->items[index].kind == binding->kind &&
            strcmp(registry->items[index].implementation_id, binding->implementation_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (registry->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *binding;
    ++registry->revision;
    return UMI_STATUS_OK;
}

const UmiBootstrapBindingDescriptor *umi_bootstrap_binding_registry_find_best(
    const UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapServiceKey *key) {
    const UmiBootstrapBindingDescriptor *best = NULL;
    size_t index;
    if (registry == NULL || key == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        const UmiBootstrapBindingDescriptor *item = &registry->items[index];
        if (!item->enabled || !umi_bootstrap_service_key_equal(&item->key, key)) continue;
        if (best == NULL || item->primary > best->primary ||
            (item->primary == best->primary && item->priority > best->priority) ||
            (item->primary == best->primary && item->priority == best->priority &&
             strcmp(item->implementation_id, best->implementation_id) < 0)) {
            best = item;
        }
    }
    return best;
}

size_t umi_bootstrap_binding_registry_count(
    const UmiBootstrapBindingRegistry *registry) {
    return registry != NULL ? registry->count : 0U;
}
