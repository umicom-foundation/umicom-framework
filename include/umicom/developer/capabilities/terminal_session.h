/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_session.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal Session developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_SESSION_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_SESSION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_SESSION "umicom.developer.terminal.terminal_session"

/**
 * Provide the developer capability terminal session operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_session(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_SESSION_H */
