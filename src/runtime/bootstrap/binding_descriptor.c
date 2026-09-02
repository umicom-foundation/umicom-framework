/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/binding_descriptor.c
 *
 * PURPOSE:
 *   Implement the binding descriptor behavior for
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
 * File: src/runtime/bootstrap/binding_descriptor.c
 *
 * PURPOSE:
 *   Describe factory, instance and alias bindings selected by dependency resolution.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/binding_descriptor.h"


#include <string.h>

/*
 * Initialise bootstrap binding descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_binding_descriptor_init(
    UmiBootstrapBindingDescriptor *binding,
    const UmiBootstrapServiceKey *key,
    UmiBootstrapBindingKind kind,
    const char *implementation_id,
    const char *target_id,
    int32_t priority) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(binding, 0, sizeof(*binding));
    binding->key = *key;
    binding->kind = kind;
    binding->priority = priority;
    binding->enabled = true;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (implementation_id != NULL && implementation_id[0] != '\0') {
        status = umi_bootstrap_copy_text(binding->implementation_id,
                                         sizeof(binding->implementation_id),
                                         implementation_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (target_id != NULL && target_id[0] != '\0') {
        status = umi_bootstrap_copy_text(binding->target_id,
                                         sizeof(binding->target_id), target_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_bootstrap_binding_descriptor_valid(binding)
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Check that bootstrap binding descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_bootstrap_binding_descriptor_valid(
    const UmiBootstrapBindingDescriptor *binding) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || !umi_bootstrap_id_valid(binding->key.service_id)) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (binding->kind < UMI_BOOTSTRAP_BIND_FACTORY ||
        binding->kind > UMI_BOOTSTRAP_BIND_ALIAS) return false;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding->kind == UMI_BOOTSTRAP_BIND_INSTANCE) return binding->instance != NULL ||
        binding->implementation_id[0] != '\0';
    /* Apply this branch only when its contract condition is satisfied. */
    if (binding->kind == UMI_BOOTSTRAP_BIND_ALIAS) return
        umi_bootstrap_id_valid(binding->target_id) &&
        umi_bootstrap_text_compare(binding->key.service_id, binding->target_id) != 0;
    return umi_bootstrap_id_valid(binding->implementation_id);
}
