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


void umi_bootstrap_factory_registry_init(UmiBootstrapFactoryRegistry *registry);
UmiStatus umi_bootstrap_factory_registry_add(
    UmiBootstrapFactoryRegistry *registry,
    const UmiBootstrapFactoryDescriptor *descriptor);
const UmiBootstrapFactoryDescriptor *umi_bootstrap_factory_registry_find(
    const UmiBootstrapFactoryRegistry *registry,
    const char *factory_id);

#ifdef __cplusplus
}
#endif

#endif
