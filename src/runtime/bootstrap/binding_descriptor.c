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

UmiStatus umi_bootstrap_binding_descriptor_init(
    UmiBootstrapBindingDescriptor *binding,
    const UmiBootstrapServiceKey *key,
    UmiBootstrapBindingKind kind,
    const char *implementation_id,
    const char *target_id,
    int32_t priority) {
    UmiStatus status;
    if (binding == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(binding, 0, sizeof(*binding));
    binding->key = *key;
    binding->kind = kind;
    binding->priority = priority;
    binding->enabled = true;
    if (implementation_id != NULL && implementation_id[0] != '\0') {
        status = umi_bootstrap_copy_text(binding->implementation_id,
                                         sizeof(binding->implementation_id),
                                         implementation_id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (target_id != NULL && target_id[0] != '\0') {
        status = umi_bootstrap_copy_text(binding->target_id,
                                         sizeof(binding->target_id), target_id);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_bootstrap_binding_descriptor_valid(binding)
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

bool umi_bootstrap_binding_descriptor_valid(
    const UmiBootstrapBindingDescriptor *binding) {
    if (binding == NULL || !umi_bootstrap_id_valid(binding->key.service_id)) return false;
    if (binding->kind < UMI_BOOTSTRAP_BIND_FACTORY ||
        binding->kind > UMI_BOOTSTRAP_BIND_ALIAS) return false;
    if (binding->kind == UMI_BOOTSTRAP_BIND_INSTANCE) return binding->instance != NULL ||
        binding->implementation_id[0] != '\0';
    if (binding->kind == UMI_BOOTSTRAP_BIND_ALIAS) return
        umi_bootstrap_id_valid(binding->target_id) &&
        umi_bootstrap_text_compare(binding->key.service_id, binding->target_id) != 0;
    return umi_bootstrap_id_valid(binding->implementation_id);
}
