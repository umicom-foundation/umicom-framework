/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/binding_registry.h
 *
 * PURPOSE:
 *   Maintain deterministic binding candidates and select the highest-priority match.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BINDING_REGISTRY_H
#define UMICOM_RUNTIME_BOOTSTRAP_BINDING_REGISTRY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_binding_registry_init(UmiBootstrapBindingRegistry *registry);
UmiStatus umi_bootstrap_binding_registry_add(
    UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapBindingDescriptor *binding);
const UmiBootstrapBindingDescriptor *umi_bootstrap_binding_registry_find_best(
    const UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapServiceKey *key);
size_t umi_bootstrap_binding_registry_count(
    const UmiBootstrapBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
