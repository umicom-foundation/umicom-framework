/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_history.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal History developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_HISTORY_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_HISTORY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_HISTORY "umicom.developer.terminal.terminal_history"

/**
 * Provide the developer capability terminal history operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_history(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_HISTORY_H */
