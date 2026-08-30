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


UmiStatus umi_bootstrap_instance_binding_configure(
    UmiBootstrapBindingDescriptor *binding,
    void *instance);
bool umi_bootstrap_instance_binding_ready(
    const UmiBootstrapBindingDescriptor *binding);

#ifdef __cplusplus
}
#endif

#endif
