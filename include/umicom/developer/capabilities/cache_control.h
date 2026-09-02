/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/cache_control.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Cache Control developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_CACHE_CONTROL_H
#define UMICOM_DEVELOPER_CAPABILITIES_CACHE_CONTROL_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_CACHE_CONTROL "umicom.developer.configuration.cache_control"

/**
 * Provide the developer capability cache control operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_cache_control(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_CACHE_CONTROL_H */
