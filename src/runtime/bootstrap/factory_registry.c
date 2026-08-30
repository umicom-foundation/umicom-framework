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
void umi_bootstrap_factory_registry_init(UmiBootstrapFactoryRegistry *registry) {
    if (registry != NULL) memset(registry, 0, sizeof(*registry));
}
const UmiBootstrapFactoryDescriptor *umi_bootstrap_factory_registry_find(
    const UmiBootstrapFactoryRegistry *registry,
    const char *factory_id) {
    size_t i;
    if (registry == NULL || factory_id == NULL) return NULL;
    for (i = 0U; i < registry->count; ++i)
        if (strcmp(registry->items[i].factory_id, factory_id) == 0) return &registry->items[i];
    return NULL;
}
UmiStatus umi_bootstrap_factory_registry_add(
    UmiBootstrapFactoryRegistry *registry,
    const UmiBootstrapFactoryDescriptor *descriptor) {
    if (registry == NULL || !umi_bootstrap_factory_descriptor_valid(descriptor))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_bootstrap_factory_registry_find(registry, descriptor->factory_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *descriptor;
    return UMI_STATUS_OK;
}
