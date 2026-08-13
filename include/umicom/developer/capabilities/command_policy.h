/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/command_policy.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Command Policy developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_COMMAND_POLICY_H
#define UMICOM_DEVELOPER_CAPABILITIES_COMMAND_POLICY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_COMMAND_POLICY "umicom.developer.security.command_policy"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_command_policy(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_COMMAND_POLICY_H */
