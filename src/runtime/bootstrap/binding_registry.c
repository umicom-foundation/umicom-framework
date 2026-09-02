/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/binding_registry.c
 *
 * PURPOSE:
 *   Implement the binding registry behavior for
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
 * File: src/runtime/bootstrap/binding_registry.c
 *
 * PURPOSE:
 *   Maintain deterministic binding candidates and select the highest-priority match.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/binding_registry.h"
#include "umicom/runtime/bootstrap/binding_descriptor.h"
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>

/*
 * Initialise bootstrap binding registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_binding_registry_init(UmiBootstrapBindingRegistry *registry) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) memset(registry, 0, sizeof(*registry));
}

/*
 * Add bootstrap binding registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_binding_registry_add(
    UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapBindingDescriptor *binding) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || !umi_bootstrap_binding_descriptor_valid(binding)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_bootstrap_service_key_equal(&registry->items[index].key, &binding->key) &&
            registry->items[index].kind == binding->kind &&
            strcmp(registry->items[index].implementation_id, binding->implementation_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *binding;
    ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the bootstrap binding registry find best operation used by this module and its
 * client applications.
 */
const UmiBootstrapBindingDescriptor *umi_bootstrap_binding_registry_find_best(
    const UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapServiceKey *key) {
    const UmiBootstrapBindingDescriptor *best = NULL;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || key == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        const UmiBootstrapBindingDescriptor *item = &registry->items[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!item->enabled || !umi_bootstrap_service_key_equal(&item->key, key)) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (best == NULL || item->primary > best->primary ||
            (item->primary == best->primary && item->priority > best->priority) ||
            (item->primary == best->primary && item->priority == best->priority &&
             strcmp(item->implementation_id, best->implementation_id) < 0)) {
            best = item;
        }
    }
    return best;
}

/*
 * Return the number of records represented by bootstrap binding registry without changing
 * their state.
 */
size_t umi_bootstrap_binding_registry_count(
    const UmiBootstrapBindingRegistry *registry) {
    return registry != NULL ? registry->count : 0U;
}
