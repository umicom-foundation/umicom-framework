/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/factory_registry.c
 *
 * PURPOSE:
 *   Implement the factory registry behavior for
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
 * File: src/runtime/bootstrap/factory_registry.c
 *
 * PURPOSE:
 *   Maintain and query bounded Framework service-factory definitions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/factory_registry.h"
#include "umicom/runtime/bootstrap/factory_descriptor.h"


#include <string.h>
/*
 * Initialise bootstrap factory registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_factory_registry_init(UmiBootstrapFactoryRegistry *registry) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) memset(registry, 0, sizeof(*registry));
}
/*
 * Find bootstrap factory registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiBootstrapFactoryDescriptor *umi_bootstrap_factory_registry_find(
    const UmiBootstrapFactoryRegistry *registry,
    const char *factory_id) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || factory_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i].factory_id, factory_id) == 0) return &registry->items[i];
    return NULL;
}
/*
 * Add bootstrap factory registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_factory_registry_add(
    UmiBootstrapFactoryRegistry *registry,
    const UmiBootstrapFactoryDescriptor *descriptor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || !umi_bootstrap_factory_descriptor_valid(descriptor))
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_bootstrap_factory_registry_find(registry, descriptor->factory_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *descriptor;
    return UMI_STATUS_OK;
}
