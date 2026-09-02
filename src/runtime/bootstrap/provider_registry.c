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
/*
 * Initialise bootstrap provider registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_provider_registry_init(UmiBootstrapProviderRegistry *registry) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) memset(registry, 0, sizeof(*registry));
}
/*
 * Add bootstrap provider registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_provider_registry_add(
    UmiBootstrapProviderRegistry *registry,
    const UmiBootstrapProviderDescriptor *descriptor) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || !umi_bootstrap_provider_descriptor_valid(descriptor))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i].provider_id, descriptor->provider_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *descriptor;
    return UMI_STATUS_OK;
}
/*
 * Provide the bootstrap provider registry best operation used by this module and its
 * client applications.
 */
const UmiBootstrapProviderDescriptor *umi_bootstrap_provider_registry_best(
    const UmiBootstrapProviderRegistry *registry) {
    const UmiBootstrapProviderDescriptor *best = NULL;
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        const UmiBootstrapProviderDescriptor *item = &registry->items[i];
        /* Apply this operation only while the related capability or state is available. */
        if (!item->enabled) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (best == NULL || item->priority > best->priority ||
            (item->priority == best->priority &&
             strcmp(item->provider_id, best->provider_id) < 0)) best = item;
    }
    return best;
}
