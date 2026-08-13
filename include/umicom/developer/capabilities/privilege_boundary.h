/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/privilege_boundary.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Privilege Boundary developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PRIVILEGE_BOUNDARY_H
#define UMICOM_DEVELOPER_CAPABILITIES_PRIVILEGE_BOUNDARY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PRIVILEGE_BOUNDARY "umicom.developer.security.privilege_boundary"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_privilege_boundary(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PRIVILEGE_BOUNDARY_H */
