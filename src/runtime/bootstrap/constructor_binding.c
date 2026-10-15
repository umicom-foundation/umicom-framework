/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/constructor_binding.c
 *
 * PURPOSE:
 *   Implement the constructor binding behavior for
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
 * File: src/runtime/bootstrap/constructor_binding.c
 *
 * PURPOSE:
 *   Capture constructor-style dependency lists for C factory functions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/constructor_binding.h"
#include "../../base/value_construction_internal.h"
#include "umicom/runtime/bootstrap/service_key.h"


#include <string.h>
/*
 * Initialise bootstrap constructor binding from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_bootstrap_constructor_binding_init(
    UmiBootstrapConstructorBinding *binding,
    const char *implementation_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || !umi_bootstrap_id_valid(implementation_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(binding, 0, sizeof(*binding));
    return umi_bootstrap_copy_text(binding->implementation_id,
        sizeof(binding->implementation_id), implementation_id);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_bootstrap_constructor_binding_init_build_value(
    UmiBootstrapConstructorBinding *binding,
    const char *implementation_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || !umi_bootstrap_id_valid(implementation_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(binding, 0, sizeof(*binding));
    return umi_bootstrap_copy_text(binding->implementation_id,
        sizeof(binding->implementation_id), implementation_id);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_bootstrap_constructor_binding_init, UmiBootstrapConstructorBinding, binding,
    (UmiBootstrapConstructorBinding *binding, const char *implementation_id),
    umi_bootstrap_constructor_binding_init_build_value(candidate, implementation_id))

/*
 * Provide the bootstrap constructor binding add dependency operation used by this module
 * and its client applications.
 */
UmiStatus umi_bootstrap_constructor_binding_add_dependency(
    UmiBootstrapConstructorBinding *binding,
    const UmiBootstrapDependencyDescriptor *dependency) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || dependency == NULL ||
        !umi_bootstrap_id_valid(dependency->key.service_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < binding->dependency_count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_bootstrap_service_key_equal(&binding->dependencies[i].key, &dependency->key))
            return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (binding->dependency_count >= UMI_BOOTSTRAP_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    binding->dependencies[binding->dependency_count++] = *dependency;
    return UMI_STATUS_OK;
}
