/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/workspace_discovery.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Workspace Discovery developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_WORKSPACE_DISCOVERY_H
#define UMICOM_DEVELOPER_CAPABILITIES_WORKSPACE_DISCOVERY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_WORKSPACE_DISCOVERY "umicom.developer.configuration.workspace_discovery"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_workspace_discovery(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_WORKSPACE_DISCOVERY_H */
