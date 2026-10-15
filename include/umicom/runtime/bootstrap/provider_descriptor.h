/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/provider_descriptor.h
 *
 * PURPOSE:
 *   Describe modules that contribute replaceable service implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_PROVIDER_DESCRIPTOR_H
#define UMICOM_RUNTIME_BOOTSTRAP_PROVIDER_DESCRIPTOR_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap provider descriptor from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_bootstrap_provider_descriptor_init(
    UmiBootstrapProviderDescriptor *descriptor,
    const char *provider_id,
    const char *module_id,
    int32_t priority);
/**
 * Check that bootstrap provider descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_bootstrap_provider_descriptor_valid(
    const UmiBootstrapProviderDescriptor *descriptor);

#ifdef __cplusplus
}
#endif

#endif
