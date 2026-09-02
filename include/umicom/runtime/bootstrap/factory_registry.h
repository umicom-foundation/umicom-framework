/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/factory_registry.h
 *
 * PURPOSE:
 *   Maintain and query bounded Framework service-factory definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_FACTORY_REGISTRY_H
#define UMICOM_RUNTIME_BOOTSTRAP_FACTORY_REGISTRY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap factory registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_factory_registry_init(UmiBootstrapFactoryRegistry *registry);
/**
 * Add bootstrap factory registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_factory_registry_add(
    UmiBootstrapFactoryRegistry *registry,
    const UmiBootstrapFactoryDescriptor *descriptor);
/**
 * Find bootstrap factory registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiBootstrapFactoryDescriptor *umi_bootstrap_factory_registry_find(
    const UmiBootstrapFactoryRegistry *registry,
    const char *factory_id);

#ifdef __cplusplus
}
#endif

#endif
