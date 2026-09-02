/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/instance_binding.h
 *
 * PURPOSE:
 *   Validate direct instance bindings for pre-built Framework services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_INSTANCE_BINDING_H
#define UMICOM_RUNTIME_BOOTSTRAP_INSTANCE_BINDING_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap instance binding configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_instance_binding_configure(
    UmiBootstrapBindingDescriptor *binding,
    void *instance);
/**
 * Provide the bootstrap instance binding ready operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_instance_binding_ready(
    const UmiBootstrapBindingDescriptor *binding);

#ifdef __cplusplus
}
#endif

#endif
