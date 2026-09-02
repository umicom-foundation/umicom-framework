/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/instance_binding.c
 *
 * PURPOSE:
 *   Implement the instance binding behavior for
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
 * File: src/runtime/bootstrap/instance_binding.c
 *
 * PURPOSE:
 *   Validate direct instance bindings for pre-built Framework services.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/instance_binding.h"


/*
 * Provide the bootstrap instance binding configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_instance_binding_configure(
    UmiBootstrapBindingDescriptor *binding,
    void *instance) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || instance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    binding->kind = UMI_BOOTSTRAP_BIND_INSTANCE;
    binding->instance = instance;
    binding->enabled = true;
    return UMI_STATUS_OK;
}
/*
 * Provide the bootstrap instance binding ready operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_instance_binding_ready(
    const UmiBootstrapBindingDescriptor *binding) {
    return binding != NULL && binding->enabled &&
           binding->kind == UMI_BOOTSTRAP_BIND_INSTANCE &&
           binding->instance != NULL;
}
