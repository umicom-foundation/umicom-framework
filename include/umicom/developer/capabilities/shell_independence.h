/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/shell_independence.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Shell Independence developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_SHELL_INDEPENDENCE_H
#define UMICOM_DEVELOPER_CAPABILITIES_SHELL_INDEPENDENCE_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_SHELL_INDEPENDENCE "umicom.developer.portability.shell_independence"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_shell_independence(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_SHELL_INDEPENDENCE_H */
