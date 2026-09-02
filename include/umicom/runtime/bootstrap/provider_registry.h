/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/provider_registry.h
 *
 * PURPOSE:
 *   Register and rank service-provider modules deterministically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_PROVIDER_REGISTRY_H
#define UMICOM_RUNTIME_BOOTSTRAP_PROVIDER_REGISTRY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap provider registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_provider_registry_init(UmiBootstrapProviderRegistry *registry);
/**
 * Add bootstrap provider registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_provider_registry_add(
    UmiBootstrapProviderRegistry *registry,
    const UmiBootstrapProviderDescriptor *descriptor);
/**
 * Provide the bootstrap provider registry best operation used by this module and its
 * client applications.
 */
const UmiBootstrapProviderDescriptor *umi_bootstrap_provider_registry_best(
    const UmiBootstrapProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
