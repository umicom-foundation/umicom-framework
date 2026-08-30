/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_search.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal Search developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_SEARCH_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_SEARCH_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_SEARCH "umicom.developer.terminal.terminal_search"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_search(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_SEARCH_H */
