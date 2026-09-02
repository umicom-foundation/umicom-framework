/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_split.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal Split developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_SPLIT_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_SPLIT_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_SPLIT "umicom.developer.terminal.terminal_split"

/**
 * Provide the developer capability terminal split operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_split(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_SPLIT_H */
