/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_links.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal Links developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_LINKS_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_LINKS_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_LINKS "umicom.developer.terminal.terminal_links"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_links(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_LINKS_H */
