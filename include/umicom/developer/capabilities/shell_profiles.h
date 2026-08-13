/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/shell_profiles.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Shell Profiles developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_SHELL_PROFILES_H
#define UMICOM_DEVELOPER_CAPABILITIES_SHELL_PROFILES_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_SHELL_PROFILES "umicom.developer.terminal.shell_profiles"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_shell_profiles(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_SHELL_PROFILES_H */
