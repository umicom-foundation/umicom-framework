/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/provider_descriptor.c
 *
 * PURPOSE:
 *   Implement the provider descriptor behavior for
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
 * File: src/runtime/bootstrap/provider_descriptor.c
 *
 * PURPOSE:
 *   Describe modules that contribute replaceable service implementations.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/provider_descriptor.h"


#include <string.h>
/*
 * Initialise bootstrap provider descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_provider_descriptor_init(
    UmiBootstrapProviderDescriptor *descriptor,
    const char *provider_id,
    const char *module_id,
    int32_t priority) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || !umi_bootstrap_id_valid(provider_id) ||
        !umi_bootstrap_id_valid(module_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(descriptor, 0, sizeof(*descriptor));
    status = umi_bootstrap_copy_text(descriptor->provider_id,
        sizeof(descriptor->provider_id), provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(descriptor->module_id,
        sizeof(descriptor->module_id), module_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    descriptor->priority = priority;
    descriptor->enabled = true;
    return UMI_STATUS_OK;
}
/*
 * Check that bootstrap provider descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_bootstrap_provider_descriptor_valid(
    const UmiBootstrapProviderDescriptor *descriptor) {
    return descriptor != NULL &&
           umi_bootstrap_id_valid(descriptor->provider_id) &&
           umi_bootstrap_id_valid(descriptor->module_id);
}
