/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/constructor_binding.c
 *
 * PURPOSE:
 *   Capture constructor-style dependency lists for C factory functions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/constructor_binding.h"
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>
UmiStatus umi_bootstrap_constructor_binding_init(
    UmiBootstrapConstructorBinding *binding,
    const char *implementation_id) {
    if (binding == NULL || !umi_bootstrap_id_valid(implementation_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(binding, 0, sizeof(*binding));
    return umi_bootstrap_copy_text(binding->implementation_id,
        sizeof(binding->implementation_id), implementation_id);
}
UmiStatus umi_bootstrap_constructor_binding_add_dependency(
    UmiBootstrapConstructorBinding *binding,
    const UmiBootstrapDependencyDescriptor *dependency) {
    size_t i;
    if (binding == NULL || dependency == NULL ||
        !umi_bootstrap_id_valid(dependency->key.service_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < binding->dependency_count; ++i)
        if (umi_bootstrap_service_key_equal(&binding->dependencies[i].key, &dependency->key))
            return UMI_STATUS_ALREADY_EXISTS;
    if (binding->dependency_count >= UMI_BOOTSTRAP_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    binding->dependencies[binding->dependency_count++] = *dependency;
    return UMI_STATUS_OK;
}
