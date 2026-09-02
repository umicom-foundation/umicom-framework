/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/binding_registry.h
 *
 * PURPOSE:
 *   Maintain deterministic binding candidates and select the highest-priority match.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_BINDING_REGISTRY_H
#define UMICOM_RUNTIME_BOOTSTRAP_BINDING_REGISTRY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap binding registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_binding_registry_init(UmiBootstrapBindingRegistry *registry);
/**
 * Add bootstrap binding registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_binding_registry_add(
    UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapBindingDescriptor *binding);
/**
 * Provide the bootstrap binding registry find best operation used by this module and its
 * client applications.
 */
const UmiBootstrapBindingDescriptor *umi_bootstrap_binding_registry_find_best(
    const UmiBootstrapBindingRegistry *registry,
    const UmiBootstrapServiceKey *key);
/**
 * Return the number of records represented by bootstrap binding registry without changing
 * their state.
 */
size_t umi_bootstrap_binding_registry_count(
    const UmiBootstrapBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
