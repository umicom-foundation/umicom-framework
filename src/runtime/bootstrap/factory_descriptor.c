/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/factory_descriptor.c
 *
 * PURPOSE:
 *   Implement the factory descriptor behavior for
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
 * File: src/runtime/bootstrap/factory_descriptor.c
 *
 * PURPOSE:
 *   Describe named factories that create services for the canonical service registry.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/factory_descriptor.h"


#include <string.h>
UmiStatus umi_bootstrap_factory_descriptor_init(
    UmiBootstrapFactoryDescriptor *descriptor,
    const char *factory_id,
    const UmiBootstrapServiceKey *produces,
    int32_t priority) {
    UmiStatus status;
    if (descriptor == NULL || produces == NULL || !umi_bootstrap_id_valid(factory_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(descriptor, 0, sizeof(*descriptor));
    status = umi_bootstrap_copy_text(descriptor->factory_id,
        sizeof(descriptor->factory_id), factory_id);
    if (status != UMI_STATUS_OK) return status;
    descriptor->produces = *produces;
    descriptor->priority = priority;
    descriptor->enabled = true;
    return UMI_STATUS_OK;
}
bool umi_bootstrap_factory_descriptor_valid(
    const UmiBootstrapFactoryDescriptor *descriptor) {
    return descriptor != NULL &&
           umi_bootstrap_id_valid(descriptor->factory_id) &&
           umi_bootstrap_id_valid(descriptor->produces.service_id);
}
