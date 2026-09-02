/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/elevation_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Elevation Policy developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_ELEVATION_POLICY_H
#define UMICOM_DEVELOPER_CAPABILITIES_ELEVATION_POLICY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_ELEVATION_POLICY "umicom.developer.execution.elevation_policy"

/**
 * Provide the developer capability elevation policy operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_elevation_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_ELEVATION_POLICY_H */
