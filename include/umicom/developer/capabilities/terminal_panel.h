/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/terminal_panel.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Terminal Panel developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_PANEL_H
#define UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_PANEL_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TERMINAL_PANEL "umicom.developer.experience.terminal_panel"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_panel(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TERMINAL_PANEL_H */
