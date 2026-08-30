/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/service_descriptor.c
 *
 * PURPOSE:
 *   Implement the service descriptor behavior for
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
 * File: src/runtime/bootstrap/service_descriptor.c
 *
 * PURPOSE:
 *   Describe Framework services, ownership scope and lifetime without global variables.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/service_descriptor.h"
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>

UmiStatus umi_bootstrap_service_descriptor_init(
    UmiBootstrapServiceDescriptor *descriptor,
    const char *service_id,
    const char *qualifier,
    const char *provider_id,
    UmiBootstrapScopeKind scope,
    UmiBootstrapLifetimeKind lifetime,
    int32_t priority) {
    UmiStatus status;
    if (descriptor == NULL || !umi_bootstrap_id_valid(provider_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(descriptor, 0, sizeof(*descriptor));
    status = umi_bootstrap_service_key_init(&descriptor->key, service_id, qualifier);
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(descriptor->provider_id,
                                     sizeof(descriptor->provider_id), provider_id);
    if (status != UMI_STATUS_OK) return status;
    descriptor->scope = scope;
    descriptor->lifetime = lifetime;
    descriptor->priority = priority;
    descriptor->enabled = true;
    return umi_bootstrap_service_descriptor_valid(descriptor)
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

bool umi_bootstrap_service_descriptor_valid(
    const UmiBootstrapServiceDescriptor *descriptor) {
    if (descriptor == NULL) return false;
    if (!umi_bootstrap_id_valid(descriptor->key.service_id) ||
        !umi_bootstrap_id_valid(descriptor->provider_id)) return false;
    if (descriptor->scope < UMI_BOOTSTRAP_SCOPE_SINGLETON ||
        descriptor->scope > UMI_BOOTSTRAP_SCOPE_TRANSIENT) return false;
    return descriptor->lifetime >= UMI_BOOTSTRAP_LIFETIME_EAGER &&
           descriptor->lifetime <= UMI_BOOTSTRAP_LIFETIME_EXTERNAL;
}
