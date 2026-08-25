/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/factory_descriptor.h
 *
 * PURPOSE:
 *   Describe named factories that create services for the canonical service registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_FACTORY_DESCRIPTOR_H
#define UMICOM_RUNTIME_BOOTSTRAP_FACTORY_DESCRIPTOR_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_factory_descriptor_init(
    UmiBootstrapFactoryDescriptor *descriptor,
    const char *factory_id,
    const UmiBootstrapServiceKey *produces,
    int32_t priority);
bool umi_bootstrap_factory_descriptor_valid(
    const UmiBootstrapFactoryDescriptor *descriptor);

#ifdef __cplusplus
}
#endif

#endif
