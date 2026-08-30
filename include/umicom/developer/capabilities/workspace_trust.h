/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/workspace_trust.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Workspace Trust developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_WORKSPACE_TRUST_H
#define UMICOM_DEVELOPER_CAPABILITIES_WORKSPACE_TRUST_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_WORKSPACE_TRUST "umicom.developer.security.workspace_trust"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_workspace_trust(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_WORKSPACE_TRUST_H */
