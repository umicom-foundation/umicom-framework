/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/build_cache.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Build Cache developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_BUILD_CACHE_H
#define UMICOM_DEVELOPER_CAPABILITIES_BUILD_CACHE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_BUILD_CACHE "umicom.developer.performance.build_cache"

/**
 * Provide the developer capability build cache operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_build_cache(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_BUILD_CACHE_H */
