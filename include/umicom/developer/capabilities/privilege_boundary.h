/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/privilege_boundary.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Privilege Boundary developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PRIVILEGE_BOUNDARY_H
#define UMICOM_DEVELOPER_CAPABILITIES_PRIVILEGE_BOUNDARY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PRIVILEGE_BOUNDARY "umicom.developer.security.privilege_boundary"

/**
 * Provide the developer capability privilege boundary operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_privilege_boundary(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PRIVILEGE_BOUNDARY_H */
