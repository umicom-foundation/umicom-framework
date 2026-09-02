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

/*
 * Initialise bootstrap service descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_service_descriptor_init(
    UmiBootstrapServiceDescriptor *descriptor,
    const char *service_id,
    const char *qualifier,
    const char *provider_id,
    UmiBootstrapScopeKind scope,
    UmiBootstrapLifetimeKind lifetime,
    int32_t priority) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || !umi_bootstrap_id_valid(provider_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(descriptor, 0, sizeof(*descriptor));
    status = umi_bootstrap_service_key_init(&descriptor->key, service_id, qualifier);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(descriptor->provider_id,
                                     sizeof(descriptor->provider_id), provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    descriptor->scope = scope;
    descriptor->lifetime = lifetime;
    descriptor->priority = priority;
    descriptor->enabled = true;
    return umi_bootstrap_service_descriptor_valid(descriptor)
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Check that bootstrap service descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_bootstrap_service_descriptor_valid(
    const UmiBootstrapServiceDescriptor *descriptor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) return false;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_bootstrap_id_valid(descriptor->key.service_id) ||
        !umi_bootstrap_id_valid(descriptor->provider_id)) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->scope < UMI_BOOTSTRAP_SCOPE_SINGLETON ||
        descriptor->scope > UMI_BOOTSTRAP_SCOPE_TRANSIENT) return false;
    return descriptor->lifetime >= UMI_BOOTSTRAP_LIFETIME_EAGER &&
           descriptor->lifetime <= UMI_BOOTSTRAP_LIFETIME_EXTERNAL;
}
