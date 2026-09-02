/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/resolver.h
 *
 * PURPOSE:
 *   Select the best enabled binding using qualifier, primary flag and priority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_RESOLVER_H
#define UMICOM_RUNTIME_BOOTSTRAP_RESOLVER_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap resolver select operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_resolver_select(
    const UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapResolutionRequest *request,
    const UmiBootstrapBindingDescriptor **out_binding,
    UmiBootstrapResolutionTrace *trace);

#ifdef __cplusplus
}
#endif

#endif
