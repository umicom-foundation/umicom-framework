/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/resolver.h
 *
 * PURPOSE:
 *   Select the best enabled binding using qualifier, primary flag and priority.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_RESOLVER_H
#define UMICOM_RUNTIME_BOOTSTRAP_RESOLVER_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_resolver_select(
    const UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapResolutionRequest *request,
    const UmiBootstrapBindingDescriptor **out_binding,
    UmiBootstrapResolutionTrace *trace);

#ifdef __cplusplus
}
#endif

#endif
