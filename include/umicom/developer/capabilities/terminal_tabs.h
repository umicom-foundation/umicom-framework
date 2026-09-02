/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_tabs.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal Tabs developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_TABS_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_TABS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_TABS "umicom.developer.terminal.terminal_tabs"

/**
 * Provide the developer capability terminal tabs operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_tabs(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_TABS_H */
