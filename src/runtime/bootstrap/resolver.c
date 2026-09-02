/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/resolver.c
 *
 * PURPOSE:
 *   Implement the resolver behavior for
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
 * File: src/runtime/bootstrap/resolver.c
 *
 * PURPOSE:
 *   Select the best enabled binding using qualifier, primary flag and priority.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/resolver.h"
#include "umicom/runtime/bootstrap/binding_registry.h"
#include "umicom/runtime/bootstrap/resolution_trace.h"


/*
 * Provide the bootstrap resolver select operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_resolver_select(
    const UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapResolutionRequest *request,
    const UmiBootstrapBindingDescriptor **out_binding,
    UmiBootstrapResolutionTrace *trace) {
    const UmiBootstrapBindingDescriptor *binding;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || request == NULL || out_binding == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    binding = umi_bootstrap_binding_registry_find_best(registry, &request->key);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (trace != NULL) (void)umi_bootstrap_resolution_trace_append(trace, "binding:not-found");
        *out_binding = NULL;
        return request->required ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
    }
    *out_binding = binding;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (trace != NULL) {
        (void)umi_bootstrap_resolution_trace_append(trace,
            binding->primary ? "binding:primary" : "binding:priority");
    }
    return UMI_STATUS_OK;
}
