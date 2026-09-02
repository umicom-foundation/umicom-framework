/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/service_descriptor.h
 *
 * PURPOSE:
 *   Describe Framework services, ownership scope and lifetime without global variables.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_SERVICE_DESCRIPTOR_H
#define UMICOM_RUNTIME_BOOTSTRAP_SERVICE_DESCRIPTOR_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap service descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_service_descriptor_init(
    UmiBootstrapServiceDescriptor *descriptor,
    const char *service_id,
    const char *qualifier,
    const char *provider_id,
    UmiBootstrapScopeKind scope,
    UmiBootstrapLifetimeKind lifetime,
    int32_t priority);
/**
 * Check that bootstrap service descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_bootstrap_service_descriptor_valid(
    const UmiBootstrapServiceDescriptor *descriptor);

#ifdef __cplusplus
}
#endif

#endif
