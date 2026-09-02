/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/compile_commands.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Compile Commands developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_COMPILE_COMMANDS_H
#define UMICOM_DEVELOPER_CAPABILITIES_COMPILE_COMMANDS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_COMPILE_COMMANDS "umicom.developer.build.compile_commands"

/**
 * Provide the developer capability compile commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_compile_commands(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_COMPILE_COMMANDS_H */
