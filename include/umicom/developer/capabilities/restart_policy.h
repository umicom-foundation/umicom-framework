/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/restart_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Restart Policy developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_RESTART_POLICY_H
#define UMICOM_DEVELOPER_CAPABILITIES_RESTART_POLICY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_RESTART_POLICY "umicom.developer.execution.restart_policy"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_restart_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_RESTART_POLICY_H */
