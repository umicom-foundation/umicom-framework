/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/binding_descriptor.h
 *
 * PURPOSE:
 *   Describe factory, instance and alias bindings selected by dependency resolution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BINDING_DESCRIPTOR_H
#define UMICOM_RUNTIME_BOOTSTRAP_BINDING_DESCRIPTOR_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_binding_descriptor_init(
    UmiBootstrapBindingDescriptor *binding,
    const UmiBootstrapServiceKey *key,
    UmiBootstrapBindingKind kind,
    const char *implementation_id,
    const char *target_id,
    int32_t priority);
bool umi_bootstrap_binding_descriptor_valid(
    const UmiBootstrapBindingDescriptor *binding);

#ifdef __cplusplus
}
#endif

#endif
